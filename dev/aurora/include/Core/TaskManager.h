#pragma once

#include <chrono>
#include <cstdint>
#include <stack>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

namespace aurora
{
	struct Task
	{
		// TODO
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

	class RenderingJob
	{
	public:

		virtual void OnStart();
		virtual void OnEnd();

		virtual bool Started();
		virtual bool Finished();

		virtual void Reset();

	protected:

		RenderingJob() = default;
		virtual ~RenderingJob() = default;

	private:

		bool executed{ false };
		bool finished{ false };
	};

	class PathTracer;
	class Scene;

	class SceneRenderingJob : public RenderingJob
	{
	public:

		SceneRenderingJob(PathTracer* pathTracer, Scene* scene);
		virtual ~SceneRenderingJob() = default;

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

	class Worker
	{
		// TODO
	};

	class RenderingWorker : public Worker
	{
	public:

		void Start();
		void Stop();

		// Call from the main thread!
		void Wait();

		void SetRenderingJob(SceneRenderingJob* renderingJob);

		bool Running() const;
		bool Executing() const;

	private:

		void StartImpl();

		std::mutex renderingJobMutex{};

		std::thread execThread;

		SceneRenderingJob* renderingJob{ nullptr };

		bool running{ false };
		bool executing{ false };
	};

	class TaskManager
	{
	public:

		void InitializeRenderingWorkers(uint32_t threadCount);

		void AddRenderingJob(std::shared_ptr<SceneRenderingJob> renderingJob);

		void ExecuteRenderingJob();
		void ExecuteRenderingJobs();

	private:

		std::vector<std::unique_ptr<RenderingWorker>> renderingWorkers;
		std::stack<std::shared_ptr<SceneRenderingJob>> sceneRenderingJobs;
	};
}