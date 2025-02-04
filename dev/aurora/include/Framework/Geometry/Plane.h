#pragma once

#include "Framework/Components/Geometry.h"

#include "Ray.h"
#include "Vec3.hpp"

namespace aurora
{
	class Plane : public Geometry
	{
	public:

		Plane();

		bool Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) override;
	};
}