#include "Framework/Atmosphere.h"

#include "Framework/Light.h"

#include "Numa.h"
#include "Random.h"

#include <cassert>

namespace aurora
{
	Atmosphere::Atmosphere(
		AtmosphereData atmosphereData,
		std::string_view name)
		: atmosphereData(atmosphereData), atmosphereName(atmosphereName)
	{
		CreateSpheres();
	}

	bool Atmosphere::Intersect(const numa::Ray& ray, ActorRayHit& rayHit) const
	{
		// The order is intentional. Here, we're relying on the lazy evaluation of the "or" conditional.
		return IntersectGround(ray, rayHit) || IntersectAtmosphere(ray, rayHit);
	}
	bool Atmosphere::IntersectGround(const numa::Ray& ray, ActorRayHit& rayHit) const
	{
		return groundSphere->Intersect(ray, rayHit);
	}
	bool Atmosphere::IntersectAtmosphere(const numa::Ray& ray, ActorRayHit& rayHit) const
	{
		return atmosphereSphere->Intersect(ray, rayHit);
	}

	numa::Vec3 Atmosphere::ComputeSkyColor(const numa::Ray& ray, DirectionalLight* dirLight) const
	{
		// Constants

		static constexpr float bias{ 0.00001f };

		static constexpr float atmosphereHitBias{ bias };

		static constexpr float trimPathDistance{ bias };
		static constexpr float acceptedPathDistanceThreshold{ bias };

		static constexpr float singleSegmentDistanceThreshold{ 1e3f * bias };

		// 1. First of all we search for an intersection with one of the spheres in our model.
		//    - Ground sphere is checked first
		//    - Atmosphere sphere is checked second
		//    The ground sphere is checked first because we might potentially want to
		//    handle that as a special case. When the ray intersects the atmosphere sphere, however,
		//    the computation is handled as usual. For now, however, we don't handle the two
		//    separately and instead compute the sky color regardless of what sphere was intersected.

		ActorRayHit atmospherePathHit{};
		bool atmospherePathHitCheck = Intersect(ray, atmospherePathHit);

		assert(atmospherePathHitCheck && "The ray must be in the space between the two spheres!");

		// 2. Now that we've figured out the intersection we can move on to the integration.
		//    Similarly to volume rendering we separate the ray into segments and integrate (collect or accumulate)
		//    the radiance at each segment, modulate it with the scattering coefficient, transmittance,
		//    and the corresponding phase function.
		//    However, there are a couple of moments where the participating medium integration differes
		//    from what we're going to do here.
		//    Essentially, we're computing the same equation, but we actually do it twice.
		//    1) We compute the Radiative Transfer Equation for Rayleigh scattering.
		//    2) We compute the Radiative Transfer Equation for Mie scattering.
		//    The results are then combined, which gives us the final sky color.

		float atmospherePathDistance = atmospherePathHit.hitDistance - trimPathDistance;

		if (atmospherePathDistance <= acceptedPathDistanceThreshold)
		{
			// Intersection is considered very close to the edge of the atmosphere.
			// That pretty much means we can't get any possible scattering in our viewing direction.
			// In participating medium we would return L(0), but since we have the outer space
			// beyond our atmosphere, we're going to return numa::Vec3{ 0.0f };
			// In theory we could have the sun in our viewing direction or faint light from some distant stars.
			// However, for now we're going to simplify the procedure and assume that beyond the atmosphere is nothing.

			return numa::Vec3{ 0.0f };
		}

		// 3. Now we can start the integration.
		//    And again, there are a couple of differences that we should note.
		//    Transmittance is now a function that depends on wavelength. This is due to the
		//    scattering coefficients depending on wavelength as wel. This is only true for
		//    Rayleigh scattering, while for Mie scattering the computation is performed as usual.

		numa::Vec3 Tr_R{ 1.0f };
		float Tr_M{ 1.0f };
		// TEST!
		numa::Vec3 Tr{ 1.0f };

		uint32_t segments = 32;
		float t = atmospherePathDistance;
		float dt = t / segments;

		numa::Vec3 wo = -ray.GetDirection();
		numa::Vec3 Lo{ 0.0f };
		numa::Vec3 Lo_R{ 0.0f };
		numa::Vec3 Lo_M{ 0.0f };
		for (uint32_t segment = 0; segment < segments; segment++)
		{
			// Move to the next segment and add some jitter within it.

			float t_prime_jitter = 0.5f * dt; // introdcues banding (can't be alleviated with more SPPs)
			// float t_prime_jitter = numa::RandomFloat() * dt; // introduces noise (can be alleviated with more SPPs)

			float t_prime = segment * dt + t_prime_jitter; // or 'segment_t'

			// Find the point corresponding to 't_prime'

			numa::Vec3 p_prime = ray.GetPoint(t_prime); // 'segment_p'

			// Calculate the segment transmittance as well as the transmittance from 'p_prime' to 'p'
			// where 'p' is where the camera ray entered the volume.

			numa::Vec3 segment_Tr = ComputeCombinedTransmittance(p_prime, dt);
			Tr *= segment_Tr;

			numa::Vec3 beta_R = ComputeBetaR(p_prime);
			float beta_M = ComputeBetaM(p_prime);

			// The only light source we consider is the directional light source.
			// That's because this is the single dominant light source that influences
			// the appearance of the sky during sunrise, daytime, and sunset.

			// Sample the directional light retrieving all the necessary information we need about it.
			// This includes light direction 'wi', radiance 'Li', and position 'p'.

			LightSampleData lightSampleData{};
			dirLight->Sample(p_prime, lightSampleData);

			// Now we need to make sure that there's nothing in our way to reach the light.
			// Again, the assumption for now is that there's nothing in the atmosphere blocking the light.

			numa::Ray lightRay{
				p_prime,
				lightSampleData.wi
			};

			// We search for an intersection with the atmosphere sphere in the light source direction.

			ActorRayHit atmosphereLightHit{};
			bool atmosphereLightHitCheck = IntersectAtmosphere(lightRay, atmosphereLightHit);

			numa::Vec3 atmosphereLightEntryPoint = lightRay.GetPoint(atmosphereLightHit.hitDistance);
			float atmosphereLightPathDistance = atmosphereLightHit.hitDistance - trimPathDistance;

			if (!atmosphereLightHitCheck || atmosphereLightPathDistance <= acceptedPathDistanceThreshold)
			{
				atmosphereLightEntryPoint = p_prime;
				atmosphereLightPathDistance = 0.0f;

				// The light path distance of 0.0f also ensures that the transmittance for
				// that single segment will be 1.0f, so no attenuation to the light radiance.
			}

			uint32_t light_segments = 32;
			float light_t = atmosphereLightPathDistance;
			float light_dt = light_t / light_segments;

			if (atmosphereLightPathDistance <= singleSegmentDistanceThreshold)
			{
				light_segments = 1;
				light_dt = light_t;
			}

			numa::Vec3 light_path_Tr_R{ 1.0f };
			float light_path_Tr_M{ 1.0f };
			// TEST!
			numa::Vec3 light_path_Tr{ 1.0f };
			for (uint32_t light_segment = 0; light_segment < light_segments; light_segment++)
			{
				// Move to the next light path segment and add some jitter within it.

				float light_t_prime_jitter = 0.5f * light_dt; // or 'light_t_shift'; could make the jitter random within 'light_dt'
				float light_t_prime = light_segment * light_dt + light_t_prime_jitter; // or 'light_segment_t'

				// Find the point corresponding to 'light_t_prime'

				numa::Vec3 light_p_prime = lightRay.GetPoint(light_t_prime); // 'segment_p'

				numa::Vec3 light_segment_Tr = ComputeCombinedTransmittance(light_p_prime, light_dt);
				light_path_Tr *= light_segment_Tr;
			}

			float cos_theta = numa::Dot(wo, lightSampleData.wi);

			float phase_R = RayleighPhaseFunction(cos_theta);
			float phase_M = MiePhaseFunction(cos_theta);

			numa::Vec3 Li = lightSampleData.Li * light_path_Tr;

			numa::Vec3 Ls_R = phase_R * Li;
			numa::Vec3 Ls_M = phase_M * Li;

			// The 'beta_R' and 'beta_M' scattering coefficients for the current segment have already been evaluated above,
			// so we don't need to do that again.

			Lo_R += Tr * beta_R * Ls_R * dt;
			Lo_M += Tr * beta_M * Ls_M * dt;
		}

		Lo = Lo_R + Lo_M;

		return Lo;
	}

