#include "Framework/Materials/ParticipatingMedium.h"

#include "Numa.h"

namespace aurora {

	ParticipatingMedium::ParticipatingMedium()
		: Material(MaterialType::PARTICIPATING_MEDIUM) {
	}
	ParticipatingMedium::ParticipatingMedium(const numa::Vec3& mediumColor, float sigma_a, float sigma_s)
		: Material(MaterialType::PARTICIPATING_MEDIUM),
		mediumColor(mediumColor),
		sigma_a(sigma_a), sigma_s(sigma_s) {
	}

	float ParticipatingMedium::EvaluateIsotropicPhaseFunction(float cosTheta) const {
		// 1. Isotropic phase function
		static constexpr float p = 1.0f / (4.0f * numa::Pi<float>());
		return p;
	}
	float ParticipatingMedium::EvaluateHenyeyGreensteinPhaseFunction(float cosTheta) const {
		// 2. Anisotropic Heyney-Greenstein phase function
		static constexpr double n = 1.0 / (4.0 * numa::Pi<double>());
		double g = assymetryFactor;
		double h_x = 1.0 + g * g + 2 * g * cosTheta;
		double f_x = n * (1.0 - g * g) / (h_x * std::sqrt(h_x));
		return static_cast<float>(f_x);
	}

	float ParticipatingMedium::ComputeTransmittance(float distance) const {
		return std::expf(-GetExitanceCoefficient() * distance);
	}
	float ParticipatingMedium::ComputeTransmittance(const numa::Vec3& p, float distance) const {
		// [TODO]
		// Introduce some 'absorption', 'scattering', or 'density' variation function of 'p'.
		return std::expf(-GetExitanceCoefficient() * distance);
	}

	const numa::Vec3& ParticipatingMedium::GetMediumColor() const {
		return mediumColor;
	}

	float ParticipatingMedium::GetAbsorptionCoefficient() const {
		return sigma_a;
	}
	float ParticipatingMedium::GetScatteringCoefficient() const {
		return sigma_s;
	}
	float ParticipatingMedium::GetExitanceCoefficient() const {
		return sigma_a + sigma_s;
	}

}