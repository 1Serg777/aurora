#include "Renderer/PathTracer.h"

#include "Framework/Actor.h"
#include "Framework/Camera.h"

#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"

#include "Framework/Geometry/Sphere.h"
#include "Framework/Geometry/Plane.h"

#include "Framework/Gradient.h"

#include "Numa.h"
#include "Random.h"
#include "Vec3.hpp"

#include <iostream>
#include <cassert>

namespace aurora
{
	void PathTracer::InitializePixelBuffer(uint32_t width, uint32_t height)
	{
		pixelBuffer.reset();
		pixelBuffer = std::make_shared<f32PixelBuffer>(width, height);
	}
	void PathTracer::ClearPixelBuffer(const numa::Vec3& clearColor)
	{
		pixelBuffer->Fill(clearColor);
	}

	void PathTracer::RenderScene(std::shared_ptr<Scene> scene)
	{
		Camera* camera = scene->GetCamera();

		uint32_t resolution_x = camera->GetCameraResolution_X();
		uint32_t resolution_y = camera->GetCameraResolution_Y();

		InitializePixelBuffer(resolution_x, resolution_y);

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

		int rayDepth{ 0 };
		numa::Vec3 pixelColor{ 0.0f };

		if (sampleCount > 1)
		{
			for (int sample = 0; sample < sampleCount; sample++)
			{
				numa::Ray ray = sceneCamera->GenerateCameraRayJittered(raster_coord_x, raster_coord_y);
				pixelColor += ComputeColor(ray, scene, rayDepth);
			}
			float scaleFactor = 1.0f / sampleCount;
			pixelColor *= scaleFactor;
		}
		else
		{
			numa::Ray ray = sceneCamera->GenerateCameraRay(raster_coord_x, raster_coord_y);
			pixelColor = ComputeColor(ray, scene, rayDepth);
		}

		pixelBuffer->WritePixel(raster_coord_x, raster_coord_y, pixelColor);
	}

