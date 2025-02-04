#include "Framework/Actor.h"

namespace aurora
{
	Actor::Actor(std::string_view actorName)
		: actorName(actorName)
	{
	}

	bool Actor::Intersect(const numa::Ray& ray, ActorRayHit& rayHit)
	{
		if (!geometry)
		{
			rayHit.hit = false;
			rayHit.hitActor = nullptr;
			return false;
		}

		bool hit = geometry->Intersect(ray, rayHit);
		if (hit)
		{
			rayHit.hitActor = this;
		}

		return hit;
	}

	void Actor::SetGeometry(std::shared_ptr<Geometry> geometry)
	{
		this->geometry = geometry;
		geometry->AttachParentActor(this);
	}
	bool Actor::HasGeometry() const
	{
		if (geometry)
			return true;
		return false;
	}
	Geometry* Actor::GetGeometry() const
	{
		return this->geometry.get();
	}

	void Actor::SetMaterial(std::shared_ptr<Material> material)
	{
		this->material = material;
		material->AttachParentActor(this);
	}
	bool Actor::HasMaterial() const
	{
		if (material)
			return true;
		return false;
	}
	Material* Actor::GetMaterial() const
	{
		return this->material.get();
	}

	void Actor::SetTransform(std::shared_ptr<Transform> transform)
	{
		this->transform = transform;
		transform->AttachParentActor(this);
	}
	bool Actor::HasTransform() const
	{
		if (transform)
			return true;
		return false;
	}
	Transform* Actor::GetTransform() const
	{
		return this->transform.get();
	}
}