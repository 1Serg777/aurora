#pragma once

#include "Framework/Components/Material.h"

#include "Vec.hpp"

namespace aurora {

	class ParticipatingMedium : public Material {
	public:
		ParticipatingMedium();
		ParticipatingMedium(const numa::Vec3& mediumColor, float sigma_a, float sigma_s);

		numa::Vec3 Scatter(const numa::Vec3& wo, const numa::Vec3& N,
			               numa::Vec3& brdf, float& pdf) const override;

		float EvaluateIsotropicPhaseFunction(float cosTheta) const;
		float EvaluateHenyeyGreensteinPhaseFunction(float cosTheta) const;

		float ComputeTransmittance(float distance) const;
		float ComputeTransmittance(const numa::Vec3& p, float distance) const;

		const numa::Vec3& GetMediumColor() const;

		float GetAbsorptionCoefficient() const;
		float GetScatteringCoefficient() const;
		float GetExitanceCoefficient() const;

	private:
		numa::Vec3 mediumColor{0.8f};
		float sigma_a{0.0f};
		float sigma_s{0.0f};
		float assymetryFactor{0.0f};
	};

}