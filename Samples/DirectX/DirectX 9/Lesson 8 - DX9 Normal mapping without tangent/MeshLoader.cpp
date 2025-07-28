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
D3DVERTEXELEMENT9 VERTEX_DECL[] = 
{ 
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END() 
};

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
    Print("\n"); // Print a new line for clarity in output log.
    Print("Creating mesh from OBJ file with name: %s", strFilename); // Log the filename being processed.

    HRESULT hr; // Declare a variable to store HRESULT return codes for error handling.
    CHAR str[MAX_PATH] = { 0 }; // Buffer for path strings, initialized to zero.

    // Start clean by destroying any previously loaded mesh or resources, ensuring no memory leaks occur.
    Destroy();

    // Store the device pointer for later use. This is essential for rendering operations.
    m_pd3dDevice = pd3dDevice;

    // Load vertex buffer, index buffer, and subset information from the specified OBJ file.
    Print("Reading mesh data from file");
    hr = LoadGeometryFromOBJ(strFilePath, strFilename); // Call the function to load mesh geometry.

    // Check if loading geometry was successful; assert and log an error if failed.
    ASSERT(SUCCEEDED(hr), "Can't load geometry from OBJ: %s", strFilePath + strFilename);

    // Save the current directory to restore it later after texture loading.
    CHAR strOldDir[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, strOldDir); // Retrieve the current working directory.
    SetCurrentDirectory(m_strMediaDir); // Change the current directory to where media files are located.

    // Load material textures associated with the mesh.
    Print("Loading material textures");
    for (int iMaterial = 0; iMaterial < m_Materials.GetSize(); iMaterial++) // Loop through each material.
    {
        Material* pMaterial = m_Materials.GetAt(iMaterial); // Get the current material structure.
        if (pMaterial->strTextureAlbedo[0]) // Check if there is an albedo texture defined.
        {
            // Avoid loading the same texture twice by checking previously loaded materials.
            bool bFound = false;
            for (int x = 0; x < iMaterial; x++)
            {
                Material* pCur = m_Materials.GetAt(x); // Compare with earlier materials.
                if (0 == strcmp(pCur->strTextureAlbedo, pMaterial->strTextureAlbedo)) // If found, reuse the texture.
                {
                    bFound = true; // Mark as found.
                    pMaterial->pTextureAlbedo = pCur->pTextureAlbedo; // Assign existing texture pointer.
                    break; // No need to continue searching.
                }
            }

            // If texture not found, load it from file.
            if (!bFound)
            {
                Print("Loading albedo texture: %s", pMaterial->strTextureAlbedo); // Log texture loading.
                hr = D3DXCreateTextureFromFile(pd3dDevice, pMaterial->strTextureAlbedo, &(pMaterial->pTextureAlbedo)); // Load texture into Direct3D.
                ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureAlbedo); // Check for success and assert if failed.
            }
        }
    }

    // Restore the original current directory after loading textures to avoid side effects on other file operations.
    SetCurrentDirectory(strOldDir);

    Print("Creating mesh from readed data");

	// Create the encapsulated mesh object using D3DX.
	ID3DXMesh* pMesh = NULL; // Pointer for the mesh, initialized to NULL.
	hr = D3DXCreateMesh( m_Indices.GetSize() / 3,			// Number of faces is indices count divided by 3 (each triangle consists of 3 indices).
						 m_Vertices.GetSize(),				// Total number of vertices to be included in the mesh.
						 D3DXMESH_MANAGED | D3DXMESH_32BIT, // Specify managed memory usage and indicate the use of 32-bit indices.
						 VERTEX_DECL,						// The vertex declaration structure that defines the format of the vertex data.
						 pd3dDevice,						// The Direct3D device used for rendering.
						 &pMesh);							// Output parameter that will receive the created mesh.

	 // Check if the mesh was created successfully; assert and log an error message if it failed.
	ASSERT(SUCCEEDED(hr), "Can't create mesh from OBJ: %s", strFilePath + strFilename);

	// Copy the vertex data into the mesh's vertex buffer.
	VERTEX* pVertex; // Pointer to access the vertex data within the mesh.
	hr = pMesh->LockVertexBuffer(0, (void**)&pVertex); // Lock the vertex buffer to allow writing of vertex data.
	ASSERT(SUCCEEDED(hr), "Can't Lock Vertex Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check lock success.
	memcpy(pVertex, m_Vertices.GetData(), m_Vertices.GetSize() * sizeof(VERTEX)); // Copy the vertex data from the source to the buffer.
	pMesh->UnlockVertexBuffer(); // Unlock the vertex buffer after copying data.
	m_Vertices.RemoveAll(); // Clear the original vertex data array to free up resources.

	// Copy the index data into the mesh's index buffer.
	DWORD* pIndex; // Pointer for accessing the index data.
	hr = pMesh->LockIndexBuffer(0, (void**)&pIndex); // Lock the index buffer for writing.
	ASSERT(SUCCEEDED(hr), "Can't Lock Index Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure lock succeeded.
	memcpy(pIndex, m_Indices.GetData(), m_Indices.GetSize() * sizeof(DWORD)); // Copy the index data.
	pMesh->UnlockIndexBuffer(); // Unlock the index buffer after copying.
	m_Indices.RemoveAll(); // Clear the original index data array to free up resources.

	// Copy the attribute data into the mesh's attribute buffer.
	DWORD* pSubset; // Pointer for accessing the attribute data.
	hr = pMesh->LockAttributeBuffer(0, &pSubset); // Lock the attribute buffer for writing.
	ASSERT(SUCCEEDED(hr), "Can't Lock Attribute Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure lock succeeded.
	memcpy(pSubset, m_Attributes.GetData(), m_Attributes.GetSize() * sizeof(DWORD)); // Copy attribute data associated with the mesh.
	pMesh->UnlockAttributeBuffer(); // Unlock the attribute buffer after copying.
	m_Attributes.RemoveAll(); // Clear the original attribute data array to free up resources.

	Print("Raw mesh created");

	Print("Create adjacency for mesh");

	// Reorder the vertices according to subset and optimize the mesh for the graphics card's vertex cache.
	// This improves performance by allowing efficient access to vertex data during rendering.
	DWORD* aAdjacency = new DWORD[pMesh->GetNumFaces() * 3L]; // Allocate an array to store adjacency information for the faces.

	ASSERT((aAdjacency != NULL), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure allocation was successful.

	hr = pMesh->GenerateAdjacency(1e-6f, aAdjacency); // Generate adjacency information for the mesh.

	ASSERT(SUCCEEDED(hr), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check for success of adjacency generation.

	Print("Optimize mesh");

	hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, aAdjacency, NULL, NULL, NULL); // Optimize the mesh in-place for better rendering performance.

	ASSERT(SUCCEEDED(hr), "Can't Optimize Inplace in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check for successful optimization.

	SAFE_DELETE_ARRAY(aAdjacency); // Clean up the adjacency array to prevent memory leaks.

	Print("Create tangents for mesh"); // Log that the tangent generation process for the mesh is starting.

	// Calculate tangent vectors for the mesh. Tangents are essential for proper normal mapping in shader programs.
	// The parameters below define how the tangents will be computed.
	// - D3DDECLUSAGE_TEXCOORD and 0: Specify the texture coordinate stream to use.
	// - D3DX_DEFAULT: Use default settings for tangent computation.
	// - D3DDECLUSAGE_TANGENT and 0: Define where to store the computed tangents in the vertex buffer.
	// - D3DDECLUSAGE_NORMAL and 0: Specify the normal stream to use for tangent computation.
	// - D3DXTANGENT_CALCULATE_NORMALS: Indicates that normals should be recalculated if they are not available.
	// - NULL: Placeholder for additional output parameters.
	// - 0.01f, 0.25f, 0.01f: Parameters influencing the tangent computation behavior (e.g., precision).
	D3DXComputeTangentFrameEx(	pMesh,
								D3DDECLUSAGE_TEXCOORD,
								0,
								D3DX_DEFAULT,
								0,
								D3DDECLUSAGE_TANGENT,
								0,
								D3DDECLUSAGE_NORMAL,
								0,
								D3DXTANGENT_CALCULATE_NORMALS,
								NULL,
								0.01f,
								0.25f,
								0.01f,
								&m_pMesh,
								NULL );

	Print("Mesh successfully created!"); // Log that the mesh creation process has completed successfully.

	Print("Mesh statistic:"); // Log header for mesh statistics output.
	Print("Vertices count: %d", m_pMesh->GetNumVertices()); // Output the number of vertices in the mesh.
	Print("Faces count: %d", m_pMesh->GetNumFaces()); // Output the number of faces (triangles) in the mesh.
	Print("Materials count: %d", m_Materials.GetSize()); // Output the number of materials associated with the mesh.
	Print("\n"); // Print a newline for better readability in the output logs.

	return S_OK; // Return success status to indicate that the mesh creation operation completed without errors.
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
			Positions.Add(D3DXVECTOR3(x, y, z));
		}
		else if (0 == strcmp(strCommand, "vt"))
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			TexCoords.Add(D3DXVECTOR2(u, v));
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
    // This function returns the index of the vertex in the list.
    bool bFoundInList = false; // Flag to indicate if the vertex was found.
    DWORD index = 0;           // Variable to store the index of the vertex.

    // Since it's inefficient to check every element in the vertex list,
    // a hashtable (m_VertexCache) stores vertex indices based on the hash value.
    if ((UINT)m_VertexCache.GetSize() > hash)
    {
        CacheEntry* pEntry = m_VertexCache.GetAt(hash); // Get the linked list entry at the hashed index.
        while (pEntry != NULL) // Traverse the linked list of cached entries.
        {
            // Access the cached vertex using its index.
            VERTEX* pCacheVertex = m_Vertices.GetData() + pEntry->index;

            // Compare the current vertex with the cached vertex.
            // If they are identical, point to the existing index.
            if (0 == memcmp(pVertex, pCacheVertex, sizeof(VERTEX)))
            {
                bFoundInList = true; // Set flag to true to indicate a match was found.
                index = pEntry->index; // Store the index of the existing vertex.
                break; // Exit the loop since we found the vertex.
            }

            pEntry = pEntry->pNext; // Move to the next entry in the linked list.
        }
    }

    // If the vertex was not found in the list, create a new entry.
    if (!bFoundInList)
    {
        // Add the new vertex to the Vertices list.
        index = m_Vertices.GetSize(); // Get the current size, which equals the new index.
        m_Vertices.Add(*pVertex); // Add the vertex to the list.

        // Create a new cache entry for the new vertex.
        CacheEntry* pNewEntry = new CacheEntry;
        if (pNewEntry == NULL) // Check for memory allocation failure.
            return (DWORD)-1; // Return an error code if memory allocation fails.

        pNewEntry->index = index; // Set the index for the new entry.
        pNewEntry->pNext = NULL; // Initialize the next pointer to NULL.

        // Grow the vertex cache array if needed to accommodate the hash.
        while ((UINT)m_VertexCache.GetSize() <= hash)
        {
            m_VertexCache.Add(NULL); // Increase the size of the cache.
        }

        // Add the new entry to the end of the linked list at the hash index.
        CacheEntry* pCurEntry = m_VertexCache.GetAt(hash); // Get the current head of the list.
        if (pCurEntry == NULL)
        {
            // If there is no entry, the new entry becomes the head of the list.
            m_VertexCache.SetAt(hash, pNewEntry);
        }
        else
        {
            // Traverse to the end of the linked list to append the new entry.
            while (pCurEntry->pNext != NULL)
            {
                pCurEntry = pCurEntry->pNext; // Move to the next entry.
            }

            pCurEntry->pNext = pNewEntry; // Link the new entry at the end of the list.
        }
    }

    return index; // Return the index of the vertex (either found or newly added).
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
