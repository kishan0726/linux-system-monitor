#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>

struct NetworkStats
{
    long long receivedBytes;
    long long transmittedBytes;
};

// Get Default Interface
std::string getDefaultInterface()
{
    FILE* pipe = popen("ip route | grep default", "r");

    if (!pipe)
        return "";

    char buffer[256];
    std::string line;

    if (fgets(buffer, sizeof(buffer), pipe))
        line = buffer;

    pclose(pipe);
    std::istringstream stream(line);
    std::string word;

    while (stream >> word)
    {
        if (word == "dev")
        {
            std::string interfaceName;
            stream >> interfaceName;
            return interfaceName;
        }
    }

    return "";
}

// Read Network Information
NetworkStats readNetworkStats(const std::string& interfaceName)
{
    std::ifstream file("/proc/net/dev");

    if (!file.is_open())
        return {0, 0};

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream stream(line);
        std::string currentInterface;
        stream >> currentInterface;

        if (currentInterface == interfaceName + ":")
        {
            NetworkStats stats;
            stream >> stats.receivedBytes;

            long long value;

            for (int i = 0; i < 7; i++)
                stream >> value;

            stream >> stats.transmittedBytes;

            return stats;
        }
    }

    return {0, 0};
}

int main()
{
    std::string interfaceName = getDefaultInterface();

    if (interfaceName.empty())
    {
        std::cout << "Failed to find network interface\n";
        return 1;
    }

    NetworkStats first = readNetworkStats(interfaceName);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        NetworkStats second = readNetworkStats(interfaceName);

        long long receivedDifference = second.receivedBytes - first.receivedBytes;
        long long transmittedDifference = second.transmittedBytes - first.transmittedBytes;

        double downloadBytesPerSecond = static_cast<double>(receivedDifference) / 0.5;
        double uploadBytesPerSecond = static_cast<double>(transmittedDifference) / 0.5;
        double downloadMB = downloadBytesPerSecond / (1024.0 * 1024.0);
        double uploadMB = uploadBytesPerSecond / (1024.0 * 1024.0);

        std::cout << "\033[2J\033[H";

        std::cout << "Network Interface: " << interfaceName << "\n\n";
        std::cout << "Download: " << downloadMB << " MB/s\n";
        std::cout << "Upload: " << uploadMB << " MB/s\n";
        std::cout << "\nTotal Received: " << second.receivedBytes << " bytes\n";
        std::cout << "Total Transmitted: " << second.transmittedBytes << " bytes\n";

        first = second;
    }

    return 0;
}