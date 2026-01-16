#include "app.h"
#include <iostream>
#include <exception>

int main()
{
    try {
        std::cout << "Starting application..." << std::endl;
        SimpleShapeApplication app(650, 480, PROJECT_NAME, true);
        std::cout << "Application created, calling run..." << std::endl;
        app.run(1);
        std::cout << "Application finished." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        std::cin.get(); // Pause to see the error
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
        std::cin.get(); // Pause to see the error
        return -1;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}