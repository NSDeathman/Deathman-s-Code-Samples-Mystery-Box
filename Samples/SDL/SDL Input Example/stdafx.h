///////////////////////////////////////////////////////////////
// Created: 20.02.2025
// Author: NS_Deathman
// Place your includes there
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
// Windows includes (You need to add $(WindowsSDK_IncludePath)
// to VS include paths of your project)
#include <iostream>
#include <ctime>
#include <windows.h>
///////////////////////////////////////////////////////////////
// Windows libraries
#pragma comment( lib, "winmm.lib")   
///////////////////////////////////////////////////////////////
// SDL Includes (You need to add $(SolutionDir)\Third-Party\Include\
// to VS include paths of your project) 
#include <SDL/SDL.h>
///////////////////////////////////////////////////////////////
// SDL Libraries (You need to add 
// $(SolutionDir)\Third-Party\Libraries\$(LibrariesArchitecture)\
// to VS include paths of your project) 
#pragma comment(lib, "SDL2.lib")
// Add 
// copy /Y "$(SolutionDir)\Third-Party\Libraries\$(LibrariesArchitecture)\SDL2.dll" "$(OutDir)SDL2.dll"
// To your post build events for auto copying dll to your bin folder
///////////////////////////////////////////////////////////////
