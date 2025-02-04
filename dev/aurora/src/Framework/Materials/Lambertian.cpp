#include "Framework/Materials/Lambertian.h"

namespace aurora
{
	Lambertian::Lambertian()
		: Material(MaterialType::LAMBERTIAN)
	{
	}
	Lambertian::Lambertian(const numa::Vec3& albedo)
		: Material(MaterialType::LAMBERTIAN), albedo(albedo)
	{
	}

	void Lambertian::SetMaterialAlbedo(const numa::Vec3& albedo)
	{
		this->albedo = albedo;
	}
	const numa::Vec3& Lambertian::GetMaterialAlbedo() const
	{
		return albedo;
	}
}