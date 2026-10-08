#include <iostream>
#include <fstream>
#include <string>

// Get Memory Information
long long getMemValue(const std::string &key)
{
    std::ifstream file("/proc/meminfo");

    if (!file.is_open())
    {
        std::cout << "Failed to open /proc/meminfo\n";
        return 0;
    }

    std::string name;
    long long value;
    std::string unit;

    while (file >> name >> value >> unit)
        if (name == key)
            return value;

    return 0;
}

int main()
{
    while (1)
    {
        std::cout << "\033[2J\033[H";

        // Memory
        long long totalMemory = getMemValue("MemTotal:");
        long long availableMemory = getMemValue("MemAvailable:");
        long long usedMemory = totalMemory - availableMemory;
        double memoryUsage = 100.0 * static_cast<double>(usedMemory) / static_cast<double>(totalMemory);
        double totalGB = totalMemory / (1024.0 * 1024.0);
        double availableGB = availableMemory / (1024.0 * 1024.0);
        double usedGB = usedMemory / (1024.0 * 1024.0);

        // swap Memory
        long long swapTotal = getMemValue("SwapTotal:");
        long long swapFree = getMemValue("SwapFree:");
        long long swapUsed = swapTotal - swapFree;
        double swapTotalGB = swapTotal / (1024.0 * 1024.0);
        double swapUsedGB = swapUsed / (1024.0 * 1024.0);
        double swapFreeGB = swapFree / (1024.0 * 1024.0);
        double swapUsage = 0.0;

        if (swapTotal > 0)
            swapUsage = 100.0 * static_cast<double>(swapUsed) / static_cast<double>(swapTotal);

        std::cout << "Total Memory: " << totalGB << " GB\n";
        std::cout << "Available Memory: " << availableGB << " GB\n";
        std::cout << "Used Memory: " << usedGB << " GB\n";
        std::cout << "Memory Usage: " << memoryUsage << "%\n";
        std::cout << "\nSwap Memory\n";
        std::cout << "Total Swap: " << swapTotalGB << " GB\n";
        std::cout << "Used Swap: " << swapUsedGB << " GB\n";
        std::cout << "Free Swap: " << swapFreeGB << " GB\n";
        std::cout << "Swap Usage: " << swapUsage << "%\n";
    }

    return 0;
}