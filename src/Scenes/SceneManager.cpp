#include "SceneManager.hpp"
#include <windows.h>
#include <iostream>
#include "../core/Debug.hpp"


SceneManager::SceneManager(std::string sceneDirectoryPath, std::string firstScene)
{
    if (sceneDirectoryPath.back() != '\\' && sceneDirectoryPath.back() != '/') 
    {
        sceneDirectoryPath += "\\";
    }

    std::string searchPath = sceneDirectoryPath + "*.*";

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open directory: " << sceneDirectoryPath << std::endl;
        return;
    }

    do 
    {
        std::string fileName = findFileData.cFileName;

        if (fileName == "." || fileName == "..") { continue; }
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }

        std::string fullPath = sceneDirectoryPath + fileName;

        size_t dotPos = fileName.find_last_of('.');
        std::string nameWithoutExt = (dotPos == std::string::npos) ? fileName : fileName.substr(0, dotPos);
        scenes[nameWithoutExt] = std::make_unique<Scene>(fullPath);

    } 
    while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    activeScene = firstScene;

    if (firstScene.size() > 0) 
    {
        if (scenes.find(firstScene) != scenes.end())
        {
            scenes[firstScene]->CreateScene();
        }
    }
}

void SceneManager::LoadScene(std::string sceneName)
{
    if (activeScene == sceneName) { return; }

    if (scenes.find(sceneName) == scenes.end()) 
    {
        Debug::Log(sceneName + " does not exist in scenelist");
        return;
    }

    if (scenes.find(activeScene) != scenes.end()) 
    {
        scenes[activeScene]->DeleteScene();
    }

    activeScene = sceneName;
    scenes[activeScene]->CreateScene();
}

void SceneManager::UpdateScene()
{
    if (scenes.find(activeScene) != scenes.end())
    {
        scenes[activeScene]->UpdateScene();
    }
}
