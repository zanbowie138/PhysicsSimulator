// Force use of discrete Nvidia GPU
#ifdef _WIN32
#include <windows.h>

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
// __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; // AMD
}
#undef ERROR // Prevent Windows ERROR macro from conflicting with Logger::ERROR
#endif

#include "utils/Logger.h"
#include "Application.h"

int main() {
    LOG_INIT("output.log");
    LOG_SET_PRINT_TO_CONSOLE(true);

    try {
        Application app("OpenGL Window", 900, 900);
        app.Run();
    } catch (const std::exception& e) {
        LOG(LOG_ERROR) << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
