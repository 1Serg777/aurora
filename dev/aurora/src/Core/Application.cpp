#include "Core/Application.h"

#include "Framework/Camera.h"

#include "Framework/Components/Geometry.h"
#include "Framework/Components/Material.h"
#include "Framework/Components/Transform.h"

#include "Framework/Materials/Dielectric.h"
#include "Framework/Materials/Lambertian.h"
#include "Framework/Materials/Metal.h"
#include "Framework/Materials/ParticipatingMedium.h"

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
		taskManager->InitializeWorkers(threadCount);
	}
	
	void Application::CreateDemoScene()
	{
		// Geometries

		std::shared_ptr<Sphere> lambertianSphereGeometry = std::make_shared<Sphere>(1.0f);
		std::shared_ptr<Sphere> metalSphereGeometry = std::make_shared<Sphere>(1.0f);
		std::shared_ptr<Sphere> fuzzyMetalSphereGeometry = std::make_shared<Sphere>(1.0f);

		std::shared_ptr<Sphere> participatingMediumSphereGeometry = std::make_shared<Sphere>(1.0f);

		std::shared_ptr<Plane> lambertianPlaneGeometry = std::make_shared<Plane>();

		// Transforms

		std::shared_ptr<Transform> cameraTransform = std::make_shared<Transform>();
		cameraTransform->SetWorldPosition(numa::Vec3{ 1.5f, 1.5f, 2.5f });
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

		std::shared_ptr<Transform> participatingMediumSphereTransform = std::make_shared<Transform>();
		participatingMediumSphereTransform->SetWorldPosition(numa::Vec3{ -0.5f, 0.0f, -0.5f });
		participatingMediumSphereTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		std::shared_ptr<Transform> lambertianPlaneTransform = std::make_shared<Transform>();
		lambertianPlaneTransform->SetWorldPosition(numa::Vec3{ 0.0f, -1.0f, 0.0f });
		lambertianPlaneTransform->SetRotation(numa::Vec3{ 0.0f, 0.0f, 0.0f });

		// Materials

		numa::Vec3 lamberttianSphereAlbedo{ 0.28f, 0.48f, 0.65f };
		std::shared_ptr<Lambertian> lambertianSphereMaterial = std::make_shared<Lambertian>(lamberttianSphereAlbedo);

		numa::Vec3 glassSphereAttenuation{ 1.0f, 1.0f, 1.0f };
		float glassSphereIOR = 1.5f;
		std::shared_ptr<Dielectric> dielectricMat = std::make_shared<Dielectric>(glassSphereAttenuation, glassSphereIOR);

		numa::Vec3 fuzzyMetalSphereAlbedo{ 0.5f, 0.5f, 0.5f };
		std::shared_ptr<Metal> fuzzyMetalSphereMaterial = std::make_shared<Metal>(fuzzyMetalSphereAlbedo, 0.5f);

		float absorptionCoefficient{ 1.0f };
		numa::Vec3 mediumColor{ 0.8f };
		std::shared_ptr<ParticipatingMedium> participatingMediumSphereMaterial =
			std::make_shared<ParticipatingMedium>(mediumColor, absorptionCoefficient);

		numa::Vec3 lambertianPlaneAlbedo{ 0.48f, 0.65f, 0.28f };
		std::shared_ptr<Lambertian> lambertianPlaneMaterial = std::make_shared<Lambertian>(lambertianPlaneAlbedo);

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
		metalSphereActor->SetMaterial(dielectricMat);

		// Metal sphere (right)

		std::shared_ptr<Actor> fuzzyMetalSphereActor = std::make_shared<Actor>("fuzzy_metal_sphere");
		fuzzyMetalSphereActor->SetTransform(fuzzyMetalSphereTransform);
		fuzzyMetalSphereActor->SetGeometry(fuzzyMetalSphereGeometry);
		fuzzyMetalSphereActor->SetMaterial(fuzzyMetalSphereMaterial);

		// Participating medium sphere volume (between the glass and lambertian spheres)

		std::shared_ptr<Actor> participatingMediumSphereActor = std::make_shared<Actor>("participating_medium_sphere");
		participatingMediumSphereActor->SetTransform(participatingMediumSphereTransform);
		participatingMediumSphereActor->SetGeometry(participatingMediumSphereGeometry);
		participatingMediumSphereActor->SetMaterial(participatingMediumSphereMaterial);

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
		demoScene->AddActor(participatingMediumSphereActor);
		demoScene->AddActor(lambertianPlaneActor);

		sceneManager->SetActiveScene(demoScene);
	}

	void Application::RenderActiveScene(std::shared_ptr<Scene> scene)
	{
		// 1. Render the scene

		CreateSceneRenderingJob(scene);

		taskManager->ExecuteAllJobs();

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

		taskManager->AddJob(sceneRenderingJob);
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