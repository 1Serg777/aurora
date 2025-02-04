#include "Framework/Geometry/Sphere.h"

#include "Framework/Actor.h"

#include "Intersect.h"

namespace aurora
{
	Sphere::Sphere()
		: Geometry(GeometryType::Sphere)
	{
	}
	Sphere::Sphere(float radius)
		: Geometry(GeometryType::Sphere),
		radius(radius)
	{
	}

	bool Sphere::Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit)
	{
		numa::Sphere sphere{
			parentActor->GetTransform()->GetWorldPosition(),
			this->radius
		};

		numa::RaySphereHit rayHit{};
		bool hit = numa::IntersectSphere(sphere, ray, rayHit);

		geometryHit.hitRay = ray;
		geometryHit.hitGeometryType = GeometryType::Sphere;
		geometryHit.hit = hit;

		// 'No hit' or the hit is behind
		if (!geometryHit.hit || rayHit.HitBehind())
		{
			geometryHit.hit = false;
		}

		// It's either a 'single' hit or a 'double' hit.

		geometryHit.hit = true;

		// One hit 'in front'
		if (rayHit.SingleHit() && rayHit.HitInFront())
		{
			geometryHit.hitPoint = rayHit.hitPointT1;
			geometryHit.hitNormal = rayHit.hitNormalT1;
			geometryHit.hitUv = rayHit.hitUvT1;
			geometryHit.hitDistance = rayHit.hitDistanceT1;
		}
		else /* if (rayHit.DoubleHit()) */
		{
			// The hit is either 'in front' or 'inside'

			// Two hits 'in front'
			if (rayHit.HitInFront())
			{
				geometryHit.hitPoint = rayHit.hitPointT2;
				geometryHit.hitNormal = rayHit.hitNormalT2;
				geometryHit.hitUv = rayHit.hitUvT2;
				geometryHit.hitDistance = rayHit.hitDistanceT2;
			}
			// Two hits 'inside'
			else if (rayHit.HitInside())
			{
				geometryHit.hitPoint = rayHit.hitPointT1;
				geometryHit.hitNormal = rayHit.hitNormalT1;
				geometryHit.hitUv = rayHit.hitUvT1;
				geometryHit.hitDistance = rayHit.hitDistanceT1;
			}
		}

		return geometryHit.hit;
	}

	float Sphere::DistanceFromEdgeNormalized(const numa::Vec3& point) const
	{
		const numa::Vec3& point_radius_vector = point - parentActor->GetTransform()->GetWorldPosition();
		float point_radius = numa::Length(point_radius_vector);
		float point_radius_normalized = point_radius / radius;
		return point_radius_normalized;
	}

	float Sphere::GetRadius() const
	{
		return radius;
	}

	numa::Vec3 Sphere::ComputeNormal(const numa::Vec3& pointOnSphere) const
	{
		return numa::Normalize(pointOnSphere - parentActor->GetTransform()->GetWorldPosition());
	}
}