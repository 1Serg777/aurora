#include "Framework/Materials/Metal.h"

#include "Numa.h"
#include "Random.h"

namespace aurora
{
	Metal::Metal()
		: Material(MaterialType::METAL)
	{
	}
	Metal::Metal(const numa::Vec3& attenuation, float fuzziness)
		: Material(MaterialType::METAL), attenuation(attenuation), fuzziness(fuzziness)
	{
	}

	/*
	numa::Vec3 Metal::Scatter(const numa::Vec3& incidentDirection, const numa::Vec3& normal)
	{
		// TODO
		return numa::Vec3{ 0.0f, 1.0f, 0.0f };
	}
	*/

	numa::Vec3 Metal::Reflect(const numa::Vec3& incidentDirection, const numa::Vec3& normal) const
	{
		numa::Vec3 reflectedDir =
			incidentDirection -
			2.0f * (numa::Dot(incidentDirection, normal)) * normal;

		// Or we can just use the library's corresponding function

		// numa::Vec3 reflectedDir = numa::Reflect(incidentDirection, normal);

		// Handle fuzziness

		numa::Vec3 randomVec = numa::RandomVec3() * fuzziness;
		reflectedDir = numa::Normalize(reflectedDir + randomVec);

		return reflectedDir;
	}

	void Metal::SetAttenuation(const numa::Vec3& attenuation)
	{
		this->attenuation = attenuation;
	}
	const numa::Vec3& Metal::GetAttenuation() const
	{
		return attenuation;
	}
}