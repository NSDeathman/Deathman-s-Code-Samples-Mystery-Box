//--------------------------------------------------------------------------------------
// File: MeshLoader.cpp
//
// Wrapper class for ID3DXMesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma warning(disable : 4995)
#include "meshloader.h"
#include <fstream>
using namespace std;
#pragma warning(default : 4995)

// Vertex declaration
D3DVERTEXELEMENT9 VERTEX_DECL[] = { {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
								   {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
								   {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
								   D3DDECL_END() };

//--------------------------------------------------------------------------------------
CMeshLoader::CMeshLoader()
{
	m_pd3dDevice = NULL;
	m_pMesh = NULL;

	ZeroMemory(m_strMediaDir, sizeof(m_strMediaDir));
}

//--------------------------------------------------------------------------------------
CMeshLoader::~CMeshLoader()
{
	Destroy();
}

//--------------------------------------------------------------------------------------
void CMeshLoader::Destroy()
{
	concurrency::parallel_for(int(0), m_Materials.GetSize(), [&](int iMaterial)
		{
			Material* pMaterial = m_Materials.GetAt(iMaterial);

			// Avoid releasing the same texture twice
			for (int x = iMaterial + 1; x < m_Materials.GetSize(); x++)
			{
				Material* pCur = m_Materials.GetAt(x);
				if (pCur->pTextureAlbedo == pMaterial->pTextureAlbedo)
					pCur->pTextureAlbedo = NULL;
			}

			SAFE_RELEASE(pMaterial->pTextureAlbedo);
			SAFE_DELETE(pMaterial);
		});

	m_Materials.RemoveAll();
	m_Vertices.RemoveAll();
	m_Indices.RemoveAll();
	m_Attributes.RemoveAll();

	SAFE_RELEASE(m_pMesh);
	m_pd3dDevice = NULL;
}

//--------------------------------------------------------------------------------------
HRESULT CMeshLoader::Create(IDirect3DDevice9* pd3dDevice, const CHAR* strFilePath, const CHAR* strFilename)
{
	Print("Creating mesh from OBJ file with name: %s", strFilename);

	HRESULT hr;
	CHAR str[MAX_PATH] = { 0 };

	// Start clean
	Destroy();

	// Store the device pointer
	m_pd3dDevice = pd3dDevice;

	// Load the vertex buffer, index buffer, and subset information from a file. In this case,
	// an .obj file was chosen for simplicity, but it's meant to illustrate that ID3DXMesh objects
	// can be filled from any mesh file format once the necessary data is extracted from file.
	hr = LoadGeometryFromOBJ(strFilePath, strFilename);

	ASSERT(SUCCEEDED(hr), "Can't load geometry from OBJ: %s", strFilePath + strFilename);

	// Set the current directory based on where the mesh was found
	CHAR strOldDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, strOldDir);
	SetCurrentDirectory(m_strMediaDir);

	// Load material textures
	for (int iMaterial = 0; iMaterial < m_Materials.GetSize(); iMaterial++)
	{
		Material* pMaterial = m_Materials.GetAt(iMaterial);
		if (pMaterial->strTextureAlbedo[0])
		{
			// Avoid loading the same texture twice
			bool bFound = false;
			for (int x = 0; x < iMaterial; x++)
			{
				Material* pCur = m_Materials.GetAt(x);
				if (0 == strcmp(pCur->strTextureAlbedo, pMaterial->strTextureAlbedo))
				{
					bFound = true;
					pMaterial->pTextureAlbedo = pCur->pTextureAlbedo;
					break;
				}
			}

			// Not found, load the texture
			if (!bFound)
			{
				Print("Loading albedo texture: %s", pMaterial->strTextureAlbedo);
				hr = D3DXCreateTextureFromFile(pd3dDevice, pMaterial->strTextureAlbedo, &(pMaterial->pTextureAlbedo));
				ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureAlbedo);
			}
		}
	}

	// Restore the original current directory
	SetCurrentDirectory(strOldDir);

	// Create the encapsulated mesh
	ID3DXMesh* pMesh = NULL;
	hr = D3DXCreateMesh(m_Indices.GetSize() / 3,
		m_Vertices.GetSize(),
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		VERTEX_DECL, pd3dDevice, &pMesh);

	ASSERT(SUCCEEDED(hr), "Can't create mesh from OBJ: %s", strFilePath + strFilename);

	// Copy the vertex data
	VERTEX* pVertex;
	hr = pMesh->LockVertexBuffer(0, (void**)&pVertex);
	ASSERT(SUCCEEDED(hr), "Can't Lock Vertex Buffer in create mesh from OBJ: %s", strFilePath + strFilename);
	memcpy(pVertex, m_Vertices.GetData(), m_Vertices.GetSize() * sizeof(VERTEX));
	pMesh->UnlockVertexBuffer();
	m_Vertices.RemoveAll();

	// Copy the index data
	DWORD* pIndex;
	hr = pMesh->LockIndexBuffer(0, (void**)&pIndex);
	ASSERT(SUCCEEDED(hr), "Can't Lock Index Buffer in create mesh from OBJ: %s", strFilePath + strFilename);
	memcpy(pIndex, m_Indices.GetData(), m_Indices.GetSize() * sizeof(DWORD));
	pMesh->UnlockIndexBuffer();
	m_Indices.RemoveAll();

	// Copy the attribute data
	DWORD* pSubset;
	hr = pMesh->LockAttributeBuffer(0, &pSubset);
	ASSERT(SUCCEEDED(hr), "Can't Lock Attribute Buffer in create mesh from OBJ: %s", strFilePath + strFilename);
	memcpy(pSubset, m_Attributes.GetData(), m_Attributes.GetSize() * sizeof(DWORD));
	pMesh->UnlockAttributeBuffer();
	m_Attributes.RemoveAll();

	// Reorder the vertices according to subset and optimize the mesh for this graphics
	// card's vertex cache. When rendering the mesh's triangle list the vertices will
	// cache hit more often so it won't have to re-execute the vertex shader.
	DWORD* aAdjacency = new DWORD[pMesh->GetNumFaces() * 3L];

	ASSERT((aAdjacency != NULL), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename);

	hr = pMesh->GenerateAdjacency(1e-6f, aAdjacency);

	ASSERT(SUCCEEDED(hr), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename);

	hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, aAdjacency, NULL, NULL, NULL);

	ASSERT(SUCCEEDED(hr), "Can't Optimize Inplace in create mesh from OBJ: %s", strFilePath + strFilename);

	SAFE_DELETE_ARRAY(aAdjacency);

	m_pMesh = pMesh;

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMeshLoader::LoadGeometryFromOBJ(const CHAR* strFilepath, const CHAR* strFileName)
{
	CHAR strMaterialFilename[MAX_PATH] = { 0 };
	HRESULT hr;

	// Create temporary storage for the input data. Once the data has been loaded into
	// a reasonable format we can create a D3DXMesh object and load it with the mesh data.
	CGrowableArray<D3DXVECTOR3> Positions;
	CGrowableArray<D3DXVECTOR2> TexCoords;
	CGrowableArray<D3DXVECTOR3> Normals;

	DWORD dwCurSubset = 0;

	// File input
	CHAR strCommand[256] = { 0 };
	ifstream InFile((std::string)strFilepath + strFileName);
	if (!InFile)
	{
		ERROR_MESSAGE("Can't open ifstream in create mesh from OBJ: %s", strFilepath + strFilename);
		return E_FAIL;
	}

	for (;;)
	{
		InFile >> strCommand;
		if (!InFile)
			break;

		if (0 == strcmp(strCommand, "#"))
		{
			// Comment
		}
		else if (0 == strcmp(strCommand, "v"))
		{
			// Vertex Position
			float x, y, z;
			InFile >> x >> y >> z;
			Positions.Add(D3DXVECTOR3(-x, y, z));
		}
		else if (0 == strcmp(strCommand, "vt"))
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			TexCoords.Add(D3DXVECTOR2(u, -v));
		}
		else if (0 == strcmp(strCommand, "vn"))
		{
			// Vertex Normal
			float x, y, z;
			InFile >> x >> y >> z;
			Normals.Add(D3DXVECTOR3(x, y, z));
		}
		else if (0 == strcmp(strCommand, "f"))
		{
			// Face
			UINT iPosition, iTexCoord, iNormal;
			VERTEX vertex;

			for (UINT iFace = 0; iFace < 3; iFace++)
			{
				ZeroMemory(&vertex, sizeof(VERTEX));

				// OBJ format uses 1-based arrays
				InFile >> iPosition;
				vertex.position = Positions[iPosition - 1];

				if ('/' == InFile.peek())
				{
					InFile.ignore();

					if ('/' != InFile.peek())
					{
						// Optional texture coordinate
						InFile >> iTexCoord;
						vertex.texcoord = TexCoords[iTexCoord - 1];
					}

					if ('/' == InFile.peek())
					{
						InFile.ignore();

						// Optional vertex normal
						InFile >> iNormal;
						vertex.normal = Normals[iNormal - 1];
					}
				}

				// If a duplicate vertex doesn't exist, add this vertex to the Vertices
				// list. Store the index in the Indices array. The Vertices and Indices
				// lists will eventually become the Vertex Buffer and Index Buffer for
				// the mesh.
				DWORD index = AddVertex(iPosition, &vertex);
				if (index == (DWORD)-1)
					return E_OUTOFMEMORY;

				m_Indices.Add(index);
			}
			m_Attributes.Add(dwCurSubset);
		}
		else if (0 == strcmp(strCommand, "mtllib"))
		{
			// Material library
			InFile >> strMaterialFilename;
		}
		else if (0 == strcmp(strCommand, "usemtl"))
		{
			// Material
			CHAR strName[MAX_PATH] = { 0 };
			InFile >> strName;

			Print("Processing material %s", strName);

			bool bFound = false;
			for (int iMaterial = 0; iMaterial < m_Materials.GetSize(); iMaterial++)
			{
				Material* pCurMaterial = m_Materials.GetAt(iMaterial);
				if (0 == strcmp(pCurMaterial->strName, strName))
				{
					bFound = true;
					dwCurSubset = iMaterial;
					break;
				}
			}

			if (!bFound)
			{
				Material* pMaterial = new Material();
				if (pMaterial == NULL)
					return E_OUTOFMEMORY;

				dwCurSubset = m_Materials.GetSize();

				InitMaterial(pMaterial);
				strcpy_s(pMaterial->strName, MAX_PATH - 1, strName);

				m_Materials.Add(pMaterial);
			}
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore(1000, '\n');
	}

	// Cleanup
	InFile.close();
	DeleteCache();

	// If an associated material file was found, read that in as well.
	if (strMaterialFilename[0])
	{
		hr = LoadMaterialsFromMTL(strFilepath, strMaterialFilename);

		if (FAILED(hr))
			Print("Can't Load Materials From MTL in load mesh from OBJ: %s", strMaterialFilename);
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
DWORD CMeshLoader::AddVertex(UINT hash, VERTEX* pVertex)
{
	// If this vertex doesn't already exist in the Vertices list, create a new entry.
	// Add the index of the vertex to the Indices list.
	bool bFoundInList = false;
	DWORD index = 0;

	// Since it's very slow to check every element in the vertex list, a hashtable stores
	// vertex indices according to the vertex position's index as reported by the OBJ file
	if ((UINT)m_VertexCache.GetSize() > hash)
	{
		CacheEntry* pEntry = m_VertexCache.GetAt(hash);
		while (pEntry != NULL)
		{
			VERTEX* pCacheVertex = m_Vertices.GetData() + pEntry->index;

			// If this vertex is identical to the vertex already in the list, simply
			// point the index buffer to the existing vertex
			if (0 == memcmp(pVertex, pCacheVertex, sizeof(VERTEX)))
			{
				bFoundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	// Vertex was not found in the list. Create a new entry, both within the Vertices list
	// and also within the hashtable cache
	if (!bFoundInList)
	{
		// Add to the Vertices list
		index = m_Vertices.GetSize();
		m_Vertices.Add(*pVertex);

		// Add this to the hashtable
		CacheEntry* pNewEntry = new CacheEntry;
		if (pNewEntry == NULL)
			return (DWORD)-1;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// Grow the cache if needed
		while ((UINT)m_VertexCache.GetSize() <= hash)
		{
			m_VertexCache.Add(NULL);
		}

		// Add to the end of the linked list
		CacheEntry* pCurEntry = m_VertexCache.GetAt(hash);
		if (pCurEntry == NULL)
		{
			// This is the head element
			m_VertexCache.SetAt(hash, pNewEntry);
		}
		else
		{
			// Find the tail
			while (pCurEntry->pNext != NULL)
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}

//--------------------------------------------------------------------------------------
void CMeshLoader::DeleteCache()
{
	// Iterate through all the elements in the cache and subsequent linked lists
	concurrency::parallel_for(int(0), m_VertexCache.GetSize(), [&](int i)
		{
			CacheEntry* pEntry = m_VertexCache.GetAt(i);
			while (pEntry != NULL)
			{
				CacheEntry* pNext = pEntry->pNext;
				SAFE_DELETE(pEntry);
				pEntry = pNext;
			}
		});

	m_VertexCache.RemoveAll();
}

//--------------------------------------------------------------------------------------
HRESULT CMeshLoader::LoadMaterialsFromMTL(const CHAR* strFilePath, const CHAR* strFileName)
{
	Print("Loading material from MTL file with name: %s", strFileName);

	HRESULT hr = E_FAIL;

	// File input
	CHAR strCommand[256] = { 0 };
	ifstream InFile((std::string)strFilePath + strFileName);
	if (!InFile)
	{
		ERROR_MESSAGE("Can't open ifstream in create mesh from OBJ (MTL Loading)");
		Print("Can't open ifstream in create mesh from OBJ (MTL Loading): %s", (std::string)strFilePath + strFileName);
		return E_FAIL;
	}

	Material* pMaterial = NULL;

	for (;;)
	{
		InFile >> strCommand;
		if (!InFile)
			break;

		if (0 == strcmp(strCommand, "newmtl"))
		{
			// Switching active materials
			CHAR strName[MAX_PATH] = { 0 };
			InFile >> strName;

			pMaterial = NULL;
			for (int i = 0; i < m_Materials.GetSize(); i++)
			{
				Material* pCurMaterial = m_Materials.GetAt(i);
				if (0 == strcmp(pCurMaterial->strName, strName))
				{
					pMaterial = pCurMaterial;
					break;
				}
			}
		}

		// The rest of the commands rely on an active material
		if (pMaterial == NULL)
			continue;

		if (0 == strcmp(strCommand, "#"))
		{
			// Comment
		}
		else if (0 == strcmp(strCommand, "Ka"))
		{
			// Ambient color
			float r, g, b;
			InFile >> r >> g >> b;
			pMaterial->vAmbient = D3DXVECTOR3(r, g, b);
		}
		else if (0 == strcmp(strCommand, "Kd"))
		{
			// Diffuse color
			float r, g, b;
			InFile >> r >> g >> b;
			pMaterial->vDiffuse = D3DXVECTOR3(r, g, b);
		}
		else if (0 == strcmp(strCommand, "Ks"))
		{
			// Specular color
			float r, g, b;
			InFile >> r >> g >> b;
			pMaterial->vSpecular = D3DXVECTOR3(r, g, b);
		}
		else if (0 == strcmp(strCommand, "d") || 0 == strcmp(strCommand, "Tr"))
		{
			// Alpha
			InFile >> pMaterial->fAlpha;
		}
		else if (0 == strcmp(strCommand, "Ns"))
		{
			// Shininess
			int nShininess;
			InFile >> nShininess;
			pMaterial->nShininess = nShininess;
		}
		else if (0 == strcmp(strCommand, "illum"))
		{
			// Specular on/off
			int illumination;
			InFile >> illumination;
			pMaterial->bSpecular = (illumination == 2);
		}
		else if (0 == strcmp(strCommand, "map_Kd"))
		{
			// Texture
			InFile >> pMaterial->strTextureAlbedo;
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore(1000, '\n');
	}

	InFile.close();

	return S_OK;
}

//--------------------------------------------------------------------------------------
void CMeshLoader::InitMaterial(Material* pMaterial)
{
	ZeroMemory(pMaterial, sizeof(Material));

	pMaterial->vAmbient = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	pMaterial->vDiffuse = D3DXVECTOR3(0.8f, 0.8f, 0.8f);
	pMaterial->vSpecular = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMaterial->nShininess = 0;
	pMaterial->fAlpha = 1.0f;
	pMaterial->bSpecular = false;

	pMaterial->pTextureAlbedo = NULL;
}
