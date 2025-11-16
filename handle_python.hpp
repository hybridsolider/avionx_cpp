#include <iostream>
#include <cstdlib>
#include "csv_handler.hpp"

#ifdef __unix__
#include <sys/wait.h> // POSIX only
#endif

#ifndef HANDLE_PYTHON
#define HANDLE_PYTHON
#define PYTHON_PATH "./venv/bin/python"
#define PYTHON_FILENAME "main.py"
#include <memory>
#include <string>

std::string call_python(const std::string function, const std::string arguments)
{
    std::string command = std::string(PYTHON_PATH) + " " + PYTHON_FILENAME + " " + function + " " + arguments;
    std::string result;
    char buffer[128];

    // Open pipe to Python process
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        result += buffer;

    pclose(pipe);
    return result;
}

double get_declination(double latitude, double longitude)
{
    std::cout << std::stod(call_python("get_declination", std::to_string(latitude) + " " + std::to_string(longitude)));
    return 0.0;
}


#endif 