#pragma once

#include "Framework/Components/Material.h"

#include "Vec3.hpp"

namespace aurora
{
	class ParticipatingMedium : public Material
	{
	public:

		ParticipatingMedium();
		ParticipatingMedium(const numa::Vec3& mediumColor, float absorptionCoefficient);

		float GetTransmittanceValue(float distance) const;

		numa::Vec3 mediumColor{ 0.8f };
		float absorptionCoefficient{ 0.0f };
	};
}