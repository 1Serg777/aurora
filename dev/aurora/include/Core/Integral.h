#pragma once

#include "Core/TaskManager.h"

#include <functional>

namespace aurora
{
	struct SingleIntegral
	{
		std::function<float(float)> f{};

		float x1{};
		float x0{};

		float dx{};
	};

	struct DoubleIntegral
	{
		std::function<float(float, float)> f{};

		float x1{};
		float x0{};

		float y1{};
		float y0{};

		float dx{};
		float dy{};
	};

	struct TripleIntegral
	{
		std::function<float(float, float, float)> f{};

		float x1{};
		float x0{};

		float y1{};
		float y0{};

		float z1{};
		float z0{};

		float dx{};
		float dy{};
		float dz{};
	};

	class ReimannSumJob : public Job
	{
	public:

		bool DoWork() override;

		void OnStart() override;
		void OnEnd() override;
	};
}