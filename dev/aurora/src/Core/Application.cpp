#include "Core/Application.h"

#include "Framework/Camera.h"

#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"
#include "Framework/Components/Transform.h"

#include "Framework/Materials/Lambertian.h"
#include "Framework/Materials/Metal.h"

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

		// 1. Multiple threads
		RenderActiveScene(sceneManager->GetActiveScene());

		// 2. Single thread
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

		// [TEST]
		// threadCount = 16;
		// threadCount = 8;
		// threadCount = 4;
		// threadCount = 1;

		taskManager = std::make_unique<TaskManager>();
		taskManager->InitializeRenderingWorkers(threadCount);
	}
	
	void Application::CreateDemoScene()
	{
		// Geometries

		std::shared_ptr<Sphere> lambertianSphereGeometry = std::make_shared<Sphere>(1.0f);
		std::shared_ptr<Sphere> metalSphereGeometry = std::make_shared<Sphere>(1.0f);
		std::shared_ptr<Sphere> fuzzyMetalSphereGeometry = std::make_shared<Sphere>(1.0f);

		std::shared_ptr<Plane> lambertianPlaneGeometry = std::make_shared<Plane>();

		// Transforms

		std::shared_ptr<Transform> cameraTransform = std::make_shared<Transform>();
		cameraTransform->SetWorldPosition(numa::Vec3{ 2.0f, 2.0f, 2.0f });
		cameraTransform->SetRotation(numa::Vec3{ -20.0f, 30.0f, 0.0f });

		std::shared_ptr<Transform> lambertianSphereTransform = std::make_shared<Transform>();
		lambertianSphereTransform->SetWorldPosition(numa::Vec3{ 0.0f, 0.0f, -3.0f });
		lambertianSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		std::shared_ptr<Transform> metalSphereTransform = std::make_shared<Transform>();
		metalSphereTransform->SetWorldPosition(numa::Vec3{ -2.0f, 0.0f, -3.0f });
		metalSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		std::shared_ptr<Transform> fuzzyMetalSphereTransform = std::make_shared<Transform>();
		fuzzyMetalSphereTransform->SetWorldPosition(numa::Vec3{ 2.0f, 0.0f, -3.0f });
		fuzzyMetalSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		std::shared_ptr<Transform> lambertianPlaneTransform = std::make_shared<Transform>();
		lambertianPlaneTransform->SetWorldPosition(numa::Vec3{ 0.0f, -1.0f, 0.0f });
		lambertianPlaneTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		// Materials

		numa::Vec3 lamberttianSphereAlbedo{ 0.28f, 0.48f, 0.65f };
		std::shared_ptr<Lambertian> lambertianSphereMaterial = std::make_shared<Lambertian>(lamberttianSphereAlbedo);

		numa::Vec3 metalSphereAlbedo{ 0.5f, 0.5f, 0.5f };
		std::shared_ptr<Metal> metalSphereMaterial = std::make_shared<Metal>(metalSphereAlbedo);

		numa::Vec3 fuzzyMetalSphereAlbedo{ 0.5f, 0.5f, 0.5f };
		std::shared_ptr<Metal> fuzzyMetalSphereMaterial = std::make_shared<Metal>(fuzzyMetalSphereAlbedo, 0.5f);

		numa::Vec3 lambertianPlaneAlbedo{ 0.48f, 0.65f, 0.28f };
		std::shared_ptr<Lambertian> lambertianPlaneMaterial = std::make_shared<Lambertian>(lambertianPlaneAlbedo);

		numa::Vec3 glassSphereAttenuation{ 1.0f, 1.0f, 1.0f };
		float glassSphereIOR = 1.5f;
		std::shared_ptr<Dielectric> dielectricMat = std::make_shared<Dielectric>(glassSphereAttenuation, glassSphereIOR);

		// Scene definition

		std::shared_ptr<Scene> demoScene = std::make_shared<Scene>("demo_scene");

		// Camera

		std::shared_ptr<Camera> camera = std::make_shared<Camera>(1920, 1080, 90.0f);
		camera->SetTransform(cameraTransform);

		// Actors

		// Lambertian Sphere

		std::shared_ptr<Actor> lambertianSphereActor = std::make_shared<Actor>("lambertian_sphere");
		lambertianSphereActor->SetTransform(lambertianSphereTransform);
		lambertianSphereActor->SetGeometry(lambertianSphereGeometry);
		lambertianSphereActor->SetMaterial(lambertianSphereMaterial);

		// Metal sphere (left)

		std::shared_ptr<Actor> metalSphereActor = std::make_shared<Actor>("metal_sphere");
		metalSphereActor->SetTransform(metalSphereTransform);
		metalSphereActor->SetGeometry(metalSphereGeometry);
		// metalSphereActor->SetMaterial(metalSphereMaterial);
		metalSphereActor->SetMaterial(dielectricMat);

		// Metal sphere (right)

		std::shared_ptr<Actor> fuzzyMetalSphereActor = std::make_shared<Actor>("fuzzy_metal_sphere");
		fuzzyMetalSphereActor->SetTransform(fuzzyMetalSphereTransform);
		fuzzyMetalSphereActor->SetGeometry(fuzzyMetalSphereGeometry);
		fuzzyMetalSphereActor->SetMaterial(fuzzyMetalSphereMaterial);

		// Plane

		std::shared_ptr<Actor> lambertianPlaneActor = std::make_shared<Actor>("lambertian_plane");
		lambertianPlaneActor->SetTransform(lambertianPlaneTransform);
		lambertianPlaneActor->SetGeometry(lambertianPlaneGeometry);
		lambertianPlaneActor->SetMaterial(lambertianPlaneMaterial);

		// 3. Adding the actors

		demoScene->AddCamera(camera);

		demoScene->AddActor(lambertianSphereActor);
		demoScene->AddActor(metalSphereActor);
		demoScene->AddActor(fuzzyMetalSphereActor);
		demoScene->AddActor(lambertianPlaneActor);

		sceneManager->SetActiveScene(demoScene);
	}

	void Application::RenderActiveScene(std::shared_ptr<Scene> scene)
	{
		// 1. Render the scene

		CreateSceneRenderingJob(scene);

		taskManager->ExecuteRenderingJobs();

		// 2. Tone mapping

		pathTracer->ToneMap();

		// 3. Save the image in a file

		std::string fileName{ scene->GetSceneName() };
		fileName.append(".ppm");

		std::filesystem::path filePath = exePath / fileName;

		imageWriter->ChangeFileName(filePath.generic_string().c_str());
		imageWriter->WritePixels(*pathTracer->GetPixelBuffer());
	}

	void Application::CreateSceneRenderingJob(std::shared_ptr<Scene> scene)
	{
		std::shared_ptr<SceneRenderingJob> sceneRenderingJob =
			std::make_unique<SceneRenderingJob>(
				pathTracer.get(), scene.get());

		taskManager->AddRenderingJob(sceneRenderingJob);
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