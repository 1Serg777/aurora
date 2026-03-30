#pragma once

#include <memory>

namespace aurora {

	enum class ComponentType {
		TRANSFORM,
		GEOMETRY,
		MATERIAL,
		LIGHT,
		COUNT
	};

	class Actor;

	class Component {
	public:
		ComponentType GetComponentType() const;

		virtual void OnOwnerAttach(std::shared_ptr<Actor> ownerActor);
		virtual void OnOwnerDetach();

	protected:
		Component(ComponentType componentType);
		~Component() = default;

		std::weak_ptr<Actor> ownerActor;
		ComponentType componentType{};
	};

}