#pragma once

#include "Framework/Components/Component.h"

#include "Vec.hpp"
#include "Mat.hpp"

namespace aurora {

	class Transform : public Component {
	public:
		Transform();
		Transform(const numa::Vec3& rotation, const numa::Vec3& position);

		// (pitch, yaw, roll) values are expected (in degrees)
		void SetRotation(const numa::Vec3& rotation);
		// (x, y, z) coordinates in the world space are expected
		void SetWorldPosition(const numa::Vec3& position);

		const numa::Vec3& GetEulerAnglesRotation() const;
		const numa::Vec3& GetWorldPosition() const;

		numa::Mat3 GetRotationMatrix() const;
		numa::Mat4 GetWorldMatrix() const;

	private:
		void UpdateWorldMatrix();

		numa::Mat4 world{};

		numa::Vec3 rotation{0.0f, 0.0f, 0.0};
		numa::Vec3 position{0.0f, 0.0f, 0.0f};
	};

}