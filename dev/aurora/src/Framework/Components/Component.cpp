#include "Framework/Components/Component.h"

#include "Framework/Actor.h"

#include <cassert>

namespace aurora {

	Component::Component(ComponentType componentType)
		: componentType(componentType) {
	}

	ComponentType Component::GetComponentType() const {
		return componentType;
	}

	void Component::OnOwnerAttach(std::shared_ptr<Actor> ownerActor) {
		assert(this->ownerActor.expired() &&
			"Having multiple owners of the same component is prohibited!"
			"Did you call 'OnOwnerDetach'?");
		this->ownerActor = ownerActor;
	}
	void Component::OnOwnerDetach() {
		assert(!this->ownerActor.expired() &&
			"The component must have a valid owner actor!"
			"Did you call 'OnOwnerAttach'?");
		ownerActor.reset();
	}

}
