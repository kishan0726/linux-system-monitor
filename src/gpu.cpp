#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

struct GPUStats
{
    std::string name;

    double utilization;
    double memoryTotal;
    double memoryUsed;
    double memoryFree;
    double temperature;
    double power;
};

// Read GPU Information
GPUStats readGPUStats()
{
    GPUStats stats{};
    FILE* pipe = popen("nvidia-smi --query-gpu=name,utilization.gpu,memory.total,memory.used,memory.free,temperature.gpu,power.draw --format=csv,noheader,nounits","r");

    if (!pipe)
        return stats;

    char buffer[512];

    if (fgets(buffer, sizeof(buffer), pipe))
    {
        std::string line = buffer;
        std::istringstream stream(line);
        std::string value;
        std::getline(stream, stats.name, ',');
        std::getline(stream, value, ',');
        stats.utilization = std::stod(value);
        std::getline(stream, value, ',');
        stats.memoryTotal = std::stod(value);
        std::getline(stream, value, ',');
        stats.memoryUsed = std::stod(value);
        std::getline(stream, value, ',');
        stats.memoryFree = std::stod(value);
        std::getline(stream, value, ',');
        stats.temperature = std::stod(value);
        std::getline(stream, value, ',');
        stats.power = std::stod(value);
    }

    pclose(pipe);
    return stats;
}

int main()
{
    while (true)
    {
        GPUStats stats = readGPUStats();

        std::cout << "\033[2J\033[H";

        std::cout << "GPU: " << stats.name << "\n\n";
        std::cout << "GPU Usage: " << stats.utilization << "%\n";
        std::cout << "Memory Total: " << stats.memoryTotal << " MiB\n";
        std::cout << "Memory Used: " << stats.memoryUsed << " MiB\n";
        std::cout << "Memory Free: " << stats.memoryFree << " MiB\n";
        std::cout << "Temperature: " << stats.temperature << " C\n";
        std::cout << "Power: " << stats.power << " W\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}