#include "Core/Integral.h"

#include <Vec.hpp>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace aurora
{
	bool ReimannSumJob::DoWork()
	{
		using namespace std::placeholders;

		float x0{ 0.0f };
		float x1{ 1.0f };

		float y0{ 0.0f };
		float y1{ 1.0f };

		float z0{ 0.0f };
		float z1{ 1.0f };

		float dx{ 0.0001f };
		float dy{ 0.0001f };
		float dz{ 0.0001f };

		uint32_t N = std::floorf((x1 - x0) / dx); // x segments
		uint32_t M = std::floorf((y1 - y0) / dy); // y segments
		uint32_t K = std::floorf((z1 - z0) / dz); // z segments

		std::function<float(float, float, float)> integrand = [](float x, float y, float z) {
			return 0.5f * x + 1.5f * y + z * z;
		};

		float result{ 0.0f };
		for (uint32_t z = z0; z < z1; z += dz)
		{
			for (uint32_t y = y0; y < y1; y += dy)
			{
				for (uint32_t x = x0; x < x1; x += dx)
				{
					// numa::Vec<float, N> x_integral{}; // N must be a constant!
				}
			}
		}

		return result;
	}

	void ReimannSumJob::OnStart()
	{
		// TODO
		// std::clog << "\rProgress: " << std::setprecision(3) << donePercentage * 100.0f << "%   ";
	}
	void ReimannSumJob::OnEnd()
	{
		// TODO
		// std::clog << "\rProgress: " << std::setprecision(3) << donePercentage * 100.0f << "%   ";
	}
}