#pragma once

#include "Scene/Scene.h"

#include <memory>
#include <vector>

namespace aurora
{
	class SceneManager
	{
	public:

		void AddScene(std::shared_ptr<Scene> scene);
		void RemoveScene(std::shared_ptr<Scene> scene);

		void SetActiveScene(std::shared_ptr<Scene> scene);
		std::shared_ptr<Scene> GetActiveScene() const;

		const std::vector<std::shared_ptr<Scene>>& GetScenes() const;

	private:

		bool SceneExists(std::shared_ptr<Scene> scene) const;

		std::vector<std::shared_ptr<Scene>> scenes;

		std::shared_ptr<Scene> activeScene;
	};
}