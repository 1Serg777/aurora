#include "Framework/Materials/ParticipatingMedium.h"

namespace aurora
{
	ParticipatingMedium::ParticipatingMedium()
		: Material(MaterialType::PARTICIPATING_MEDIUM)
	{
	}
	ParticipatingMedium::ParticipatingMedium(const numa::Vec3& mediumColor, float absorptionCoefficient)
		: Material(MaterialType::PARTICIPATING_MEDIUM),
		mediumColor(mediumColor), absorptionCoefficient(absorptionCoefficient)
	{
	}

	float ParticipatingMedium::GetTransmittanceValue(float distance) const
	{
		return std::expf(-absorptionCoefficient * distance);
	}
}