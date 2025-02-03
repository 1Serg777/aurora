#pragma once

#include "Renderer/PixelBuffer.h"

#include "Ray.h"
#include "Vec3.hpp"

#include <cstdint>
#include <memory>

namespace aurora
{
	class PathTracer
	{
	public:

		void ClearPixelBuffer(const numa::Vec3& clearColor);

		void RenderScene(uint32_t resolutionX, uint32_t resolutionY);

		const f32PixelBuffer* GetPixelBuffer() const;

	private:

		// std::shared_ptr<u8PixelBuffer> pixelBuffer;
		std::shared_ptr<f32PixelBuffer> pixelBuffer;

		bool multisampling{ false };
	};
}