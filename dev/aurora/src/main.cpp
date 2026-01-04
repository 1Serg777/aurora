#include "Core/Application.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace aurora;

int main(int argc, char* argv[]) {
	std::filesystem::path exePath{argv[0]};
	exePath.remove_filename();

	aurora::Application app{exePath};
	try {
		app.Initialize();
		app.Run();
		app.Terminate();
	} catch (const std::runtime_error& re) {
		std::cerr << re.what() << std::endl;
		app.Terminate();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}