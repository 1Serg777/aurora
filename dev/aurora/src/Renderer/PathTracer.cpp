#include "Renderer/PathTracer.h"

#include "Random.h"
#include "Vec3.hpp"

#include <iostream>

namespace aurora
{
	void PathTracer::ClearPixelBuffer(const numa::Vec3& clearColor)
	{
		pixelBuffer->Fill(clearColor);
	}

	void PathTracer::RenderScene(uint32_t resolutionX, uint32_t resolutionY)
	{
		pixelBuffer.reset();
		pixelBuffer = std::make_shared<f32PixelBuffer>(resolutionX, resolutionY);

		ClearPixelBuffer(numa::Vec3{ 0.15f, 0.11f, 0.49f });
	}

	const f32PixelBuffer* PathTracer::GetPixelBuffer() const
	{
		return pixelBuffer.get();
	}
}