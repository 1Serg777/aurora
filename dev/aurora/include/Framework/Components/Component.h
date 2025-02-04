#pragma once

namespace aurora
{
	enum class ComponentType
	{
		Transform,
		Geometry,
		Material
	};

	class Actor;

	class Component
	{
	public:

		ComponentType GetComponentType() const;

		void AttachParentActor(Actor* actor);
		void DetachParentActor();

	protected:

		Component(ComponentType componentType);
		~Component() = default;

		Actor* parentActor{ nullptr };

	private:

		ComponentType componentType{};
	};
}