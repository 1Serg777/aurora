#include "Renderer/PathTracer.h"

#include "Framework/Camera.h"
#include "Framework/Gradient.h"

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

		// numa::Vec3 clearColor{ 0.15f, 0.11f, 0.49f };
		// ClearPixelBuffer(clearColor);

		Camera sceneCamera{
			resolutionX, resolutionY,
			90.0f
		};
		Gradient skyGradient{
			numa::Vec3{ 1.0f, 1.0f, 1.0f }, // white
			numa::Vec3{ 0.5f, 0.7f, 1.0f }, // blue
		};

		size_t pixelsToRender = static_cast<size_t>(resolutionX) * resolutionY;

		for (uint32_t y = 0; y < resolutionY; y++)
		{
			float progress = static_cast<float>(y + 1) / resolutionY;
			progress *= 100.0f;

			for (uint32_t x = 0; x < resolutionX; x++)
			{
				size_t current_pixel_idx = static_cast<size_t>(y) * resolutionX + x;

				numa::Ray ray = sceneCamera.GenerateCameraRay(x, y);

				float t = ray.GetDirection().y * 0.5f + 0.5f;
				numa::Vec3 bgColor = skyGradient.GetColor(t);

				pixelBuffer->WritePixel(x, y, bgColor);

				// RenderPixel(x, y, *scene);
			}
			std::clog << "\rProgress: " << progress << "%    " << std::flush;
		}
	}

	const f32PixelBuffer* PathTracer::GetPixelBuffer() const
	{
		return pixelBuffer.get();
	}
}