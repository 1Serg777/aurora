#include "Framework/Gradient.h"

namespace aurora {

	Gradient::Gradient(const numa::Vec3& color1, const numa::Vec3& color2)
		: color1(color1), color2(color2) {
	}

	numa::Vec3 Gradient::GetColor(float t) const {
		return (1.0f - t) * color1 + t * color2;
	}

}
