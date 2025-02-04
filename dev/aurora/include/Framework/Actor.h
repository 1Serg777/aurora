#pragma once

#include "Framework/Components/Transform.h"
#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"

#include "Ray.h"

#include <string_view>
#include <string>
#include <memory>

namespace aurora
{
	class Actor;

	struct ActorRayHit : GeometryRayHit
	{
		Actor* hitActor{ nullptr };
	};

	class Actor
	{
	public:

		Actor(std::string_view actorName);

		virtual bool Intersect(const numa::Ray& ray, ActorRayHit& actorRayHit);

		void SetGeometry(std::shared_ptr<Geometry> geometry);
		bool HasGeometry() const;
		Geometry* GetGeometry() const;

		void SetMaterial(std::shared_ptr<Material> material);
		bool HasMaterial() const;
		Material* GetMaterial() const;

		void SetTransform(std::shared_ptr<Transform> transform);
		bool HasTransform() const;
		Transform* GetTransform() const;

	private:

		std::string actorName;

		std::shared_ptr<Geometry> geometry;
		std::shared_ptr<Material> material;
		std::shared_ptr<Transform> transform;
	};
}