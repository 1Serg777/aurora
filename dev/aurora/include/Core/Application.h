#pragma once

#include "Renderer/PathTracer.h"
#include "Renderer/PpmImageWriter.h"

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

		std::filesystem::path exePath{};

		std::unique_ptr<PathTracer> pathTracer;
		std::unique_ptr<PpmImageWriter> imageWriter;
	};
}