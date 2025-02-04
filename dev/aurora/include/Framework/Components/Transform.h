#pragma once

#include "Framework/Components/Component.h"

#include "Vec3.hpp"
#include "Mat3.hpp"
#include "Mat4.hpp"

namespace aurora
{
	class Transform : public Component
	{
	public:

		Transform();
		Transform(const numa::Vec3& rotation, const numa::Vec3& position);

		// (pitch, yaw, roll) values are expected (in radians)
		void SetRotation(const numa::Vec3& rotation);
		// (x, y, z) coordinates in the world space are expected
		void SetWorldPosition(const numa::Vec3& position);

		const numa::Vec3& GetEulerAnglesRotation() const;
		const numa::Vec3& GetWorldPosition() const;

		numa::Mat3 GetRotationMatrix() const;
		numa::Mat4 GetWorldMatrix() const;

	private:

		numa::Vec3 rotation{ 0.0f, 0.0f, 0.0 };
		numa::Vec3 translation{ 0.0f, 0.0f, 0.0f };
	};
}