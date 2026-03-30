#include "Framework/Materials/Lambertian.h"

#include "Framework/Actor.h"
#include "Framework/Components/Transform.h"

#include "Numa.h"
#include "Random.h"
#include "Sample.h"

#include <cmath>

namespace aurora {

	Lambertian::Lambertian()
		: Material(MaterialType::LAMBERTIAN) {
	}
	Lambertian::Lambertian(const numa::Vec3& albedo)
		: Material(MaterialType::LAMBERTIAN), albedo(albedo) {
	}

	numa::Vec3 Lambertian::Scatter(const numa::Vec3& wo, const numa::Vec3& N,
		                           numa::Vec3& brdf, float& pdf) const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f, 0.0f, 0.0f};
		numa::Vec3 wiLocal = numa::SampleHemisphereCosWeight(numa::RandomVec2());
		// numa::Vec3 wiLocal = numa::SampleHemisphereUniform(numa::RandomVec2());
		// Now we need to construct a TNB (or TBN) matrix to transform the local 'wi' direction into the world coordinates.
		numa::Vec3 up = numa::Vec3{0.0f, 1.0f, 0.0f};
		if (N.y > 0.995f)
			up = numa::Vec3{1.0f, 0.0f, 0.0f};
		numa::Vec3 B = numa::Normalize(numa::Cross(up, N));
		numa::Vec3 T = numa::Cross(N, B);
		numa::Mat3 TNB{
			T, N, B
		};
		numa::Vec3 wiWorld = TNB * wiLocal;

		float cosTheta = std::clamp(numa::Dot(wiWorld, N), 0.0f, 1.0f);
		brdf = Brdf();
		pdf = Pdf(cosTheta);
		return wiWorld;
	}

	numa::Vec3 Lambertian::Brdf() const {
		return albedo / numa::Pi<float>();
	}
	float Lambertian::Pdf(float cosTheta) const {
		// 1. Uniform
		// return 1.0f / numa::TwoPi<float>();
		// 2. Cosine-weighted
		return cosTheta / numa::Pi<float>();
	}

	void Lambertian::SetMaterialAlbedo(const numa::Vec3& albedo) {
		this->albedo = albedo;
	}
	const numa::Vec3& Lambertian::GetMaterialAlbedo() const {
		return albedo;
	}

}