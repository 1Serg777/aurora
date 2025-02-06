#include "Core/TaskManager.h"

#include "Renderer/PathTracer.h"
#include "Scene/Scene.h"

namespace aurora
{
	// RenderingWorker class

	void RenderingWorker::Start(RenderingTaskStorage* renderingTaskStorage)
	{
		this->renderingTaskStorage = renderingTaskStorage;

		std::thread thread{ &RenderingWorker::StartImpl, this };
		execThread = std::move(thread);
	}
	void RenderingWorker::Wait()
	{
		if (execThread.joinable())
			execThread.join();
	}

	void RenderingWorker::SetRenderer(PathTracer* pathTracer)
	{
		this->pathTracer = pathTracer;
	}
	void RenderingWorker::SetScene(Scene* scene)
	{
		this->scene = scene;
	}

	void RenderingWorker::StartImpl()
	{
		while (renderingTaskStorage->AcquireRenderingTask(currentRenderingTask))
		{
			for (uint32_t y = currentRenderingTask.raster_y_start; y < currentRenderingTask.raster_y_end; y++)
			{
				for (uint32_t x = currentRenderingTask.raster_x_start; x < currentRenderingTask.raster_x_end; x++)
				{
					pathTracer->RenderPixel(x, y, *scene);
				}
			}
		}
	}

	// TaskManager class

	void TaskManager::InitializeRenderingWorkers(uint32_t threadCount)
	{
		renderingWorkers.resize(threadCount);
		for (int i = 0; i < threadCount; i++)
		{
			renderingWorkers[i] = std::make_unique<RenderingWorker>();
		}
	}

	void TaskManager::RunRenderingWorkers(PathTracer* renderer, Scene* scene)
	{
		for (std::unique_ptr<RenderingWorker>& worker : renderingWorkers)
		{
			worker->SetRenderer(renderer);
			worker->SetScene(scene);
			worker->Start(this);
		}

		StopRenderingWorkers();
	}
	void TaskManager::StopRenderingWorkers()
	{
		for (std::unique_ptr<RenderingWorker>& worker : renderingWorkers)
		{
			worker->Wait();
		}
	}

	void TaskManager::AddRenderingTask(const RenderingTask& renderingTask)
	{
		std::lock_guard<std::mutex> lock{ renderingTaskMutex };

		renderingTasks.push(renderingTask);
	}
	void TaskManager::AddRenderingTasks(const std::vector<RenderingTask>& renderingTasks)
	{
		for (const RenderingTask& renderingTask : renderingTasks)
		{
			this->renderingTasks.push(renderingTask);
		}
	}
	void TaskManager::ClearRenderingTasks()
	{
		std::lock_guard<std::mutex> lock{ renderingTaskMutex };

		while (!renderingTasks.empty())
		{
			renderingTasks.pop();
		}
	}

	bool TaskManager::AcquireRenderingTask(RenderingTask& renderingTask)
	{
		std::lock_guard<std::mutex> lock{ renderingTaskMutex };

		if (renderingTasks.empty())
			return false;

		renderingTask = renderingTasks.top();
		renderingTasks.pop();
		return true;
	}
}