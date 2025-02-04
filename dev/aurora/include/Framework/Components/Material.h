#pragma once

#include "Framework/Components/Component.h"

namespace aurora
{
	enum class MaterialType
	{
		LAMBERTIAN,
		DIELECTRIC,
		METAL,

		PARTICIPATING_MEDIUM
	};

	class Material : public Component
	{
	public:

		MaterialType GetMaterialType() const;

	protected:

		Material(MaterialType materialType);
		~Material() = default;

		MaterialType materialType{};
	};
}