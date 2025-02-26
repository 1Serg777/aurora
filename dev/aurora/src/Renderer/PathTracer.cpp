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

#include <cassert>
#include <iostream>
#include <iomanip>

namespace aurora
{
	static constexpr float bias{ 0.00001f };

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

	void PathTracer::RenderPixels(const ImageRegion& renderRegion, const Scene& scene)
	{
		for (uint32_t y = renderRegion.raster_y_start; y < renderRegion.raster_y_end; y++)
		{
			for (uint32_t x = renderRegion.raster_x_start; x < renderRegion.raster_x_end; x++)
			{
				RenderPixel(x, y, scene);
			}
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
			case MaterialType::PARTICIPATING_MEDIUM:
			{
				ParticipatingMedium* medium = static_cast<ParticipatingMedium*>(rayHit.hitActor->GetMaterial());
				pixelColor = ShadeParticipatingMedium(rayHit, scene, medium, rayDepth);
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
	numa::Vec3 PathTracer::ShadeParticipatingMedium(const ActorRayHit& rayHit, const Scene& scene, const ParticipatingMedium* medium, int rayDepth)
	{
		// 1. Handle the ray inside the medium
		//    Assume that there's no nested objects inside the volume,
		//    so we can directly figure out the exit point without
		//    having to check every actor in the scene.
		// 
		//    [TODO]
		//    At some point later on, I should probably figure out a way
		//    to relax that assumption.

		numa::Ray insideVolumeRay{
			numa::Vec3{ rayHit.hitPoint - bias * rayHit.hitNormal },
			numa::Vec3{ rayHit.hitRay.GetDirection() }
		};

		Actor* volumeActor = rayHit.hitActor;
		ActorRayHit exitPointHit{};
		bool exitPointHitCheck = volumeActor->Intersect(insideVolumeRay, exitPointHit);
		if (!exitPointHitCheck) // Intersection was tangent to the volume
		{
			// What should we return? Background or atmosphere color, that is, L(0)?
			// return numa::Vec3{ 0.0f };

			numa::Ray behindVolumeRay{
				numa::Vec3{ exitPointHit.hitPoint + bias * exitPointHit.hitNormal },
				numa::Vec3{ exitPointHit.hitRay.GetDirection() }
			};

			return ComputeColor(behindVolumeRay, scene, rayDepth);
		}

		assert(rayHit.hitActor == exitPointHit.hitActor && "The volume must not have any nested objects!");

		// 2. Now we can start integration.
		//    There are generally two ways to integrate the equation
		//    1) From the camera ray side where the radiance exits the volume toward it
		//    2) From the point where the radiance enters the volume in the camera ray direction
		//    You're free to pick whatever's easier for you, but there are a couple of benefits
		//    to using the first approach.
		//    For the sake of learning, I will try to implement both approaches here.

		float Tr{ 1.0f };

		uint32_t segments = 32;
		float t = exitPointHit.hitDistance;
		float dt = t / segments;

		// 2.1 Integrating from the camera ray direction. The side closest to the camera ray.

		for (uint32_t segment = 0; segment < segments; segment++)
		{
			// Move to the next segment and add some jitter within it.

			float t_prime_jitter = 0.5f * dt; // or 't_shift'; could make the jitter random within 'dt'
			float t_prime = segment * dt + t_prime_jitter; // or 'segment_t'

			// Find the point corresponding to 't_prime'

			numa::Vec3 p_prime = insideVolumeRay.GetPoint(t_prime); // 'segment_p'

			// Calculate the segment transmittance as well as the transmittance from 'p_prime' to 'p'
			// where 'p' is where the camera ray entered the volume.

			float segment_Tr = medium->ComputeTransmittance(p_prime, dt);
			Tr *= segment_Tr;

			// [TODO]
		}

		// [TODO]
		// 2.2 Integrating from the radiance entrance direction. The side where L0 enters the volume.
		// [TODO]

		// 3. Handle the background or atmosphere color.
		//    That is, the color that is behind the medium, which is
		//    also denoted L(0) in the Equation of Transfer.

		numa::Ray behindVolumeRay{
			numa::Vec3{ exitPointHit.hitPoint + bias * exitPointHit.hitNormal },
			numa::Vec3{ exitPointHit.hitRay.GetDirection() }
		};
		numa::Vec3 L0 = ComputeColor(behindVolumeRay, scene, rayDepth);

		return Tr * L0 + (1.0f - Tr) * medium->GetMediumColor();
	}

	const f32PixelBuffer* PathTracer::GetPixelBuffer() const
	{
		return pixelBuffer.get();
	}

	// SceneRenderingJob class

	SceneRenderingJob::SceneRenderingJob(PathTracer* pathTracer, Scene* scene)
		: pathTracer(pathTracer), scene(scene)
	{
		InitializeRenderingTasks();
	}

	void SceneRenderingJob::OnStart()
	{
		Job::OnStart();

		Camera* camera = scene->GetCamera();
		uint32_t imageWidth = camera->GetCameraResolution_X();
		uint32_t imageHeight = camera->GetCameraResolution_Y();
		pathTracer->InitializePixelBuffer(imageWidth, imageHeight);

		std::clog << "Rendering scene '" << scene->GetSceneName() << "'...\n";
	}
	void SceneRenderingJob::OnEnd()
	{
		Job::OnStart();

		std::clog << "\nDone rendering scene! Tasks finished: " << tasksDone << " out of " << tasksToDo << "\n";
	}

	bool SceneRenderingJob::DoWork()
	{
		// Acquire rendering task

		SceneRenderingTask renderingTask{};
		if (!AcquireRenderingTask(renderingTask))
		{
			return false;
		}

		// Do the work

		ImageRegion renderRegion{};
		renderRegion.raster_x_start = renderingTask.raster_x_start;
		renderRegion.raster_x_end = renderingTask.raster_x_end;
		renderRegion.raster_y_start = renderingTask.raster_y_start;
		renderRegion.raster_y_end = renderingTask.raster_y_end;

		pathTracer->RenderPixels(renderRegion, *scene);

		NotifyRenderingTaskFinished(renderingTask);

		return true;
	}

	bool SceneRenderingJob::AcquireRenderingTask(SceneRenderingTask& renderingTask)
	{
		std::lock_guard<std::mutex> lock{ renderingTaskMutex };

		if (renderingTasks.empty())
			return false;

		renderingTask = renderingTasks.top();
		renderingTasks.pop();

		return true;
	}

	void SceneRenderingJob::NotifyRenderingTaskFinished(const SceneRenderingTask& renderingTask)
	{
		std::lock_guard<std::mutex> lockNotification{ notificationMutex };

		size_t pixelsRendered_x = static_cast<size_t>(renderingTask.raster_x_end) - renderingTask.raster_x_start;
		size_t pixelsRendered_y = static_cast<size_t>(renderingTask.raster_y_end) - renderingTask.raster_y_start;
		size_t pixelsRendered = pixelsRendered_x * pixelsRendered_y;

		size_t renderingJobPixels = static_cast<size_t>(this->imageWidth) * this->imageHeight;

		float taskPercentage = static_cast<float>(pixelsRendered) / renderingJobPixels;

		donePercentage += taskPercentage;

		std::clog << "\rProgress: " << std::setprecision(3) << donePercentage * 100.0f << "%   ";

		std::lock_guard<std::mutex> lockTask{ renderingTaskMutex };

		// if (renderingTasks.empty())
			// End();

		tasksDone++;

		if (tasksDone == tasksToDo)
			End();
	}

	void SceneRenderingJob::InitializeRenderingTasks()
	{
		std::lock_guard<std::mutex> lock{ renderingTaskMutex };

		Camera* camera = scene->GetCamera();

		this->imageWidth = camera->GetCameraResolution_X();
		this->imageHeight = camera->GetCameraResolution_Y();

		// 1. Line Rendering Tasks

		this->lineCount = 10; // 108 tasks for 1080 lines
		CreateLineRenderingTasks(imageWidth, imageHeight, lineCount);

		tasksToDo = static_cast<uint32_t>(renderingTasks.size());
		tasksDone = 0;
	}

	void SceneRenderingJob::CreateLineRenderingTasks(uint32_t width, uint32_t height, uint32_t lineCount)
	{
		uint32_t taskCount = static_cast<uint32_t>(std::floorf(static_cast<float>(height) / lineCount));

		uint32_t taskIdx{ 0 };
		for (taskIdx = 0; taskIdx < taskCount; taskIdx++)
		{
			SceneRenderingTask renderingTask{};
			renderingTask.raster_x_start = 0;
			renderingTask.raster_x_end = width;
			renderingTask.raster_y_start = taskIdx * lineCount;
			renderingTask.raster_y_end = renderingTask.raster_y_start + lineCount;

			renderingTasks.push(renderingTask);
		}

		// Everything left (if any)

		SceneRenderingTask renderingTask{};
		renderingTask.raster_x_start = 0;
		renderingTask.raster_x_end = width;
		renderingTask.raster_y_start = taskIdx * lineCount;
		renderingTask.raster_y_end =
			std::clamp(
				renderingTask.raster_y_start + lineCount,
				renderingTask.raster_y_start,
				height);

		renderingTasks.push(renderingTask);
	}
	void SceneRenderingJob::CreateLineRenderingTask(uint32_t taskIdx, uint32_t lineCount)
	{
		// TODO
	}

	void SceneRenderingJob::CreateSquareRenderingTasks(uint32_t width, uint32_t height, uint32_t squareSideSize)
	{
		// TODO
	}

}