#pragma once

#include "Framework/Actor.h"
#include "Framework/Atmosphere.h"
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
		void AddLight(std::shared_ptr<DirectionalLight> light);

		void SetAtmosphere(std::shared_ptr<Atmosphere> atmosphere);
		void SetCamera(std::shared_ptr<Camera> camera);

		const std::vector<std::shared_ptr<Actor>>& GetActors() const;
		const std::vector<std::shared_ptr<Light>>& GetLights() const;

		Atmosphere* GetAtmosphere() const;
		Camera* GetCamera() const;
		DirectionalLight* GetDirectionalLight() const;

		const std::string& GetSceneName() const;

	private:

		std::string sceneName;

		std::vector<std::shared_ptr<Actor>> actors;
		std::vector<std::shared_ptr<Light>> lights;
		
		std::shared_ptr<Atmosphere> atmosphere;
		std::shared_ptr<Camera> camera;

		DirectionalLight* dirLight{ nullptr };
	};
}	