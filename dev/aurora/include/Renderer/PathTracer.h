#pragma once

#include "Renderer/PixelBuffer.h"

#include "Framework/Actor.h"

#include "Framework/Materials/Lambertian.h"

#include "Scene/Scene.h"

#include "Ray.h"
#include "Vec3.hpp"

#include <cstdint>
#include <memory>

namespace aurora
{
	class PathTracer
	{
	public:

		void InitializePixelBuffer(uint32_t width, uint32_t height);
		void ClearPixelBuffer(const numa::Vec3& clearColor);

		void RenderScene(std::shared_ptr<Scene> scene);

		void RenderPixel(uint32_t raster_coord_x, uint32_t raster_coord_y, const Scene& scene);

		const f32PixelBuffer* GetPixelBuffer() const;

	private:

		numa::Vec3 ComputeColor(const numa::Ray& ray, const Scene& scene, int rayDepth);

		numa::Vec3 ShadeMaterial(const ActorRayHit& rayHit, const Scene& scene, int rayDepth);
		numa::Vec3 ShadeLambertian(const ActorRayHit& rayHit, const Scene& scene, const Lambertian* lambertian, int rayDepth);
		// numa::Vec3 ShadeDielectric(const RayHit& rayHit, const Scene& scene, const Dielectric* dielectric, int rayDepth);
		// numa::Vec3 ShadeMetal(const RayHit& rayHit, const Scene& scene, const Metal* metal, int rayDepth);

		std::shared_ptr<f32PixelBuffer> pixelBuffer;

		int rayDepthLimit{ 5 };

		int sampleCount{ 50 };

		bool multisampling{ false };
	};
}