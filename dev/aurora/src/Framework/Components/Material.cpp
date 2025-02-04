#include "Framework/Components/Material.h"

namespace aurora
{
	Material::Material(MaterialType materialType)
		: Component(ComponentType::Material),
		materialType(materialType)
	{
	}

	MaterialType Material::GetMaterialType() const
	{
		return materialType;
	}
}