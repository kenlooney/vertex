#include <iostream>
#include <string>
#include "vertex/loaders/cpuloader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: osasm <cpu-plugin>" << std::endl;
        return 1;
    }

    CpuLoader loader;
    std::string error;
    if (!loader.open(argv[1], error)) {
        std::cerr << "Failed to load CPU plugin '" << argv[1] << "': " << error << std::endl;
        return 1;
    }
    std::cout << "CPU name: " << loader.cpu()->name() << std::endl;

    if (!loader.close()) {
        std::cerr << "Failed to close CPU plugin." << std::endl;
        return 1;
    }
    return 0;
}
