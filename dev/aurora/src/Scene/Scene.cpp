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
	void Scene::AddActor(std::shared_ptr<Actor> actor)
	{
		actors.push_back(actor);
	}
	void Scene::AddCamera(std::shared_ptr<Camera> camera)
	{
		this->camera = camera;
	}
	void Scene::AddDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight)
	{
		this->directionalLight = directionalLight;
	}

	const std::string& Scene::GetSceneName() const
	{
		return sceneName;
	}

	Camera* Scene::GetCamera() const
	{
		return camera.get();
	}
	DirectionalLight* Scene::GetDirectionalLight() const
	{
		return directionalLight.get();
	}
}