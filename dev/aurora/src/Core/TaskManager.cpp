#include "Core/TaskManager.h"

#include "Renderer/PathTracer.h"
#include "Scene/Scene.h"

#include <iostream>

namespace aurora
{
	// RenderingJob class

	void Job::OnStart()
	{
		executed = true;
		finished = false;
	}
	void Job::OnEnd()
	{
		executed = false;
		finished = true;
	}

	bool Job::Started()
	{
		return Executed() && !Finished();
	}
	bool Job::Executed()
	{
		return executed;
	}
	bool Job::Finished()
	{
		return finished;
	}

	void Job::Reset()
	{
		executed = false;
		finished = false;
	}

	// RenderingWorker class

	void Worker::Start()
	{
		this->running = true;

		std::thread thread{ &Worker::StartImpl, this };
		execThread = std::move(thread);
	}
	void Worker::Stop()
	{
		this->running = false;
		this->executing = false;
	}
	void Worker::Wait()
	{
		if (execThread.joinable())
			execThread.join();
	}
	void Worker::Detach()
	{
		if (execThread.joinable())
			execThread.detach();
	}

	void Worker::SetJob(Job* job)
	{
		std::lock_guard<std::mutex> lock{ jobMutex };
		this->job = job;
	}

	void Worker::RemoveJob()
	{
		std::lock_guard<std::mutex> lock{ jobMutex };
		this->job = nullptr;
	}

	bool Worker::Running() const
	{
		return running;
	}
	bool Worker::Executing() const
	{
		return executing;
	}

	void Worker::StartImpl()
	{
		using namespace std::chrono_literals;

		while (running)
		{
			bool workDone{ false };

			// 1. 
			/*
			jobMutex.lock();
			while (job && !job->Finished())
			{
				executing = true;
				workDone = job->DoWork();
				executing = false;
			}
			jobMutex.unlock();
			*/

			// 2. 
			jobMutex.lock();
			if (job)
			{
				executing = true;
				workDone = job->DoWork();
				executing = false;
			}
			jobMutex.unlock();

			// The wait affects the start and finish times!
			/*
			if (!workDone)
				std::this_thread::sleep_for(1000ms); // 1s.
			*/
		}
	}

	// TaskManager class

	void TaskManager::InitializeWorkers(uint32_t threadCount)
	{
		workers.resize(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
		{
			workers[i] = std::make_unique<Worker>();
		}
	}

	void TaskManager::AddJob(std::shared_ptr<Job> job)
	{
		jobs.push(job);
	}

	void TaskManager::ExecuteTopJob()
	{
		if (jobs.empty())
			return;

		std::shared_ptr<Job> job = jobs.top();
		job->OnStart();

		for (auto& worker : workers)
		{
			worker->Start();
			worker->SetJob(job.get());
		}

		for (auto& worker : workers)
		{
			worker->Stop();
			worker->Wait();
			// worker->Detach();
		}

		jobs.pop();
	}
	void TaskManager::ExecuteAllJobs()
	{
		if (jobs.empty())
			return;

		for (auto& worker : workers)
		{
			worker->Start();
		}

		while (!jobs.empty())
		{
			std::shared_ptr<Job> job = jobs.top();
			job->OnStart();

			// 1. Send the job

			for (auto& worker : workers)
			{
				worker->SetJob(job.get());
			}

			// 2. Stall, periodically checking if the job's done

			using namespace std::chrono_literals;
			while (!job->Finished())
			{
				std::this_thread::sleep_for(1000ms); // 1s.
			}

			// 3. Remove the job

			for (auto& worker : workers)
			{
				worker->RemoveJob();
			}

			jobs.pop();
		}

		for (auto& worker : workers)
		{
			worker->Stop();
			worker->Wait();
			// worker->Detach();
		}
	}
}