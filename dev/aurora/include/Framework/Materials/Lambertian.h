#pragma once

#include "Framework/Components/Material.h"

#include "Vec3.hpp"

namespace aurora
{
	class Lambertian : public Material
	{
	public:

		Lambertian();
		Lambertian(const numa::Vec3& albedo);

		void SetMaterialAlbedo(const numa::Vec3& albedo);
		const numa::Vec3& GetMaterialAlbedo() const;

	private:

		numa::Vec3 albedo{ 1.0f, 1.0f, 1.0f };
	};
}