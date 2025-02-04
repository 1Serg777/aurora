#pragma once

#include "Framework/Actor.h"
#include "Framework/Camera.h"
#include "Framework/Light.h"

#include "Ray.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace aurora
{
	class Scene
	{
	public:

		Scene(std::string_view sceneName);

		bool IntersectClosest(const numa::Ray& ray, ActorRayHit& rayHit) const;

		void AddActor(std::shared_ptr<Actor> actor);
		void AddCamera(std::shared_ptr<Camera> camera);
		void AddDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight);

		const std::string& GetSceneName() const;

		Camera* GetCamera() const;
		DirectionalLight* GetDirectionalLight() const;

	private:

		std::string sceneName;

		std::vector<std::shared_ptr<Actor>> actors;

		std::shared_ptr<DirectionalLight> directionalLight;
		std::shared_ptr<Camera> camera;
	};
}