#include "Framework/Components/Material.h"

namespace aurora {

	Material::Material(MaterialType materialType)
		: Component(ComponentType::MATERIAL),
		materialType(materialType) {
	}

	MaterialType Material::GetMaterialType() const {
		return materialType;
	}

}