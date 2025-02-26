#include "Framework/Light.h"

#include "Numa.h"

namespace aurora
{
	// Light Sample

	void LightSampleBundle::AddLightSample(const LightSampleData& lightSample)
	{
		bundle.push_back(lightSample);
	}

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

	DirectionalLight::DirectionalLight(std::string_view lightName, const numa::Vec3& lightColor, float lightStrength)
		: Light(LightType::DIRECTIONAL, lightName), color(lightColor), strength(lightStrength)
	{
	}

	void DirectionalLight::Sample(const numa::Vec3& p, LightSampleData& data)
	{
		data.wi = Wi();
		data.pos = Pos();
		data.Li = Li();
		data.lightPtr = this;
	}

	numa::Vec3 DirectionalLight::Pos() const
	{
		return this->GetTransform()->GetWorldPosition();
	}

	numa::Vec3 DirectionalLight::Wi() const
	{
		numa::Mat4 world = this->GetTransform()->GetWorldMatrix();

		// Lights are oriented in the same way as cameras are, i.e. along the negative z-axis.
		// Light source shines in the direction '-world.forward', however 'wi' variable
		// is the opposite of that, directed toward the light source, so the result
		// should be world.forward

		return numa::Vec3{ world.forward };
	}
	numa::Vec3 DirectionalLight::Li() const
	{
		return color * strength;
	}

	// Point light

	PointLight::PointLight(std::string_view lightName, const numa::Vec3& lightColor, float lightIntensity)
		: Light(LightType::POINT, lightName), color(lightColor), intensity(lightIntensity)
	{
	}

	void PointLight::Sample(const numa::Vec3& p, LightSampleData& data)
	{
		data.wi = Wi(p);
		data.pos = Pos();
		data.Li = Li(numa::Length(p - data.pos));
		data.lightPtr = this;
	}

	numa::Vec3 PointLight::Pos() const
	{
		return this->GetTransform()->GetWorldPosition();
	}

	numa::Vec3 PointLight::Wi(const numa::Vec3& p) const
	{
		const numa::Vec3& lightPos = this->GetTransform()->GetWorldPosition();
		return numa::Normalize(lightPos - p);
	}
	numa::Vec3 PointLight::Li(float d) const
	{
		// To avoid division by zero!
		static constexpr float bias{ 0.00001f };

		// 1) is it
		// return (color * intensity) / (4.0f * numa::Pi<float>() * d + bias);

		// 2) or
		return (color * intensity) / (d + bias);
	}

	// Area light

	AreaLight::AreaLight(std::string_view lightName)
		: Light(LightType::AREA, lightName)
	{
	}

	void AreaLight::Sample(const numa::Vec3& p, LightSampleData& data)
	{
		// [TODO]
	}
}
