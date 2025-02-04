#include "Framework/Components/Transform.h"

namespace aurora
{
	Transform::Transform()
		: Component(ComponentType::Transform)
	{
	}
	Transform::Transform(const numa::Vec3& rotation, const numa::Vec3& position)
		: Component(ComponentType::Transform),
		rotation(rotation), translation(position)
	{
		// TODO
	}

	void Transform::SetRotation(const numa::Vec3& rotation)
	{
		// TODO
		this->rotation = rotation;
	}
	void Transform::SetWorldPosition(const numa::Vec3& position)
	{
		this->translation = position;
	}

	const numa::Vec3& Transform::GetEulerAnglesRotation() const
	{
		return rotation;
	}
	const numa::Vec3& Transform::GetWorldPosition() const
	{
		return translation;
	}

	numa::Mat3 Transform::GetRotationMatrix() const
	{
		return numa::Mat3{};
	}

	numa::Mat4 Transform::GetWorldMatrix() const
	{
		/*
		glm::mat4 pitch = glm::rotate(glm::mat4(1.0), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yaw = glm::rotate(glm::mat4(1.0), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 roll = glm::rotate(glm::mat4(1.0), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 R = yaw * pitch * roll;

		glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);

		glm::mat4 worldMatrix = T * R;

		return worldMatrix;
		*/

		return numa::Mat4{};
	}
}