	float Atmosphere::RayleighPhaseFunction(float cosTheta) const
	{
		double result = 3.0 / (16.0 * numa::Pi<double>()) * (1.0 + cosTheta * cosTheta);
		return static_cast<float>(result);
	}
	float Atmosphere::MiePhaseFunction(float cosTheta) const
	{
		double g = atmosphereData.mie.mie_phase_g;
		double g_sqr = g * g;
		double Mu_sqr = cosTheta * cosTheta;

		constexpr static double factor = 3.0 / (8.0 * numa::Pi<double>());

		// Source:
		// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky.html
		// Section: Mie Scattering
		// In the lesson 'pow_denom' factor in the denominator has a '-' instead of a '+'.
		// This, however, results in the direction of the distant light source beeing reversed.
		// The Heyney-Greenstein function in the volume rendering on Scratchapixel had the same factor in the equation.
		// https://www.scratchapixel.com/lessons/3d-basic-rendering/volume-rendering-for-developers/ray-marching-get-it-right.html
		// The lesson also uses a '-' instead of a '+', which also results in the directions beeing reversed.
		// In the case of that phase function, light is scattered forward when the assymetry factor 'g' has
		// negative values and light is scattered back when 'g' is positive.
		// This is opposite to what 

		double pow_denom = 1.0 + g_sqr + 2.0 * g * cosTheta;

		double numerator = (1.0 - g_sqr) * (1.0 + Mu_sqr);
		double denominator = (2.0 + g_sqr) * (pow_denom * std::sqrt(pow_denom));
		double result = factor * (numerator / denominator);

		return static_cast<float>(result);
	}

