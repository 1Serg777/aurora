#pragma once

#include "Framework/Components/Material.h"

#include "Vec.hpp"

namespace aurora {

	class Lambertian : public Material {
	public:
		Lambertian();
		Lambertian(const numa::Vec3& albedo);

		numa::Vec3 Scatter(const numa::Vec3& wo, const numa::Vec3& N,
			               numa::Vec3& brdf, float& pdf) const override;

		numa::Vec3 Brdf() const;
		float Pdf(float cosTheta) const;

		void SetMaterialAlbedo(const numa::Vec3& albedo);
		const numa::Vec3& GetMaterialAlbedo() const;

	private:
		numa::Vec3 albedo{1.0f, 1.0f, 1.0f};
	};

}