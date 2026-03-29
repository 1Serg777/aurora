#include "Framework/Actor.h"

#include "Framework/Components/Geometry.h"

namespace aurora {

	Actor::Actor(std::string_view actorName)
		: actorName(actorName) {
	}

	bool Actor::Intersect(const numa::Ray& ray, ActorRayHit& rayHit) {
		std::shared_ptr<Geometry> geometry = GetComponent<Geometry>();
		if (!geometry) {
			rayHit.hit = false;
			rayHit.hitActor = nullptr;
			return false;
		}
		rayHit.hit = geometry->Intersect(ray, rayHit);
		if (rayHit.hit) {
			rayHit.hitActor = this;
		}
		return rayHit.hit;
	}

}