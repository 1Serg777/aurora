#pragma once

#include "Framework/Actor.h"
#include "Framework/Light.h"
#include "Framework/Components/Geometry.h"
#include "Framework/Components/Transform.h"
#include "Geometry/Sphere.h"

#include <memory>
#include <string>
#include <string_view>

#include <Ray.h>
#include <Vec.hpp>

namespace aurora {

	struct RayleighScatteringData {
		numa::Vec3 betaR0{};
		float HR{};
	};
	struct MieScatteringData {
		float betaM0{};
		float HM{};
		float mie_phase_g{};
	};

	struct AtmosphereData {
		RayleighScatteringData rayleigh{};
		MieScatteringData mie{};
		float groundRadius{};
		float atmosphereRadius{};
	};

	class Atmosphere {
	public:
		Atmosphere(AtmosphereData atmosphereData, std::string_view name);

		bool Intersect(const numa::Ray& ray, ActorRayHit& rayHit) const;
		bool IntersectGround(const numa::Ray& ray, ActorRayHit& rayHit) const;
		bool IntersectAtmosphere(const numa::Ray& ray, ActorRayHit& rayHit) const;

		numa::Vec3 GetSunlight(const numa::Vec3& p, DirectionalLight* sun);
		numa::Vec3 ComputeSkyColor(const numa::Ray& ray, DirectionalLight* dirLight) const;

		float RayleighPhaseFunction(float cosTheta) const;
		float MiePhaseFunction(float cosTheta) const;

		// Mie phase function assymetry factor (g)
		float GetMiePhaseG() const;

		// Sea level Rayleigh scattering coefficient
		numa::Vec3 GetBetaR0() const;
		// Sea level Mie scattering coefficient
		float GetBetaM0() const;

		numa::Vec3 ComputeBetaR(const numa::Vec3& p) const;
		float ComputeBetaM(const numa::Vec3& p) const;
		numa::Vec3 ComputeBetaCombined(const numa::Vec3& p) const;

		// Rayleigh scale height
		float GetScaleHeightRayleigh() const;
		// Mie scale height
		float GetScaleHeightMie() const;

		const std::string& GetAtmosphereName() const;

	private:
		void CreateSpheres();

		float ComputeSamplePointHeight(const numa::Vec3& p) const;

		numa::Vec3 ComputeRayleighTransmittance(const numa::Vec3& p, float dt) const;
		float ComputeMieTransmittance(const numa::Vec3& p, float dt) const;
		numa::Vec3 ComputeCombinedTransmittance(const numa::Vec3& p, float dt) const;

		std::shared_ptr<Actor> groundSphere;
		std::shared_ptr<Actor> atmosphereSphere;
		AtmosphereData atmosphereData{};
		std::string atmosphereName;
	};

}