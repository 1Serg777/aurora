#include "Core/Application.h"

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
	}
	void Application::Terminate()
	{
		imageWriter.reset();
		pathTracer.reset();
	}

	void Application::Run()
	{
		std::string fileName{ "demo_scene" };
		fileName.append(".ppm");

		std::filesystem::path filePath = exePath / fileName;

		uint32_t resolutionX{ 1920 };
		uint32_t resolutionY{ 1080 };

		imageWriter->ChangeFileName(filePath.generic_string().c_str());
		pathTracer->RenderScene(resolutionX, resolutionY);
		imageWriter->WritePixels(*pathTracer->GetPixelBuffer());
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
}