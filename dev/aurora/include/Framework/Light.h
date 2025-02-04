#pragma once

#include "Framework/Actor.h"

#include "Vec3.hpp"

#include <vector>

namespace aurora
{
	enum class LightType
	{
		DIRECTIONAL,
		SPHERICAL,
		AREA
	};

	class Light : public Actor
	{
	public:

		Light(LightType type, std::string_view lightName);

		virtual numa::Vec3 GetLightDirection(const numa::Vec3& point) const = 0;
		virtual numa::Vec3 GetLightPosition() const = 0;

		LightType GetLightType() const;

		numa::Vec3 color{ 1.0f, 1.0f, 1.0f };
		float intensity{ 1.0f };

	private:

		LightType type;
	};

	class DirectionalLight : public Light
	{
	public:

		DirectionalLight();
		DirectionalLight(std::string_view lightName);

		numa::Vec3 GetLightDirection(const numa::Vec3& point) const override;
		numa::Vec3 GetLightPosition() const override;

		numa::Vec3 GetLight() const;
	};

	class SphericalLight : public Light
	{
	public:

		SphericalLight();
		SphericalLight(std::string_view lightName);

		numa::Vec3 GetLightDirection(const numa::Vec3& point) const override;
		numa::Vec3 GetLightPosition() const override;

		numa::Vec3 GetLight(const numa::Vec3& point) const;
	};

	class AreaLight : public Light
	{
	public:

		AreaLight();
		AreaLight(std::string_view lightName);

		numa::Vec3 GetLightDirection(const numa::Vec3& point) const override;
		numa::Vec3 GetLightPosition() const override;
	};

	struct LightHitData : GeometryRayHit
	{
		numa::Vec3 lightDir{};
		numa::Vec3 lightPos{};

		Light* lightPtr{};
		numa::Vec3 light{};
		numa::Vec3 lightColor{};
		float lightIntensity{};

		// That's needed for volume rendering,
		// for the Ray-Marching algorithm in particular.
		GeometryRayHit lightPathInVolume{};
		numa::Vec3 lightEnterPoint{};
	};

	struct LightHitBundle
	{
		std::vector<LightHitData> lightHitBundle;
	};
}