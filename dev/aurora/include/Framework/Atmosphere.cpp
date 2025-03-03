#include "Framework/Atmosphere.h"

namespace aurora
{
	Atmosphere::Atmosphere(
		AtmosphereData atmosphereData,
		std::string_view name)
		: atmosphereData(atmosphereData), atmosphereName(atmosphereName)
	{
		CreateSpheres();
	}

	bool Atmosphere::Intersect(const numa::Ray& ray, GeometryRayHit& rayHit)
	{
		return false;
	}
	bool Atmosphere::IntersectGround(const numa::Ray& ray, GeometryRayHit& rayHit)
	{
		return false;
	}
	bool Atmosphere::IntersectAtmosphere(const numa::Ray& ray, GeometryRayHit& rayHit)
	{
		return false;
	}

	numa::Vec3 Atmosphere::ComputeSkyColor(const numa::Ray& ray) const
	{
		// TODO
		return numa::Vec3{ 0.0f };
	}

	float Atmosphere::RayleighPhaseFunction(float cosTheta) const
	{
		// TODO
		return 0.0f;
	}
	float Atmosphere::MiePhaseFunction(float cosTheta) const
	{
		// TODO
		return 0.0f;
	}

	float Atmosphere::GetMiePhaseG() const
	{
		return atmosphereData.mie.mie_phase_g;
	}

	Sphere* Atmosphere::GetGroundSphere() const
	{
		return groundSphere.get();
	}
	Sphere* Atmosphere::GetAtmosphereSphere() const
	{
		return atmosphereSphere.get();
	}

	numa::Vec3 Atmosphere::GetBetaR0() const
	{
		return atmosphereData.rayleigh.betaR0;
	}
	float Atmosphere::GetBetaM0() const
	{
		return atmosphereData.mie.betaM0;
	}

	float Atmosphere::GetScaleHeightRayleigh() const
	{
		return atmosphereData.rayleigh.HR;
	}
	float Atmosphere::GetScaleHeightMie() const
	{
		return atmosphereData.mie.HM;
	}

	const std::string& Atmosphere::GetAtmosphereName() const
	{
		return atmosphereName;
	}

	void Atmosphere::CreateSpheres()
	{
		groundSphere = std::make_shared<Sphere>(atmosphereData.groundRadius);
		atmosphereSphere = std::make_shared<Sphere>(atmosphereData.atmosphereRadius);
	}
}