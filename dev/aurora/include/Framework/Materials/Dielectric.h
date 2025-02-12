#pragma once

#include "Framework/Components/Material.h"

#include "Vec3.hpp"

#include <utility>

namespace aurora
{
	struct FresnelData
	{
		numa::Vec3 reflected{ 0.0f, 0.0f, 0.0f };
		numa::Vec3 refracted{ 0.0f, 0.0f, 0.0f };
		float reflectedLightRatio{ 0.0f };
		float refractedLightRatio{ 0.0f };
	};

	class Dielectric : public Material
	{
	public:

		Dielectric(const numa::Vec3& attenuation, float ior);

		FresnelData Fresnel(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const;

		numa::Vec3 Reflect(const numa::Vec3& incidentDirection, const numa::Vec3& normal) const;
		numa::Vec3 Refract(const numa::Vec3& incidentDirection, const numa::Vec3& normal, float ior) const;

		float GetReflectedLightRatio() const;
		float GetRefractedLightRatio() const;

		void SetAttenuation(const numa::Vec3& attenuation);
		const numa::Vec3& GetAttenuation() const;

		float GetIndexOfRefraction() const;

	private:

		FresnelData RefractImpl1(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const;
		FresnelData RefractImpl2(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const;

		std::pair<float, float> Fresnel(float c1, float c2, float n1, float n2) const;
		std::pair<float, float> FresnelSchlick(float c1, float n1, float n2) const;

		numa::Vec3 attenuation{ 1.0f, 1.0f, 1.0f };
		float ior{ 1.0f };
	};
}