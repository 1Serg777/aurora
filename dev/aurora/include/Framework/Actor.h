#pragma once

#include "Framework/Components/Component.h"
#include "Framework/Components/Geometry.h"

#include "Ray.h"

#include <string_view>
#include <string>
#include <memory>
#include <unordered_map>

namespace aurora {

	class Actor;

	struct ActorRayHit : public GeometryRayHit {
		Actor* hitActor{nullptr};
	};

	class Actor : public std::enable_shared_from_this<Actor> {
	public:
		Actor(std::string_view actorName);

		virtual bool Intersect(const numa::Ray& ray, ActorRayHit& actorRayHit);

		template <typename T>
		void AttachComponent(std::shared_ptr<T> component) {
			auto find = components.find(T::COMPONENT_TYPE);
			if (find != components.end())
				find->second->OnOwnerDetach();
			component->OnOwnerAttach(Actor::shared_from_this());
			components[T::COMPONENT_TYPE] = component;
		}
		template <typename T>
		void DetachComponent() {
			auto find = components.find(T::COMPONENT_TYPE);
			if (find == components.end())
				return;
			find->second->OnOwnerDetach();
			components.erase(T::COMPONENT_TYPE);
		}

		template <typename T>
		bool HasComponent() const {
			return components.find(T::COMPONENT_TYPE) != components.end();
		}
		template <typename T>
		std::shared_ptr<T> GetComponent() const {
			auto find = this->components.find(T::COMPONENT_TYPE);
			if (find != this->components.end()) {
				return std::static_pointer_cast<T>(find->second);
			}
			return std::shared_ptr<T>();
		}

	private:
		std::unordered_map<ComponentType, std::shared_ptr<Component>> components;
		std::string actorName;
	};

}