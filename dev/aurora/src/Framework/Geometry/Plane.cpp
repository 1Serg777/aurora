#include "Framework/Geometry/Plane.h"

#include "Framework/Actor.h"

#include "Intersect.h"

namespace aurora
{
	Plane::Plane()
		: Geometry(GeometryType::Plane)
	{
	}

	bool Plane::Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit)
	{
		geometryHit.hitRay = ray;

		numa::Vec3 planeNormal{ 0.0f, 1.0f, 0.0f }; // Should be retrieved from the transform's orientation matrix!
		numa::Vec3 planeOrigin = parentActor->GetTransform()->GetWorldPosition();

		numa::Plane plane{ planeNormal, planeOrigin };
		numa::RayPlaneHit rayPlaneHit{};
		bool hit = IntersectPlane(plane, ray, rayPlaneHit);

		geometryHit.hit = hit;

		if (rayPlaneHit.HitBehind())
		{
			geometryHit.hit = false;
		}
		else /* if (rayPlaneHit.HitInFront()) */
		{
			geometryHit.hitPoint = rayPlaneHit.hitPoint;
			geometryHit.hitNormal = rayPlaneHit.hitNormal;
			geometryHit.hitUv = rayPlaneHit.hitUv;
			geometryHit.hitDistance = rayPlaneHit.hitDistance;

			geometryHit.hitFrontFace = rayPlaneHit.hitFrontFace;
			geometryHit.hitGeometryType = GeometryType::Plane;
		}

		return geometryHit.hit;
	}
}