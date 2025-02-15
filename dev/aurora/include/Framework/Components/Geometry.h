#pragma once

#include "Framework/Components/Component.h"

#include "Intersect.h"
#include "Ray.h"
#include "Vec3.hpp"

#include <limits>

namespace aurora
{
	enum class GeometryType
	{
		Sphere,
		Plane,
		Triangle,

		Mesh
	};

	class Geometry;

	struct GeometryRayHit
	{
		numa::Ray   hitRay{ numa::Vec3{ 0.0f }, numa::Vec3{ 0.0f } };

		numa::Vec3  hitPoint{ 0.0f };
		numa::Vec3  hitNormal{ 0.0f };
		numa::Vec2  hitUv{ 0.0f };

		float hitDistance{ 0.0f };

		GeometryType hitGeometryType{};

		bool hit{ false };
		bool hitFrontFace{ false };
	};

	class Geometry : public Component
	{
	public:

		Geometry(GeometryType geometryType);

		virtual bool Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) = 0;

		GeometryType GetGeometryType() const;

	private:

		GeometryType geometryType{};
	};
}