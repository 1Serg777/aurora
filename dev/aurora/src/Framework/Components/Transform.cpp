#include "Framework/Components/Transform.h"

#include "Numa.h"
#include "MatrixTransform.hpp"

namespace aurora {

	Transform::Transform()
		: Component(ComponentType::Transform) {
		UpdateWorldMatrix();
	}
	Transform::Transform(const numa::Vec3& rotation, const numa::Vec3& position)
		: Component(ComponentType::Transform),
		rotation(rotation), position(position) {
		UpdateWorldMatrix();
	}

	void Transform::SetRotation(const numa::Vec3& rotation) {
		this->rotation = rotation;
		UpdateWorldMatrix();
	}
	void Transform::SetWorldPosition(const numa::Vec3& position) {
		this->position = position;
		UpdateWorldMatrix();
	}

	const numa::Vec3& Transform::GetEulerAnglesRotation() const {
		return rotation;
	}
	const numa::Vec3& Transform::GetWorldPosition() const {
		return position;
	}

	numa::Mat3 Transform::GetRotationMatrix() const {
		return numa::Mat3{ this->world };
	}
	numa::Mat4 Transform::GetWorldMatrix() const {
		return this->world;
	}

	void Transform::UpdateWorldMatrix() {
		this->world = numa::Mat4{
			numa::EulerAnglesRotate(
				numa::Vec3{
					numa::Rad(rotation.x),
					numa::Rad(rotation.y),
					numa::Rad(rotation.z)
				}),
			position
		};
	}

}