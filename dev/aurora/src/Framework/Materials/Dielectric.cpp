#include "Framework/Materials/Dielectric.h"

#include "Numa.h"

#include <iostream>
#include <utility>

namespace aurora
{
	Dielectric::Dielectric(const numa::Vec3& attenuation, float ior)
		: Material(MaterialType::DIELECTRIC), ior(ior)
	{
	}

	FresnelData Dielectric::Fresnel(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const
	{
		return Fresnel1(incident, normal, ior);
	}

	numa::Vec3 Dielectric::Reflect(const numa::Vec3& incidentDirection, const numa::Vec3& normal) const
	{
		// TODO
		return numa::Vec3{ 0.0f, 0.0f, 0.0f };
	}
	numa::Vec3 Dielectric::Refract(const numa::Vec3& incidentDirection, const numa::Vec3& normal, float ior) const
	{
		// TODO
		return numa::Vec3{ 0.0f, 0.0f, 0.0f };
	}

	float Dielectric::GetReflectedLightRatio() const
	{
		// TODO
		return 0.0f;
	}
	float Dielectric::GetRefractedLightRatio() const
	{
		// TODO
		return 0.0f;
	}

	void Dielectric::SetAttenuation(const numa::Vec3& attenuation)
	{
		this->attenuation = attenuation;
	}
	const numa::Vec3& Dielectric::GetAttenuation() const
	{
		return attenuation;
	}

	float Dielectric::GetIndexOfRefraction() const
	{
		return ior;
	}
	
	FresnelData Dielectric::Fresnel1(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const
	{
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel.html

		FresnelData fresnelData{};

		float eta1 = ior;
		float eta2 = this->ior;

		numa::Vec3 I = incident;
		numa::Vec3 N = normal;
		float IdotN = numa::Dot(I, N);

		// 1. Need a positive 'IdotN'
		// 2. Need a normal opposite the 'incident' direction
		// 3. eta1 represents ..., and eta2 represents ...
		if (IdotN < 0.0f)
		{
			IdotN = -IdotN;
		}
		else
		{
			N = -N;
			std::swap(eta1, eta2);
		}

		// Reassigning it here for convenience
		float cosTheta1 = IdotN;

		float eta = eta1 / eta2;
		float c1 = cosTheta1;
		float c2 = std::sqrtf(1.0f - eta * eta * (1.0f - cosTheta1 * cosTheta1));

		numa::Vec3 T = eta * I + (eta * c1 - c2) * N;
		numa::Vec3 R = I - 2.0f * (numa::Dot(I, N)) * N;

		fresnelData.refracted = T;
		fresnelData.reflected = R;

		// Total internal reflection check
		float s1 = eta * std::sqrtf(1 - cosTheta1 * cosTheta1);
		if (s1 > 1.0f)
		{
			fresnelData.reflectedLightRatio = 1.0f;
			fresnelData.refractedLightRatio = 0.0;
			fresnelData.refracted = numa::Vec3{ 0.0f };
			return fresnelData;
		}

		// Calculate the ratio

		// 1.

		float cosTheta2 = numa::Dot(R, N);

		float Fr_parallel = (eta2 * cosTheta1 - eta1 * cosTheta2) / (eta2 * cosTheta1 + eta1 * cosTheta2);
		Fr_parallel *= Fr_parallel;

		float Fr_perpendicular = (eta1 * cosTheta2 - eta2 * cosTheta1) / (eta1 * cosTheta2 + eta2 * cosTheta1);
		Fr_perpendicular *= Fr_perpendicular;

		float Fr = 0.5f * (Fr_parallel + Fr_perpendicular);
		float Ft = 1.0f - Fr;

		fresnelData.reflectedLightRatio = Fr;
		fresnelData.refractedLightRatio = Ft;

		// 2. Schlick approximation

		// [TODO]

		return fresnelData;

		return FresnelData();
	}
	FresnelData Dielectric::Fresnel2(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const
	{
		// https://raytracing.github.io/books/RayTracingInOneWeekend.html#diffusematerials/truelambertianreflection

		// TODO
		return FresnelData();
	}
}