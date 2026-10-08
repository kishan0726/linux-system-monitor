#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cstdio>

struct DiskStats
{
    long long sectorsRead;
    long long sectorsWritten;
};

// Get Root Device
std::string getRootDevice()
{
    FILE *pipe = popen("findmnt -no SOURCE /", "r");

    if (!pipe)
        return "";

    char buffer[128];
    std::string result;

    if (fgets(buffer, sizeof(buffer), pipe))
        result = buffer;

    pclose(pipe);

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

// Get Parent Disk
std::string getParentDisk(const std::string &rootDevice)
{
    std::filesystem::path devicePath( rootDevice);
    std::string deviceName = devicePath.filename().string();
    std::filesystem::path sysPath = "/sys/class/block/" + deviceName;
    std::filesystem::path realPath = std::filesystem::canonical(sysPath);

    return realPath.parent_path().filename().string();
}

// Read Disk Information
DiskStats readDiskStats(const std::string &diskName)
{
    std::ifstream file("/proc/diskstats");

    if (!file.is_open())
        return {0, 0};

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream stream(line);

        int major;
        int minor;

        std::string device;

        long long readsCompleted;
        long long readsMerged;
        long long sectorsRead;
        long long timeReading;

        long long writesCompleted;
        long long writesMerged;
        long long sectorsWritten;
        long long timeWriting;

        stream >> major >> minor >> device >> readsCompleted >> readsMerged >> sectorsRead >> timeReading >> writesCompleted >> writesMerged >> sectorsWritten >> timeWriting;

        if (device == diskName)
            return {sectorsRead, sectorsWritten};
    }

    return {0, 0};
}

int main()
{
    std::string rootDevice =
        getRootDevice();

    if (rootDevice.empty())
    {
        std::cout << "Failed to find root device\n";
        return 1;
    }

    std::string diskName = getParentDisk(rootDevice);

    if (diskName.empty())
    {
        std::cout << "Failed to find parent disk\n";
        return 1;
    }

    DiskStats first = readDiskStats(diskName);

    while (true)
    {
        std::cout << "\033[2J\033[H";

        auto space = std::filesystem::space("/");
        auto used = space.capacity - space.free;

        double totalGB = space.capacity / (1024.0 * 1024.0 * 1024.0);
        double usedGB = used / (1024.0 * 1024.0 * 1024.0);
        double freeGB = space.free / (1024.0 * 1024.0 * 1024.0);
        double diskUsage = 100.0 * static_cast<double>(used) / static_cast<double>(space.capacity);

        DiskStats stats = readDiskStats(diskName);
        DiskStats second = readDiskStats(diskName);

        long long readDifference = second.sectorsRead - first.sectorsRead;
        long long writeDifference = second.sectorsWritten - first.sectorsWritten;
        long long readBytes = readDifference * 512;
        long long writeBytes = writeDifference * 512;

        double readSpeed = static_cast<double>(readBytes) / 0.5;
        double readSpeedMB = readSpeed / (1024.0 * 1024.0);
        double writeSpeed = static_cast<double>(writeBytes) / 0.5;
        double writeSpeedMB = writeSpeed / (1024.0 * 1024.0);

        std::cout << "Disk: " << diskName << "\n\n";
        std::cout << "Total Disk: " << totalGB << " GB\n";
        std::cout << "Used Disk: " << usedGB << " GB\n";
        std::cout << "Free Disk: " << freeGB << " GB\n";
        std::cout << "Disk Usage: " << diskUsage << "%\n\n";
        std::cout << "Sectors Read: " << stats.sectorsRead << "\n";
        std::cout << "Sectors Written: " << stats.sectorsWritten << "\n";
        std::cout << "Read Speed: " << readSpeedMB << " MB/s\n";
        std::cout << "Write Speed: " << writeSpeedMB << " MB/s\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        first = second;
    }

    return 0;
}