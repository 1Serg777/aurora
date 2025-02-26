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

	class Job
	{
	public:

		void Start();
		virtual void OnStart() {};

		void End();
		virtual void OnEnd() {};

		virtual bool Started();
		virtual bool Executed();
		virtual bool Finished();

		virtual void Reset();

		virtual bool DoWork() = 0;

	protected:

		Job() = default;
		virtual ~Job() = default;

	private:

		bool executed{ false };
		bool finished{ false };
	};

	class Worker
	{
	public:

		void Start();
		void Stop();
		void Wait();
		void Detach();

		void SetJob(Job* job);
		void RemoveJob();

		bool Running() const;
		bool Executing() const;

	private:

		void StartImpl();

		std::mutex jobMutex{};

		std::thread execThread;

		Job* job{ nullptr };

		bool running{ false };
		bool executing{ false };
	};

	class TaskManager
	{
	public:

		void InitializeWorkers(uint32_t threadCount);

		void AddJob(std::shared_ptr<Job> job);

		void ExecuteTopJob();
		void ExecuteAllJobs();

	private:

		std::vector<std::unique_ptr<Worker>> workers;
		std::stack<std::shared_ptr<Job>> jobs;
	};
}