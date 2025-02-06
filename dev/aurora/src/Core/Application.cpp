#include "Core/Application.h"

#include "Framework/Camera.h"

#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"
#include "Framework/Components/Transform.h"

#include "Framework/Materials/Lambertian.h"

#include "Framework/Geometry/Plane.h"
#include "Framework/Geometry/Sphere.h"

#include "Numa.h"
#include "Vec3.hpp"

#include <cassert>

namespace aurora
{
	Application::Application(const std::filesystem::path& exePath)
		: exePath(exePath)
	{
	}

	void Application::Initialize()
	{
		CreateImageWriter();
		pathTracer = std::make_unique<PathTracer>();

		sceneManager = std::make_unique<SceneManager>();

		CreateTaskManager();
	}
	void Application::Terminate()
	{
		sceneManager.reset();

		pathTracer.reset();
		imageWriter.reset();
	}

	void Application::Run()
	{
		CreateDemoScene();
		
		RenderActiveScene(sceneManager->GetActiveScene());

		// RenderScene(sceneManager->GetActiveScene());
	}

	void Application::CreateImageWriter()
	{
		PpmImageProps ppmImageProps{};
		ppmImageProps.maxColorValue = 255;
		ppmImageProps.ppmImageFormat = PpmImageFormat::BINARY;
		// ppmImageProps.ppmImageFormat = PpmImageFormat::ASCII;

		if (ppmImageProps.ppmImageFormat == PpmImageFormat::ASCII)
		{
			imageWriter = std::make_unique<PpmAsciiImageWriter>(ppmImageProps);
		}
		else if (ppmImageProps.ppmImageFormat == PpmImageFormat::BINARY)
		{
			imageWriter = std::make_unique<PpmBinaryImageWriter>(ppmImageProps);
		}
		else
		{
			assert(false && "Unsupported PPM Image Format provided!");
		}
	}
	void Application::CreateTaskManager()
	{
		uint32_t requestedThreadCount{ 16 };
		uint32_t physicalCores = std::thread::hardware_concurrency();

		uint32_t threadCount = std::clamp(requestedThreadCount, uint32_t(1), physicalCores);

		taskManager = std::make_unique<TaskManager>();
		taskManager->InitializeRenderingWorkers(threadCount);
	}
	
	void Application::CreateDemoScene()
	{
		std::shared_ptr<Scene> demoScene = std::make_shared<Scene>("demo_scene");

		// numa::Vec3 cameraPosition{ 2.0f, 2.0f, 2.0f };
		// numa::Vec3 cameraRotation{ 0.0f, 30.0f, 0.0f }; // eulerAngles

		numa::Vec3 cameraPosition{ 2.0f, 2.0f, 2.0f };
		numa::Vec3 cameraRotation{ -20.0f, 30.0f, 0.0f }; // eulerAngles
		
		std::shared_ptr<Transform> cameraTransform = std::make_shared<Transform>(cameraRotation, cameraPosition);

		// std::shared_ptr<Camera> camera = std::make_shared<Camera>(1280, 720, 90.0f);
		// std::shared_ptr<Camera> camera = std::make_shared<Camera>(800, 600, 90.0f);
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(1920, 1080, 90.0f);
		camera->SetTransform(cameraTransform);

		demoScene->AddCamera(camera);

		// 1. Sphere

		std::shared_ptr<Actor> redSphereActor = std::make_shared<Actor>("red_sphere");

		std::shared_ptr<Transform> redSphereTransform = std::make_shared<Transform>();
		redSphereTransform->SetWorldPosition(numa::Vec3{ 0.0f, 0.0f, -3.0f });
		redSphereTransform->SetRotation(numa::Vec3{ numa::Rad(0.0f), numa::Rad(0.0f), numa::Rad(0.0f) });

		float sphereRadius{ 1.0f };
		std::shared_ptr<Sphere> sphereGeometry = std::make_shared<Sphere>(sphereRadius);

		// numa::Vec3 sphereMatAlbedo{ 1.0f, 0.0f, 0.0f };
		numa::Vec3 sphereMatAlbedo{ 0.8f, 0.8f, 0.8f };
		std::shared_ptr<Lambertian> sphereMaterial = std::make_shared<Lambertian>(sphereMatAlbedo);

		redSphereActor->SetTransform(redSphereTransform);
		redSphereActor->SetGeometry(sphereGeometry);
		redSphereActor->SetMaterial(sphereMaterial);

		// 2. Plane

		std::shared_ptr<Actor> greenPlaneActor = std::make_shared<Actor>("green_plane");

		std::shared_ptr<Transform> greenPlaneTransform = std::make_shared<Transform>();
		// greenPlaneTransform->SetWorldPosition(numa::Vec3{ 0.0f, -0.5f, 0.0f });
		greenPlaneTransform->SetWorldPosition(numa::Vec3{ 0.0f, -1.0f, 0.0f });
		greenPlaneTransform->SetRotation(numa::Vec3{ numa::Rad(0.0f), numa::Rad(0.0f), numa::Rad(0.0f) });

		std::shared_ptr<Plane> planeGeometry = std::make_shared<Plane>();

		// numa::Vec3 planeMatAlbedo{ 0.0f, 1.0f, 0.0f };
		numa::Vec3 planeMatAlbedo{ 0.8f, 0.8f, 0.8f };
		std::shared_ptr<Lambertian> planeMaterial = std::make_shared<Lambertian>(planeMatAlbedo);

		greenPlaneActor->SetTransform(greenPlaneTransform);
		greenPlaneActor->SetGeometry(planeGeometry);
		greenPlaneActor->SetMaterial(planeMaterial);

		// 3. Adding the actors

		demoScene->AddActor(redSphereActor);
		demoScene->AddActor(greenPlaneActor);

		sceneManager->SetActiveScene(demoScene);
	}

