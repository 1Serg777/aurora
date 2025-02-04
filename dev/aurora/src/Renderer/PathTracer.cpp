#include "Renderer/PathTracer.h"

#include "Framework/Actor.h"
#include "Framework/Camera.h"

#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"

#include "Framework/Geometry/Sphere.h"
#include "Framework/Geometry/Plane.h"

#include "Framework/Materials/Lambertian.h"

#include "Framework/Gradient.h"

#include "Numa.h"
#include "Random.h"
#include "Vec3.hpp"

#include <iostream>
#include <cassert>

namespace aurora
{
	void PathTracer::ClearPixelBuffer(const numa::Vec3& clearColor)
	{
		pixelBuffer->Fill(clearColor);
	}

	void PathTracer::RenderScene(std::shared_ptr<Scene> scene)
	{
		Camera* camera = scene->GetCamera();

		uint32_t resolution_x = camera->GetCameraResolution_X();
		uint32_t resolution_y = camera->GetCameraResolution_Y();

		pixelBuffer.reset();
		pixelBuffer = std::make_shared<f32PixelBuffer>(resolution_x, resolution_y);

		size_t pixelsToRender = static_cast<size_t>(resolution_x) * resolution_y;

		for (uint32_t y = 0; y < resolution_y; y++)
		{
			float progress = static_cast<float>(y + 1) / resolution_y;
			progress *= 100.0f;
			for (uint32_t x = 0; x < resolution_x; x++)
			{
				size_t current_pixel_idx = static_cast<size_t>(y) * resolution_x + x;
				RenderPixel(x, y, *scene);
			}
			std::clog << "\rProgress: " << progress << "%    " << std::flush;
		}
	}

	void PathTracer::RenderPixel(uint32_t raster_coord_x, uint32_t raster_coord_y, const Scene& scene)
	{
		Camera* sceneCamera = scene.GetCamera();
		numa::Ray ray = sceneCamera->GenerateCameraRay(raster_coord_x, raster_coord_y);

		int rayDepth{ 0 };
		numa::Vec3 pixelColor{ 0.0f };

		pixelColor = ComputeColor(ray, scene, rayDepth);

		pixelBuffer->WritePixel(raster_coord_x, raster_coord_y, pixelColor);
	}

	numa::Vec3 PathTracer::ComputeColor(const numa::Ray& ray, const Scene& scene, int rayDepth)
	{
		Gradient skyGradient{
			numa::Vec3{ 1.0f, 1.0f, 1.0f },
			numa::Vec3{ 0.5f, 0.7f, 1.0f },
		};

		float t = ray.GetDirection().y * 0.5f + 0.5f;
		numa::Vec3 bgColor = skyGradient.GetColor(t);

		numa::Vec3 pixelColor{ 0.0f, 0.0f, 0.0f };

		if (rayDepth >= rayDepthLimit)
			return numa::Vec3{ 0.0f, 0.0f, 0.0f };

		ActorRayHit rayHit{};
		if (scene.IntersectClosest(ray, rayHit) && rayHit.hitActor)
		{
			// Hit something, use this object's color

			if (rayHit.hitActor->HasMaterial())
				pixelColor = ShadeMaterial(rayHit, scene, rayDepth);
			else
				pixelColor = bgColor;
		}
		else
		{
			// Missed, use the background color

			pixelColor = bgColor;
		}

		return pixelColor;
	}

	numa::Vec3 PathTracer::ShadeMaterial(const ActorRayHit& rayHit, const Scene& scene, int rayDepth)
	{
		numa::Vec3 pixelColor{ 0.0f, 0.0f, 0.0f };

		switch (rayHit.hitActor->GetMaterial()->GetMaterialType())
		{
			case MaterialType::LAMBERTIAN:
			{
				Lambertian* lambertianMat = static_cast<Lambertian*>(rayHit.hitActor->GetMaterial());
				pixelColor = ShadeLambertian(rayHit, scene, lambertianMat, rayDepth);
			}
			break;
			default:
			{
				assert(false && "Material type is not supported!");
			}
			break;
		}

		return pixelColor;
	}
	numa::Vec3 PathTracer::ShadeLambertian(const ActorRayHit& rayHit, const Scene& scene, const Lambertian* lambertian, int rayDepth)
	{
		numa::Vec3 pointAlbedo{};

		numa::Vec3 materialAlbedo = lambertian->GetMaterialAlbedo();
		numa::Vec3 normalColor = rayHit.hitNormal * 0.5f + numa::Vec3{ 0.5f };

		// pointAlbedo = materialAlbedo;
		pointAlbedo = normalColor;

		return pointAlbedo;
	}

	const f32PixelBuffer* PathTracer::GetPixelBuffer() const
	{
		return pixelBuffer.get();
	}
}