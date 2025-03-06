#include "Scene/Scene.h"

#include <limits>

namespace aurora
{
	Scene::Scene(std::string_view sceneName)
		: sceneName(sceneName)
	{
	}

	bool Scene::IntersectClosest(const numa::Ray& ray, ActorRayHit& rayHit) const
	{
		float closest_distance = std::numeric_limits<float>::max();

		for (auto& actor : actors)
		{
			ActorRayHit hit{};
			if (actor->Intersect(ray, hit))
			{
				if (hit.hitDistance < closest_distance)
				{
					closest_distance = hit.hitDistance;
					rayHit = hit;
				}
			}
		}

		/*
		if (atmosphere)
		{
			RayHit hit{};
			if (atmosphere->Intersect(ray, hit))
			{
				if (hit.hitDistanceClose < closest_distance)
				{
					closest_distance = hit.hitDistanceClose;
					rayHit = hit;
				}
			}
		}
		*/

		return closest_distance != std::numeric_limits<float>::max();
	}
	bool Scene::IntersectLights(const numa::Vec3& p, LightSampleBundle& lightBundle)
	{
		bool anyLightInView{ false };
		for (auto& light : lights)
		{
			// Sample the light source

			LightSampleData lightSample{};
			light->Sample(p, lightSample);

			// Create a ray toward the light source

			numa::Ray lightRay{
				p, // 'bias' should be handled elsewhere!
				lightSample.wi
			};

			// Find if there's anything blocking the the path

			float distanceToLight = numa::Length(lightSample.pos - p);

			bool blocking{ false };
			for (auto& actor : actors)
			{
				ActorRayHit hit{};
				if (actor->Intersect(lightRay, hit))
				{
					if (hit.hitDistance < distanceToLight)
					{
						blocking = true;
						break;
					}
				}
			}

			if (!blocking)
			{
				lightBundle.AddLightSample(lightSample);
				anyLightInView = true;
			}
		}
		return anyLightInView;
	}

	void Scene::AddActor(std::shared_ptr<Actor> actor)
	{
		actors.push_back(actor);
	}
	void Scene::AddLight(std::shared_ptr<DirectionalLight> light)
	{
		dirLight = light.get();
		lights.push_back(light);
	}

	void Scene::SetAtmosphere(std::shared_ptr<Atmosphere> atmosphere)
	{
		this->atmosphere = atmosphere;
	}
	void Scene::SetCamera(std::shared_ptr<Camera> camera)
	{
		this->camera = camera;
	}

	const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const
	{
		return actors;
	}
	const std::vector<std::shared_ptr<Light>>& Scene::GetLights() const
	{
		return lights;
	}

	Atmosphere* Scene::GetAtmosphere() const
	{
		return atmosphere.get();
	}

	Camera* Scene::GetCamera() const
	{
		return camera.get();
	}

	DirectionalLight* Scene::GetDirectionalLight() const
	{
		return dirLight;
	}

	const std::string& Scene::GetSceneName() const
	{
		return sceneName;
	}
}