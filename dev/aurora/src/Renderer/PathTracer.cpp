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

		// 1. Sphere

		std::shared_ptr<Actor> redSphereActor = std::make_shared<Actor>("red_sphere");

		std::shared_ptr<Transform> redSphereTransform = std::make_shared<Transform>();
		redSphereTransform->SetWorldPosition(numa::Vec3{ 0.0f, 0.0f, -3.0f });
		redSphereTransform->SetRotation(numa::Vec3{ numa::Rad(0.0f), numa::Rad(0.0f), numa::Rad(0.0f) });

		float sphereRadius{ 1.0f };
		std::shared_ptr<Sphere> sphereGeometry = std::make_shared<Sphere>(sphereRadius);

		// numa::Vec3 sphereMatAlbedo{ 1.0f, 0.0f, 0.0f };
		numa::Vec3 sphereMatAlbedo{ 0.8f, 0.8f, 0.8f };
		std::shared_ptr<Lambertian> sphereMaterial = std::make_shared<Lambertian>(sphereMatAlbedo);

		redSphereActor->SetTransform(redSphereTransform);
		redSphereActor->SetGeometry(sphereGeometry);
		redSphereActor->SetMaterial(sphereMaterial);

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

				numa::Ray cameraRay = sceneCamera.GenerateCameraRay(x, y);

				float t = cameraRay.GetDirection().y * 0.5f + 0.5f;
				numa::Vec3 bgColor = skyGradient.GetColor(t);

				numa::Vec3 pixelColor{};

				ActorRayHit actorHit{};
				redSphereActor->Intersect(cameraRay, actorHit);

				if (!actorHit.hit)
				{
					pixelColor = bgColor;
				}
				else
				{
					pixelColor = actorHit.hitNormal * 0.5f + numa::Vec3{ 0.5f };
				}

				pixelBuffer->WritePixel(x, y, pixelColor);

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