	void Application::RenderActiveScene(std::shared_ptr<Scene> scene)
	{
		CreateRenderingTasks(scene);

		Camera* camera = scene->GetCamera();
		uint32_t imageWidth = camera->GetCameraResolution_X();
		uint32_t imageHeight = camera->GetCameraResolution_Y();
		pathTracer->InitializePixelBuffer(imageWidth, imageHeight);

		taskManager->RunRenderingWorkers(pathTracer.get(), scene.get());
	}

	void Application::CreateRenderingTasks(std::shared_ptr<Scene> scene)
	{
		Camera* camera = scene->GetCamera();

		uint32_t imageWidth = camera->GetCameraResolution_X();
		uint32_t imageHeight = camera->GetCameraResolution_Y();

		// 1. Line Rendering Tasks
		
		uint32_t lineCount{ 10 }; // 108 tasks
		CreateLineRenderingTasks(imageWidth, imageHeight, lineCount);

		// 2. Square Rendering Tasks

		// TODO
	}

	void Application::CreateLineRenderingTasks(uint32_t width, uint32_t height, uint32_t lineCount)
	{
		uint32_t taskCount = static_cast<uint32_t>(std::floorf(static_cast<float>(height) / lineCount));

		// std::vector<RenderingTask> lineRenderingTasks(taskCount + 1);
		std::vector<RenderingTask> lineRenderingTasks{ taskCount + 1 };

		uint32_t taskIdx{ 0 };
		for (taskIdx = 0; taskIdx < taskCount; taskIdx++)
		{
			RenderingTask lineRenderingTask{};
			lineRenderingTask.raster_x_start = 0;
			lineRenderingTask.raster_x_end = width;
			lineRenderingTask.raster_y_start = taskIdx * lineCount;
			lineRenderingTask.raster_y_end = lineRenderingTask.raster_y_start + lineCount;

			lineRenderingTasks[taskIdx] = lineRenderingTask;
		}

		// Everything left (if any)

		RenderingTask lineRenderingTask{};
		lineRenderingTask.raster_x_start = 0;
		lineRenderingTask.raster_x_end = width;
		lineRenderingTask.raster_y_start = taskIdx * lineCount;
		lineRenderingTask.raster_y_end =
			std::clamp(
				lineRenderingTask.raster_y_start + lineCount,
				lineRenderingTask.raster_y_start,
				height);

		lineRenderingTasks[taskIdx] = lineRenderingTask;

		taskManager->AddRenderingTasks(lineRenderingTasks);
	}
	void Application::CreateSquareRenderingTasks(uint32_t width, uint32_t height, uint32_t squareSideSize)
	{
		// TODO
	}

	void Application::RenderScene(std::shared_ptr<Scene> scene)
	{
		std::string fileName{ scene->GetSceneName() };
		fileName.append(".ppm");

		std::filesystem::path filePath = exePath / fileName;

		imageWriter->ChangeFileName(filePath.generic_string().c_str());
		pathTracer->RenderScene(scene);
		imageWriter->WritePixels(*pathTracer->GetPixelBuffer());
	}
}