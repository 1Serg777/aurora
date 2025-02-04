#include "Scene/SceneManager.h"

#include <algorithm>
#include <cassert>

namespace aurora
{
	void SceneManager::AddScene(std::shared_ptr<Scene> scene)
	{
		if (!SceneExists(scene))
			scenes.push_back(scene);
	}
	void SceneManager::RemoveScene(std::shared_ptr<Scene> scene)
	{
		auto pred = [&scene](const std::shared_ptr<Scene>& s) {
			return scene == s;
			};

		auto remove_iter = std::remove_if(
			scenes.begin(), scenes.end(), pred);

		scenes.erase(remove_iter, scenes.end());
	}

	void SceneManager::SetActiveScene(std::shared_ptr<Scene> scene)
	{
		if (!SceneExists(scene))
		{
			scenes.push_back(scene);
		}
		this->activeScene = scene;
	}
	std::shared_ptr<Scene> SceneManager::GetActiveScene() const
	{
		return activeScene;
	}

	const std::vector<std::shared_ptr<Scene>>& SceneManager::GetScenes() const
	{
		return scenes;
	}

	bool SceneManager::SceneExists(std::shared_ptr<Scene> scene) const
	{
		auto searchRes = std::find(scenes.begin(), scenes.end(), scene);
		return searchRes != scenes.end();
	}
}