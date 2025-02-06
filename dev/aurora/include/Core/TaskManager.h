#pragma once

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
		uint32_t raster_x_start{ 0 };
		uint32_t raster_x_end{ 0 };
		uint32_t raster_y_start{ 0 };
		uint32_t raster_y_end{ 0 };
	};

	class RenderingTaskStorage
	{
	public:
		virtual void AddRenderingTask(const RenderingTask&) = 0;
		virtual void AddRenderingTasks(const std::vector<RenderingTask>& renderingTasks) = 0;
		virtual bool AcquireRenderingTask(RenderingTask& renderingTask) = 0;
		virtual void ClearRenderingTasks() = 0;
	};

	class Worker
	{
		// TODO
	};

	class PathTracer;
	class Scene;

	class RenderingWorker : public Worker
	{
	public:

		void Start(RenderingTaskStorage* renderingTaskStorage);
		void Wait();

		void SetRenderer(PathTracer* pathTracer);
		void SetScene(Scene* scene);

	private:

		void StartImpl();

		RenderingTask currentRenderingTask{};

		std::thread execThread;

		RenderingTaskStorage* renderingTaskStorage{ nullptr };

		PathTracer* pathTracer{ nullptr };
		Scene* scene{ nullptr };
	};

	

	class TaskManager : public RenderingTaskStorage
	{
	public:

		void InitializeRenderingWorkers(uint32_t threadCount);

		void RunRenderingWorkers(PathTracer* renderer, Scene* scene);
		void StopRenderingWorkers();

		void AddRenderingTask(const RenderingTask& renderingTask) override;
		void AddRenderingTasks(const std::vector<RenderingTask>& renderingTasks) override;
		void ClearRenderingTasks() override;

	private:

		bool AcquireRenderingTask(RenderingTask& renderingTask) override;

		std::vector<std::unique_ptr<RenderingWorker>> renderingWorkers;
		std::stack<RenderingTask> renderingTasks;

		std::mutex renderingTaskMutex{};
	};
}