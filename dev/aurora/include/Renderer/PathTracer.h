#pragma once

#include "Renderer/PixelBuffer.h"

#include "Framework/Actor.h"

#include "Framework/Materials/Dielectric.h"
#include "Framework/Materials/Lambertian.h"
#include "Framework/Materials/Metal.h"
#include "Framework/Materials/ParticipatingMedium.h"

#include "Core/TaskManager.h"

#include "Scene/Scene.h"

#include "Ray.h"
#include "Vec3.hpp"

#include <cstdint>
#include <memory>

namespace aurora
{
	struct ImageRegion
	{
		uint32_t raster_x_start{ 0 };
		uint32_t raster_x_end{ 0 };
		uint32_t raster_y_start{ 0 };
		uint32_t raster_y_end{ 0 };
	};

	class PathTracer
	{
	public:

		void InitializePixelBuffer(uint32_t width, uint32_t height);
		void ClearPixelBuffer(const numa::Vec3& clearColor);

		void RenderScene(std::shared_ptr<Scene> scene);
		void RenderPixels(const ImageRegion& renderRegion, const Scene& scene);
		void RenderPixel(uint32_t raster_coord_x, uint32_t raster_coord_y, const Scene& scene);

		// Tone Mapping Opperators

		void ToneMapReinhardtRGB();
		void ToneMapReinhardtLuminance();
		void ToneMap2();

		// Gamma Correction Procedures

		void GammaCorrectPower12();

		const f32PixelBuffer* GetPixelBuffer() const;

	private:

		numa::Vec3 BackgroundColor(const numa::Ray& ray);

		numa::Vec3 ComputeColor(const numa::Ray& ray, const Scene& scene, int rayDepth);

		numa::Vec3 ShadeMaterial(const ActorRayHit& rayHit, const Scene& scene, int rayDepth);
		numa::Vec3 ShadeLambertian(const ActorRayHit& rayHit, const Scene& scene, const Lambertian* lambertian, int rayDepth);
		numa::Vec3 ShadeMetal(const ActorRayHit& rayHit, const Scene& scene, const Metal* metal, int rayDepth);
		numa::Vec3 ShadeDielectric(const ActorRayHit& rayHit, const Scene& scene, const Dielectric* dielectric, int rayDepth);
		numa::Vec3 ShadeParticipatingMedium(const ActorRayHit& rayHit, const Scene& scene, const ParticipatingMedium* medium, int rayDepth);

		std::shared_ptr<f32PixelBuffer> pixelBuffer;

		int rayDepthLimit{ 5 };

		// int sampleCount{ 50 };
		// int sampleCount{ 25 };
		// int sampleCount{ 15 };
		// int sampleCount{ 10 };
		// int sampleCount{ 5 };
		int sampleCount{ 1 };

		bool multisampling{ false };
	};

	struct RenderingTask : Task
	{
		// TODO
	};

	struct SceneRenderingTask : RenderingTask
	{
		uint32_t raster_x_start{ 0 };
		uint32_t raster_x_end{ 0 };
		uint32_t raster_y_start{ 0 };
		uint32_t raster_y_end{ 0 };
	};

	class SceneRenderingJob : public Job
	{
	public:

		SceneRenderingJob(PathTracer* pathTracer, Scene* scene);
		virtual ~SceneRenderingJob() = default;

		void OnStart() override;
		void OnEnd() override;

		bool DoWork() override;

		bool AcquireRenderingTask(SceneRenderingTask& renderingTask);

		void NotifyRenderingTaskFinished(const SceneRenderingTask& renderingTask);

		PathTracer* pathTracer{ nullptr };
		Scene* scene{ nullptr };

	private:

		void InitializeRenderingTasks();

		void CreateLineRenderingTasks(uint32_t width, uint32_t height, uint32_t lineCount);
		void CreateLineRenderingTask(uint32_t taskIdx, uint32_t lineCount);

		void CreateSquareRenderingTasks(uint32_t width, uint32_t height, uint32_t squareSideSize);

		std::mutex renderingTaskMutex{};
		std::mutex notificationMutex{};

		std::stack<SceneRenderingTask> renderingTasks;

		uint32_t tasksToDo{};
		uint32_t tasksDone{};

		double donePercentage{};

		uint32_t lineCount{};
		uint32_t imageWidth{};
		uint32_t imageHeight{};
	};
}