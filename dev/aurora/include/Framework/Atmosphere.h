#pragma once

#include "Framework/Components/Geometry.h"
#include "Geometry/Sphere.h"

#include <memory>
#include <string>
#include <string_view>

#include <Ray.h>
#include <Vec3.hpp>

namespace aurora
{
	struct RayleighScatteringData
	{
		numa::Vec3 betaR0{  };
		float HR{  };
	};

	struct MieScatteringData
	{
		float betaM0{  };
		float HM{  };
		float mie_phase_g{  };
	};

	struct AtmosphereData
	{
		RayleighScatteringData rayleigh{};
		MieScatteringData mie{};
		float groundRadius{};
		float atmosphereRadius{};
	};

	class Atmosphere
	{
	public:

		Atmosphere(
			AtmosphereData atmosphereData,
			std::string_view name);

		bool Intersect(const numa::Ray& ray, GeometryRayHit& rayHit);
		bool IntersectGround(const numa::Ray& ray, GeometryRayHit& rayHit);
		bool IntersectAtmosphere(const numa::Ray& ray, GeometryRayHit& rayHit);

		numa::Vec3 ComputeSkyColor(const numa::Ray& ray) const;

		float RayleighPhaseFunction(float cosTheta) const;
		float MiePhaseFunction(float cosTheta) const;

		// Mie phase function assymetry factor (g)
		float GetMiePhaseG() const;

		Sphere* GetGroundSphere() const;
		Sphere* GetAtmosphereSphere() const;

		// Sea level Rayleigh scattering coefficient
		numa::Vec3 GetBetaR0() const;
		// Sea level Mie scattering coefficient
		float GetBetaM0() const;

		// Rayleigh scale height
		float GetScaleHeightRayleigh() const;
		// Mie scale height
		float GetScaleHeightMie() const;

		const std::string& GetAtmosphereName() const;

	private:

		void CreateSpheres();

		std::shared_ptr<Sphere> groundSphere;
		std::shared_ptr<Sphere> atmosphereSphere;

		AtmosphereData atmosphereData{
			RayleighScatteringData{
				// numa::Vec3 { 5.8e-6, 13.5e-6, 33.1e-6 }, // betaR0
				numa::Vec3 { 3.8e-6, 13.5e-6, 33.1e-6 }, // betaR0
				// float { 8000 }, // HR
				float { 7994 }, // HR
			},
			MieScatteringData{
				float { 21e-6f }, // betaM0
				float { 1200 }, // HM
				float { 0.76f }, // mie_phase_g
			},
			636e4,
			642e4,
		};

		std::string atmosphereName;
	};
}