	float Atmosphere::GetMiePhaseG() const
	{
		return atmosphereData.mie.mie_phase_g;
	}

	numa::Vec3 Atmosphere::GetBetaR0() const
	{
		return atmosphereData.rayleigh.betaR0;
	}
	float Atmosphere::GetBetaM0() const
	{
		return atmosphereData.mie.betaM0;
	}

	numa::Vec3 Atmosphere::ComputeBetaR(const numa::Vec3& p) const
	{
		float h = ComputeSamplePointHeight(p);
		float HR = GetScaleHeightRayleigh();
		float exp = std::exp(-h / HR);
		numa::Vec3 BetaR = GetBetaR0() * exp;
		return BetaR;
	}
	float Atmosphere::ComputeBetaM(const numa::Vec3& p) const
	{
		float h = ComputeSamplePointHeight(p);
		float HM = GetScaleHeightMie();
		float exp = std::exp(-h / HM);
		float BetaM = GetBetaM0() * exp;
		return BetaM;
	}

	numa::Vec3 Atmosphere::ComputeBetaCombined(const numa::Vec3& p) const
	{
		return ComputeBetaR(p) + numa::Vec3{ ComputeBetaM(p) };
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
		std::shared_ptr<Sphere> groundSphereGeometry = std::make_shared<Sphere>(atmosphereData.groundRadius);
		std::shared_ptr<Sphere> atmosphereSphereGeometry = std::make_shared<Sphere>(atmosphereData.atmosphereRadius);

		// Origin is a random sea level place on the ground.
		// The ground and atmosphere spheres are defined w.r.t. that origin.

		std::shared_ptr<Transform> groundSphereTransform = std::make_shared<Transform>();
		groundSphereTransform->SetWorldPosition(numa::Vec3{ 0.0f, -atmosphereData.groundRadius, 0.0f });
		groundSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		std::shared_ptr<Transform> atmosphereSphereTransform = std::make_shared<Transform>();
		atmosphereSphereTransform->SetWorldPosition(numa::Vec3{ 0.0f, -atmosphereData.groundRadius, 0.0f });
		atmosphereSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		groundSphere = std::make_shared<Actor>(atmosphereName + "_Ground");
		groundSphere->SetGeometry(groundSphereGeometry);
		groundSphere->SetTransform(groundSphereTransform);

		atmosphereSphere = std::make_shared<Actor>(atmosphereName + "_Atmosphere");
		atmosphereSphere->SetGeometry(atmosphereSphereGeometry);
		atmosphereSphere->SetTransform(atmosphereSphereTransform);
	}

	float Atmosphere::ComputeSamplePointHeight(const numa::Vec3& p) const
	{
		// The assumption is that the caller code should make sure that
		// the sample point is between the two spheres. Otherwise we're
		// going to get a negative value for height.

		float height =
			numa::Length(p - groundSphere->GetTransform()->GetWorldPosition()) -
			atmosphereData.groundRadius;

		return height;
	}

	numa::Vec3 Atmosphere::ComputeRayleighTransmittance(const numa::Vec3& p, float dt) const
	{
		numa::Vec3 Tr_R = numa::Exp(-ComputeBetaR(p) * dt);
		return Tr_R;
	}
	float Atmosphere::ComputeMieTransmittance(const numa::Vec3& p, float dt) const
	{
		float Tr_M = std::exp(-ComputeBetaM(p) * dt);
		return Tr_M;
	}
	numa::Vec3 Atmosphere::ComputeCombinedTransmittance(const numa::Vec3& p, float dt) const
	{
		numa::Vec3 BetaR = ComputeBetaR(p);
		numa::Vec3 BetaM = ComputeBetaM(p);
		numa::Vec3 Tr = numa::Exp(-(BetaR + BetaM) * dt);
		return Tr;
	}
}