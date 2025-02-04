#include "Framework/Light.h"

namespace aurora
{
	// Light base class

	Light::Light(LightType type, std::string_view lightName)
		: Actor(lightName), type(type)
	{
	}

	LightType Light::GetLightType() const
	{
		return type;
	}

	// Directional light

	DirectionalLight::DirectionalLight()
		: Light(LightType::DIRECTIONAL, "Directional Light")
	{
	}
	DirectionalLight::DirectionalLight(std::string_view lightName)
		: Light(LightType::DIRECTIONAL, lightName)
	{
	}

	numa::Vec3 DirectionalLight::GetLightDirection(const numa::Vec3& lightDirAtPoint) const
	{
		numa::Mat4 world = this->GetTransform()->GetWorldMatrix();

		// Lights are oriented in the same way as cameras are:
		// along the negative z-axis.
		// return -world[2];

		return numa::Vec3{ -world.forward };
	}
	numa::Vec3 DirectionalLight::GetLightPosition() const
	{
		return this->GetTransform()->GetWorldPosition();
	}

	numa::Vec3 DirectionalLight::GetLight() const
	{
		return color * intensity;
	}

	// Spherical light

	SphericalLight::SphericalLight()
		: Light(LightType::SPHERICAL, "Spherical Light")
	{
	}
	SphericalLight::SphericalLight(std::string_view lightName)
		: Light(LightType::SPHERICAL, lightName)
	{
	}

	numa::Vec3 SphericalLight::GetLightDirection(const numa::Vec3& point) const
	{
		const numa::Vec3& lightPos = this->GetTransform()->GetWorldPosition();
		return numa::Normalize(point - lightPos);
	}
	numa::Vec3 SphericalLight::GetLightPosition() const
	{
		return this->GetTransform()->GetWorldPosition();
	}

	numa::Vec3 SphericalLight::GetLight(const numa::Vec3& point) const
	{
		// To avoid division by zero!
		static constexpr float bias{ 0.00001f };

		// const numa::Vec3& lightPos = this->GetTransform()->GetWorldPosition();
		// float r2 = glm::max(0.0f, glm::length2(point - lightPos));
		// return (GetLightIntensity() * GetLightColor()) / (4 * glm::pi<float>() * r2 + bias);

		return numa::Vec3{ 0.0f };
	}

	// Area light

	AreaLight::AreaLight()
		: Light(LightType::SPHERICAL, "Area Light")
	{
	}
	AreaLight::AreaLight(std::string_view lightName)
		: Light(LightType::AREA, lightName)
	{
	}

	numa::Vec3 AreaLight::GetLightDirection(const numa::Vec3& lightDirAtPoint) const
	{
		// TODO
		return numa::Vec3();
	}
	numa::Vec3 AreaLight::GetLightPosition() const
	{
		// TODO
		return numa::Vec3();
	}
}
