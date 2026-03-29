#pragma once

#include "Framework/Components/Component.h"

#include "Intersect.h"
#include "Ray.h"
#include "Vec.hpp"

#include <limits>

namespace aurora {

	enum class GeometryType {
		CIRCLE,
		MESH,
		PLANE,
		SPHERE,
		TRIANGLE,
		COUNT
	};

	class Geometry;

	struct GeometryRayHit {
		numa::Ray   hitRay{numa::Vec3{0.0f}, numa::Vec3{0.0f}};

		numa::Vec3  hitPoint{0.0f};
		numa::Vec3  hitNormal{0.0f};
		numa::Vec2  hitUv{0.0f};

		float hitDistance{0.0f};

		GeometryType hitGeometryType{};

		bool hit{false};
		bool hitFrontFace{false};
	};

	class Geometry : public Component {
	public:
		Geometry(GeometryType geometryType);
		virtual bool Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) = 0;
		GeometryType GetGeometryType() const;

	private:
		GeometryType geometryType{};
	};

	class Plane : public Geometry {
	public:
		Plane();
		Plane(const numa::Vec2& dimensions);

		bool Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) override;

		const numa::Vec2& GetDimensions() const;

	private:
		// numa::Vec3 center{0.0f, 0.0f, 0.0f};
		numa::Vec2 dimensions{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
	};

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

		float radius{ 1.0 };
	};

}