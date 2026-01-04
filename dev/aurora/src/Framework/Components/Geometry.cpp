#include "Framework/Components/Geometry.h"

namespace aurora {

	Geometry::Geometry(GeometryType geometryType)
		: Component(ComponentType::Geometry), geometryType(geometryType) {
	}
	GeometryType Geometry::GetGeometryType() const {
		return geometryType;
	}

}