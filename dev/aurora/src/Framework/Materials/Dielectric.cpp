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
		return RefractImpl1(incident, normal, ior);
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
	
	FresnelData Dielectric::RefractImpl1(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const
	{
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel.html

		FresnelData fresnelData{};

		float n1 = ior;
		float n2 = this->ior;

		numa::Vec3 I = incident;
		numa::Vec3 N = normal;

		float IdotN = numa::Dot(I, N);

		// 1. Need a positive 'IdotN'
		// 2. Need a normal opposite the 'incident' direction
		// 3. 'n1' represents the medium the ray is travelling through,
		//     and 'n2' represents the medium the ray is entering.
		if (IdotN < 0.0f)
		{
			IdotN = -IdotN;
		}
		else
		{
			N = -N;
			std::swap(n1, n2);
		}

		float eta = n1 / n2;
		float c1 = IdotN;
		float c2_sqr = 1.0f - eta * eta * (1.0f - c1 * c1);
		float c2 = std::sqrtf(c2_sqr);

		numa::Vec3 T = eta * I + (eta * c1 - c2) * N;
		numa::Vec3 R = I - 2.0f * (numa::Dot(I, N)) * N;

		fresnelData.refracted = T;
		fresnelData.reflected = R;

		// 1.
		// Total internal reflection check using the following equation:
		// s1 / s2 == n2 / n1

		//float s2 = eta * std::sqrtf(1 - c1 * c1); // sin of the refraction angle
		//if (s2 > 1.0f)
		//{
		//	fresnelData.reflectedLightRatio = 1.0f;
		//	fresnelData.refractedLightRatio = 0.0;
		//	fresnelData.refracted = numa::Vec3{ 0.0f };
		//	return fresnelData;
		//}

		// 2.
		// Total internal reflection check

		if (c2_sqr < 0.0f)
		{
			fresnelData.reflectedLightRatio = 1.0f;
			fresnelData.refractedLightRatio = 0.0;
			fresnelData.refracted = numa::Vec3{ 0.0f };
			return fresnelData;
		}

		// Calculate the ratio

		// 1.

		// float c2 = -numa::Dot(T, N); // but we've already computed it earlier!
		std::pair<float, float> fresnel = Fresnel(c1, c2, n1, n2);

		// 2. Schlick approximation

		// std::pair<float, float> fresnel = FresnelSchlick(cosTheta1, eta1, eta2);

		fresnelData.reflectedLightRatio = fresnel.first;
		fresnelData.refractedLightRatio = fresnel.second;

		return fresnelData;
	}
	FresnelData Dielectric::RefractImpl2(const numa::Vec3& incident, const numa::Vec3& normal, float ior) const
	{
		// https://raytracing.github.io/books/RayTracingInOneWeekend.html#diffusematerials/truelambertianreflection

		// TODO

		return FresnelData();
	}

	std::pair<float, float> Dielectric::Fresnel(float c1, float c2, float n1, float n2) const
	{
		float Fr_parallel = (n2 * c1 - n1 * c2) / (n2 * c1 + n1 * c2);
		Fr_parallel *= Fr_parallel;

		float Fr_perpendicular = (n1 * c2 - n2 * c1) / (n1 * c2 + n2 * c1);
		Fr_perpendicular *= Fr_perpendicular;

		float Fr = 0.5f * (Fr_parallel + Fr_perpendicular);
		float Ft = 1.0f - Fr;

		return std::make_pair(Fr, Ft);
	}

	std::pair<float, float> Dielectric::FresnelSchlick(float c1, float n1, float n2) const
	{
		// 1. https://en.wikipedia.org/wiki/Schlick%27s_approximation
		// 2. https://graphicscompendium.com/raytracing/11-fresnel-beer

		float R0 = (n1 - n2) / (n1 + n2);
		R0 *= R0;

		float Fr = R0 + (1.0f - R0) * std::powf(1.0f - c1, 5.0f);
		float Ft = 1.0f - Fr;

		return std::make_pair(Fr, Ft);
	}
}