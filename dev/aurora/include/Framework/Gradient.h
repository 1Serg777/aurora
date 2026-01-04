#pragma once

#include "Vec.hpp"

namespace aurora {

	class Gradient {
	public:
		Gradient(const numa::Vec3& color1, const numa::Vec3& color2);
		numa::Vec3 GetColor(float t) const;

	private:
		numa::Vec3 color1{0.0f};
		numa::Vec3 color2{1.0f};
	};

}