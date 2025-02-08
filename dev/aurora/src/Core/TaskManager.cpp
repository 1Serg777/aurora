#include "Core/TaskManager.h"

#include "Renderer/PathTracer.h"
#include "Scene/Scene.h"

#include <iostream>
#include <iomanip>

namespace aurora
{
	// RenderingJob class

	void RenderingJob::OnStart()
	{
		executed = true;
		finished = false;
	}
	void RenderingJob::OnEnd()
	{
		executed = false;
		finished = true;
	}

	bool RenderingJob::Started()
	{
		return !executed && !finished;
	}
	bool RenderingJob::Finished()
	{
		return finished;
	}

	void RenderingJob::Reset()
	{
		executed = false;
		finished = false;
	}

	// SceneRenderingJob class

	SceneRenderingJob::SceneRenderingJob(PathTracer* pathTracer, Scene* scene)
		: pathTracer(pathTracer), scene(scene)
	{
		InitializeRenderingTasks();
	}

	void SceneRenderingJob::OnStart()
	{
		RenderingJob::OnStart();

		Camera* camera = scene->GetCamera();
		uint32_t imageWidth = camera->GetCameraResolution_X();
		uint32_t imageHeight = camera->GetCameraResolution_Y();
		pathTracer->InitializePixelBuffer(imageWidth, imageHeight);
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

		std::clog << "\rProgress: " << std::setprecision(3) << donePercentage * 100.0f << "% " << std::flush;

		std::lock_guard<std::mutex> lockTask{ renderingTaskMutex };

		// if (renderingTasks.empty())
			// OnEnd();

		tasksDone++;

		if (tasksDone == tasksToDo)
			OnEnd();
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

	// RenderingWorker class

	void RenderingWorker::Start()
	{
		this->running = true;

		std::thread thread{ &RenderingWorker::StartImpl, this };
		execThread = std::move(thread);
	}
	void RenderingWorker::Stop()
	{
		this->running = false;
		this->executing = false;
	}

	void RenderingWorker::Wait()
	{
		if (execThread.joinable())
			execThread.join();
	}

	void RenderingWorker::SetRenderingJob(SceneRenderingJob* renderingJob)
	{
		std::lock_guard<std::mutex> lock{ renderingJobMutex };
		this->renderingJob = renderingJob;
	}

	bool RenderingWorker::Running() const
	{
		return running;
	}
	bool RenderingWorker::Executing() const
	{
		return executing;
	}

	void RenderingWorker::StartImpl()
	{
		SceneRenderingTask currentRenderingTask{};
		while (running)
		{
			while (renderingJob && renderingJob->AcquireRenderingTask(currentRenderingTask))
			{
				executing = true;
				for (uint32_t y = currentRenderingTask.raster_y_start; y < currentRenderingTask.raster_y_end; y++)
				{
					for (uint32_t x = currentRenderingTask.raster_x_start; x < currentRenderingTask.raster_x_end; x++)
					{
						renderingJob->pathTracer->RenderPixel(x, y, *renderingJob->scene);
					}
				}

				renderingJob->NotifyRenderingTaskFinished(currentRenderingTask);
				executing = false;
			}
		}
	}

	// TaskManager class

	void TaskManager::InitializeRenderingWorkers(uint32_t threadCount)
	{
		renderingWorkers.resize(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
		{
			renderingWorkers[i] = std::make_unique<RenderingWorker>();
		}
	}

	void TaskManager::AddRenderingJob(std::shared_ptr<SceneRenderingJob> renderingJob)
	{
		sceneRenderingJobs.push(renderingJob);
	}

	void TaskManager::ExecuteRenderingJob()
	{
		if (sceneRenderingJobs.empty())
			return;

		std::shared_ptr<SceneRenderingJob> renderingJob = sceneRenderingJobs.top();

		for (auto& worker : renderingWorkers)
		{
			worker->Start();
			worker->SetRenderingJob(renderingJob.get());
		}

		for (auto& worker : renderingWorkers)
		{
			worker->Stop();
		}

		sceneRenderingJobs.pop();
	}
	void TaskManager::ExecuteRenderingJobs()
	{
		if (sceneRenderingJobs.empty())
			return;

		for (auto& worker : renderingWorkers)
		{
			worker->Start();
		}

		while (!sceneRenderingJobs.empty())
		{
			std::shared_ptr<SceneRenderingJob> renderingJob = sceneRenderingJobs.top();
			renderingJob->OnStart();

			// 1. Send the job

			for (auto& worker : renderingWorkers)
			{
				worker->SetRenderingJob(renderingJob.get());
			}

			// 2. Stall, periodically checking if the job's done

			using namespace std::chrono_literals;
			while (!renderingJob->Finished())
			{
				std::this_thread::sleep_for(1000ms); // 1s.
			}

			sceneRenderingJobs.pop();
		}

		for (auto& worker : renderingWorkers)
		{
			worker->Stop();
			worker->Wait();
		}
	}
}