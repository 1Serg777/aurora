#pragma once

#include "Core/TaskManager.h"
#include "Renderer/PathTracer.h"
#include "Renderer/PpmImageWriter.h"
#include "Scene/SceneManager.h"

#include <filesystem>
#include <memory>

namespace aurora
{
	struct AppSettings
	{
		uint32_t physicalCoreCount{ 1 };
		uint32_t threadCount{ 1 };

		int sampleCount{ 1 };

		bool multithreading{ false };
		bool multisampling{ false };
	};

	class Application
	{
	public:

		Application(const std::filesystem::path& exePath);

		void Initialize();
		void Terminate();

		void Run();

	private:

		void CreateImageWriter();
		void CreateTaskManager();

		void CreateDemoScene();

		void RenderActiveScene(std::shared_ptr<Scene> scene);

		void CreateRenderingTasks(std::shared_ptr<Scene> scene);

		void CreateLineRenderingTasks(uint32_t width, uint32_t height, uint32_t lineCount);
		// void CreateLineRenderingTask(uint32_t taskIdx, uint32_t lineCount);

		void CreateSquareRenderingTasks(uint32_t width, uint32_t height, uint32_t squareSideSize);

		void RenderScene(std::shared_ptr<Scene> scene);

		std::filesystem::path exePath{};

		std::unique_ptr<PathTracer> pathTracer;
		std::unique_ptr<PpmImageWriter> imageWriter;

		std::unique_ptr<SceneManager> sceneManager;

		std::unique_ptr<TaskManager> taskManager;
	};
}