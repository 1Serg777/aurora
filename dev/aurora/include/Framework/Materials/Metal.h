#pragma once

#include "Framework/Components/Material.h"

#include "Vec3.hpp"

namespace aurora
{
	class Metal : public Material
	{
	public:

		Metal();
		Metal(const numa::Vec3& attenuation, float fuzziness = 0.0f);

		// numa::Vec3 Scatter(const numa::Vec3& incidentDirection, const numa::Vec3& normal);

		// Incident direction should point at the surface
		numa::Vec3 Reflect(const numa::Vec3& incidentDirection, const numa::Vec3& normal) const;

		void SetAttenuation(const numa::Vec3& attenuation);
		const numa::Vec3& GetAttenuation() const;

	private:

		numa::Vec3 attenuation{ 1.0f, 1.0f, 1.0f };
		float fuzziness{ 0.0f };
	};
}