#pragma once

#include "Framework/Components/Component.h"

#include "Vec.hpp"

namespace aurora {

	enum class MaterialType {
		DIELECTRIC,
		LAMBERTIAN,
		METAL,
		PARTICIPATING_MEDIUM
	};

	class Material : public Component {
	public:
		static constexpr ComponentType COMPONENT_TYPE = ComponentType::MATERIAL;

		MaterialType GetMaterialType() const;

		virtual numa::Vec3 Scatter(const numa::Vec3& wo, const numa::Vec3& N,
			                       numa::Vec3& brdf, float& pdf) const = 0;

	protected:
		Material(MaterialType materialType);
		~Material() = default;

		MaterialType materialType{};
	};

}