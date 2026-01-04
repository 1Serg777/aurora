#pragma once

#include "Framework/Components/Geometry.h"

#include "Ray.h"
#include "Vec.hpp"

namespace aurora {

	class Sphere : public Geometry {
	public:
		Sphere();
		Sphere(float radius);

		bool Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) override;

		float DistanceFromEdge(const numa::Vec3& point) const;
		// Result is a value in the range [0, 1]
		// 0 - the point is in the center of the sphere
		// 1 - the point is right on the edge of the sphere
		float DistanceFromEdgeNormalized(const numa::Vec3& point) const;

		float GetRadius() const;

	private:
		numa::Vec3 ComputeNormal(const numa::Vec3& pointOnSphere) const;

		float radius{1.0};
	};

}