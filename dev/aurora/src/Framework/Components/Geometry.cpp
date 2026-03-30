#include "Framework/Components/Geometry.h"

#include "Framework/Actor.h"
#include "Framework/Components/Transform.h"

namespace aurora {

	Geometry::Geometry(GeometryType geometryType)
		: Component(ComponentType::GEOMETRY), geometryType(geometryType) {
	}
	GeometryType Geometry::GetGeometryType() const {
		return geometryType;
	}

	Plane::Plane()
		: Geometry(GeometryType::PLANE) {
	}
	Plane::Plane(const numa::Vec2& dimensions)
		: Geometry(GeometryType::PLANE), dimensions(dimensions) {
	}

	bool Plane::Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) {
		geometryHit.hitRay = ray;

		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return false;

		const numa::Mat3 rotMat = transform->GetRotationMatrix();
		const numa::Vec3& planeOrigin = transform->GetWorldPosition();
		numa::Vec3 planeNormal = transform->GetForwardAxis();

		numa::Plane plane{planeNormal, planeOrigin};
		numa::RayPlaneHit rayPlaneHit{};
		bool hit = IntersectPlane(plane, ray, rayPlaneHit);
		geometryHit.hit = hit;
		
		if (rayPlaneHit.HitBehind()) {
			geometryHit.hit = false;
		} else /* if (rayPlaneHit.HitInFront()) */ {
			// We know that the ray intersects the plane, but we haven't checked the boundaries.
			// If the dimensions of the plane are set to finite numbers, we'll have to make sure
			// that the ray stays within them.
			bool widthCheck{true};
			bool heightCheck{true};
			numa::Vec3 dP = rayPlaneHit.hitPoint - planeOrigin;
			if (std::isfinite(dimensions.x)) {
				numa::Vec3 right = transform->GetRightAxis();
				float x = numa::Dot(dP, right); // [-w/2; w/2]
				float xNorm = x + dimensions.x / 2.0f; // [0; w]
				widthCheck = xNorm >= 0.0f && xNorm <= dimensions.x;
			}
			if (std::isfinite(dimensions.y)) {
				numa::Vec3 up = transform->GetUpAxis();
				float y = numa::Dot(dP, up); // [-h/2, h/2]
				float yNorm = y + dimensions.y / 2.0f; // [0; h]
				heightCheck = yNorm >= 0.0f && yNorm <= dimensions.y;
			}
			geometryHit.hit = widthCheck && heightCheck;
			if (geometryHit.hit) {
				geometryHit.hitPoint = rayPlaneHit.hitPoint;
				geometryHit.hitNormal = rayPlaneHit.hitNormal;
				geometryHit.hitUv = rayPlaneHit.hitUv;
				geometryHit.hitDistance = rayPlaneHit.hitDistance;
				geometryHit.hitFrontFace = rayPlaneHit.hitFrontFace;
				geometryHit.hitGeometryType = GeometryType::PLANE;
			}
		}
		return geometryHit.hit;
	}

	const numa::Vec2& Plane::GetDimensions() const {
		return dimensions;
	}

	Sphere::Sphere()
		: Geometry(GeometryType::SPHERE) {
	}
	Sphere::Sphere(float radius)
		: Geometry(GeometryType::SPHERE), radius(radius) {
	}

	bool Sphere::Intersect(const numa::Ray& ray, GeometryRayHit& geometryHit) {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return false;

		numa::Sphere sphere{
			transform->GetWorldPosition(), this->radius
		};
		numa::RaySphereHit rayHit{};
		bool hit = numa::IntersectSphere(sphere, ray, rayHit);

		geometryHit.hitRay = ray;
		geometryHit.hitGeometryType = GeometryType::SPHERE;
		geometryHit.hit = hit;
		// 'No hit' or the hit is behind
		if (!geometryHit.hit || rayHit.HitBehind()) {
			geometryHit.hit = false;
			return geometryHit.hit;
		}
		// It's either a 'single' hit or a 'double' hit.
		geometryHit.hit = true;
		// One hit 'in front'
		if (rayHit.SingleHit() && rayHit.HitInFront()) {
			geometryHit.hitPoint = rayHit.hitPointT1;
			geometryHit.hitNormal = rayHit.hitNormalT1;
			geometryHit.hitUv = rayHit.hitUvT1;
			geometryHit.hitDistance = rayHit.hitDistanceT1;
			geometryHit.hitFrontFace = true;
		} else /* if (rayHit.DoubleHit()) */ {
			// The hit is either 'in front' or 'inside'
			// Two hits 'in front'
			if (rayHit.HitInFront()) {
				geometryHit.hitPoint = rayHit.hitPointT2;
				geometryHit.hitNormal = rayHit.hitNormalT2;
				geometryHit.hitUv = rayHit.hitUvT2;
				geometryHit.hitDistance = rayHit.hitDistanceT2;
				geometryHit.hitFrontFace = true;
			}  else if (rayHit.HitInside()) { // Two hits 'inside'
				geometryHit.hitPoint = rayHit.hitPointT1;
				geometryHit.hitNormal = rayHit.hitNormalT1;
				geometryHit.hitUv = rayHit.hitUvT1;
				geometryHit.hitDistance = rayHit.hitDistanceT1;
				geometryHit.hitFrontFace = false;
			}
		}
		return geometryHit.hit;
	}

	float Sphere::DistanceFromEdge(const numa::Vec3& point) const {
		return 0.0f;
	}
	float Sphere::DistanceFromEdgeNormalized(const numa::Vec3& point) const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return 0.0f;

		const numa::Vec3& point_radius_vector = point - transform->GetWorldPosition();
		float point_radius = numa::Length(point_radius_vector);
		float point_radius_normalized = point_radius / radius;
		return point_radius_normalized;
	}

	float Sphere::GetRadius() const {
		return radius;
	}

	numa::Vec3 Sphere::ComputeNormal(const numa::Vec3& pointOnSphere) const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f,1.0f,0.0f};

		return numa::Normalize(pointOnSphere - transform->GetWorldPosition());
	}

}