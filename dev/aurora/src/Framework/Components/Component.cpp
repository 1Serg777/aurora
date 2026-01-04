#include "Framework/Components/Component.h"

namespace aurora {

	Component::Component(ComponentType componentType)
		: componentType(componentType) {
	}

	ComponentType Component::GetComponentType() const {
		return componentType;
	}

	void Component::AttachParentActor(Actor* actor) {
		this->parentActor = actor;
	}
	void Component::DetachParentActor() {
		parentActor = nullptr;
	}

}
