#include "Core/Application.h"

#include "Renderer/PpmImageWriter.h"
#include "Renderer/PixelBuffer.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace numa;
using namespace aurora;

int main(int argc, char* argv[])
{
	// u8PixelBuffer pixelBuffer{ 1280, 720 };
	// pixelBuffer.Fill(u8Vec3{ 255, 0, 0 });

	// f32PixelBuffer pixelBuffer{ 1280, 720 };
	// pixelBuffer.Fill(Vec3{ 1.0f, 0.0f, 0.0f });

	// PpmImageProps ppmImageProps{};
	// ppmImageProps.fileName = "out.ppm";
	// ppmImageProps.maxColorValue = 255;
	// ppmImageProps.ppmImageFormat = PpmImageFormat::ASCII;
	// ppmImageProps.ppmImageFormat = PpmImageFormat::BINARY;

	// PpmImageWriter* imageWriter = CreateImageWriter(ppmImageProps);
	// imageWriter->WritePixels(pixelBuffer);

	// DeleteImageWriter(imageWriter);


	std::filesystem::path exePath{ argv[0] };
	exePath.remove_filename();

	aurora::Application app{ exePath };

	try
	{
		app.Initialize();
		app.Run();
		app.Terminate();
	}
	catch (const std::runtime_error& re)
	{
		std::cerr << re.what() << std::endl;
		app.Terminate();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}