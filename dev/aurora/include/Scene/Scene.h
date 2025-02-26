#pragma once

#include "Framework/Actor.h"
#include "Framework/Camera.h"
#include "Framework/Light.h"

#include "Ray.h"
#include "Vec3.hpp"

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
		bool IntersectLights(const numa::Vec3& p, LightSampleBundle& lightBundle);

		void AddActor(std::shared_ptr<Actor> actor);
		void AddCamera(std::shared_ptr<Camera> camera);
		void AddDirectionalLight(std::shared_ptr<DirectionalLight> light);

		const std::vector<std::shared_ptr<Actor>>& GetActors() const;
		const std::vector<std::shared_ptr<Light>>& GetLights() const;

		Camera* GetCamera() const;

		const std::string& GetSceneName() const;

	private:

		std::string sceneName;

		std::vector<std::shared_ptr<Actor>> actors;
		std::vector<std::shared_ptr<Light>> lights;
		
		std::shared_ptr<Camera> camera;
	};
}