	void PathTracer::ToneMap()
	{
		uint32_t resolution_x = pixelBuffer->GetWidth();
		uint32_t resolution_y = pixelBuffer->GetHeight();

		// Normalize pixel values

		for (uint32_t y = 0; y < resolution_y; y++)
		{
			for (uint32_t x = 0; x < resolution_x; x++)
			{
				numa::Vec3 radiance = pixelBuffer->GetPixelValue(x, y);
				
				// radiance = numa::Pow(radiance, 0.5f);
				// radiance = numa::Pow(radiance, 1.0f / 2.2f);
				radiance = numa::Sqrt(radiance);

				pixelBuffer->WritePixel(x, y, radiance);
			}
		}
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

		if (rayDepth > rayDepthLimit)
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
			case MaterialType::METAL:
			{
				Metal* metalMat = static_cast<Metal*>(rayHit.hitActor->GetMaterial());
				pixelColor = ShadeMetal(rayHit, scene, metalMat, rayDepth);
			}
			break;
			case MaterialType::DIELECTRIC:
			{
				Dielectric* dielectricMat = static_cast<Dielectric*>(rayHit.hitActor->GetMaterial());
				pixelColor = ShadeDielectric(rayHit, scene, dielectricMat, rayDepth);
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
		numa::Vec3 pointAlbedo = lambertian->GetMaterialAlbedo();

		float bias{ 0.00001f };
		numa::Vec3 hitPoint = rayHit.hitPoint + bias * rayHit.hitNormal;

		// Indirect lighting

		// 1. Lambertian

		//numa::Vec3 scatteredDir = numa::RandomOnUnitSphere();
		//if (numa::Dot(scatteredDir, rayHit.hitNormal) < 0.0f)
		//{
		//	scatteredDir = -scatteredDir;
		//}

		// 2. Lambertian (fixed)

		// numa::Vec3 scatteredDir = numa::Normalize(rayHit.hitNormal + numa::RandomOnUnitSphere());

		// 3. Lambertian (fixed 2)

		numa::Vec3 scatteredDir = numa::Normalize(rayHit.hitNormal + numa::RandomInUnitCube());

		if (numa::Length2(scatteredDir) < 1e-10)
			scatteredDir = rayHit.hitNormal;

		numa::Ray scatteredRay{ hitPoint, scatteredDir };
		return pointAlbedo * ComputeColor(scatteredRay, scene, ++rayDepth);
	}
	numa::Vec3 PathTracer::ShadeMetal(const ActorRayHit& rayHit, const Scene& scene, const Metal* metal, int rayDepth)
	{
		numa::Vec3 attenuation = metal->GetAttenuation();

		float bias{ 0.00001f };
		numa::Vec3 hitPoint = rayHit.hitPoint + bias * rayHit.hitNormal;

		numa::Vec3 reflectedDir = metal->Reflect(rayHit.hitRay.GetDirection(), rayHit.hitNormal);

		numa::Ray reflectedRay{ hitPoint, reflectedDir };
		numa::Vec3 color = attenuation * ComputeColor(reflectedRay, scene, ++rayDepth);
		return color;
	}
	numa::Vec3 PathTracer::ShadeDielectric(const ActorRayHit& rayHit, const Scene& scene, const Dielectric* dielectric, int rayDepth)
	{
		// Wait, what value for the ior parameter should I provide to the function?
		FresnelData fresnelData = dielectric->Fresnel(rayHit.hitRay.GetDirection(), rayHit.hitNormal, 1.0f);
		// Should we somehow remember this somewhere? 'RayHit' probably?

		// numa::Vec3 pointToShade = rayHit.hitRay.GetPoint(rayHit.hitDistance);
		numa::Vec3 pointToShade = rayHit.hitPoint;
		numa::Vec3 pointToShadeR = numa::Vec3{ 0.0f };
		numa::Vec3 pointToShadeT = numa::Vec3{ 0.0f };

		// glm::vec3 pointToShadeR = rayHit.hitRay.GetPoint(rayHit.hitDistanceClose) + Ray::defaultBias * rayHit.hitNormal;
		// glm::vec3 pointToShadeT = rayHit.hitRay.GetPoint(rayHit.hitDistanceClose) - Ray::defaultBias * rayHit.hitNormal;

		// glm::vec3 pointToShadeR = rayHit.hitRay.GetPoint(rayHit.hitDistanceClose);
		// glm::vec3 pointToShadeT = rayHit.hitRay.GetPoint(rayHit.hitDistanceClose);

		numa::Vec3 attenuation = dielectric->GetAttenuation();
		numa::Vec3 color{ 0.0f, 0.0f, 0.0f };

		float bias{ 0.00001f };
		float rayDir_dot_hitNorm = numa::Dot(rayHit.hitRay.GetDirection(), rayHit.hitNormal);
		if (rayDir_dot_hitNorm < 0.0f) // We're entering the object!
		{
			// Ray reflectedRay{ pointToShadeR, fresnelData.reflected };
			// color += fresnelData.reflectedLightRatio * ComputeColor(reflectedRay, ++rayDepth, scene) * attenuation;

			// That's what the Fresnel() method does internally
			// Here we just have to avoid "waisting" rayDepth for computing reflected color.
			// 
			// fresnelData.reflectedLightRatio = 0.0f;
			// fresnelData.refractedLightRatio = 1.0f;

			pointToShadeR = pointToShade + bias * rayHit.hitNormal;
			pointToShadeT = pointToShade - bias * rayHit.hitNormal;

			// Ray reflectedRay{ pointToShadeR, fresnelData.reflected };
			// color += fresnelData.reflectedLightRatio * ComputeColor(reflectedRay, ++rayDepth, scene) * attenuation;
		}
		else // We're leaving the object!
		{
			pointToShadeR = pointToShade - bias * rayHit.hitNormal;
			pointToShadeT = pointToShade + bias * rayHit.hitNormal;
		}

		// Ray reflectedRay{ pointToShade, fresnelData.reflected };
		numa::Ray reflectedRay{ pointToShadeR, fresnelData.reflected };
		color += fresnelData.reflectedLightRatio * ComputeColor(reflectedRay, scene, rayDepth + 1) * attenuation;

		// Ray refractedRay{ pointToShade, fresnelData.refracted };
		numa::Ray refractedRay{ pointToShadeT, fresnelData.refracted };
		color += fresnelData.refractedLightRatio * ComputeColor(refractedRay, scene, rayDepth + 1) * attenuation;

		// Less accurate (inaccurate) because we waste all the depth available for reflected rays.
		// color += fresnelData.refractedLightRatio * ComputeColor(refractedRay, ++rayDepth, scene) * attenuation;

		return color;
	}

	const f32PixelBuffer* PathTracer::GetPixelBuffer() const
	{
		return pixelBuffer.get();
	}
}