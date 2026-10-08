#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <filesystem>

struct CPUStats
{
    std::string name;
    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
    long long irq;
    long long softirq;
    long long steal;
};

std::vector<CPUStats> readCPUStats()
{
    std::ifstream file("/proc/stat");

    if (!file.is_open())
    {
        std::cout << "Failed to open /proc/stat\n";
        return {};
    }

    std::vector<CPUStats> stats;

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream stream(line);

        std::string cpuName;
        stream >> cpuName;

        if (cpuName.rfind("cpu", 0) != 0)
        {
            break;
        }

        CPUStats cpu;

        cpu.name = cpuName;

        stream >> cpu.user
               >> cpu.nice
               >> cpu.system
               >> cpu.idle
               >> cpu.iowait
               >> cpu.irq
               >> cpu.softirq
               >> cpu.steal;

        stats.push_back(cpu);
    }

    return stats;
}

long long getTotalCPUTime(const CPUStats& cpu)
{
    return cpu.user +
           cpu.nice +
           cpu.system +
           cpu.idle +
           cpu.iowait +
           cpu.irq +
           cpu.softirq +
           cpu.steal;
}

double calculateCPUUsage(
    const CPUStats& first,
    const CPUStats& second)
{
    long long firstIdleTime =
        first.idle + first.iowait;

    long long secondIdleTime =
        second.idle + second.iowait;

    long long firstTotalTime =
        getTotalCPUTime(first);

    long long secondTotalTime =
        getTotalCPUTime(second);

    long long totalDifference =
        secondTotalTime - firstTotalTime;

    long long idleDifference =
        secondIdleTime - firstIdleTime;

    long long activeDifference =
        totalDifference - idleDifference;

    if (totalDifference == 0)
    {
        return 0.0;
    }

    double cpuUsage =
        100.0 *
        static_cast<double>(activeDifference) /
        static_cast<double>(totalDifference);

    return cpuUsage;
}

long long getCPUFrequency(int cpuNumber)
{
    std::string path =
        "/sys/devices/system/cpu/cpu" +
        std::to_string(cpuNumber) +
        "/cpufreq/scaling_cur_freq";

    std::ifstream file(path);

    if (!file.is_open())
    {
        return 0;
    }

    long long frequency;

    file >> frequency;

    return frequency;
}

double getCPUTemperature()
{
    std::string hwmonPath;

    // Search all hwmon devices
    for (const auto& entry :
         std::filesystem::directory_iterator("/sys/class/hwmon"))
    {
        std::filesystem::path namePath =
            entry.path() / "name";

        std::ifstream nameFile(namePath);

        if (!nameFile.is_open())
        {
            continue;
        }

        std::string name;

        nameFile >> name;

        if (name == "coretemp")
        {
            hwmonPath = entry.path();
            break;
        }
    }

    // coretemp was not found
    if (hwmonPath.empty())
    {
        return 0.0;
    }

    // Search for Package id 0
    for (const auto& entry :
         std::filesystem::directory_iterator(hwmonPath))
    {
        std::string filename =
            entry.path().filename().string();

        if (filename.find("_label") == std::string::npos)
        {
            continue;
        }

        std::ifstream labelFile(entry.path());

        if (!labelFile.is_open())
        {
            continue;
        }

        std::string label;

        std::getline(labelFile, label);

        if (label == "Package id 0")
        {
            std::string inputFile =
                filename.substr(
                    0,
                    filename.length() - 6
                ) + "_input";

            std::filesystem::path inputPath =
                entry.path().parent_path() / inputFile;

            std::ifstream temperatureFile(inputPath);

            if (!temperatureFile.is_open())
            {
                return 0.0;
            }

            long long temperature;

            temperatureFile >> temperature;

            return temperature / 1000.0;
        }
    }

    return 0.0;
}

int main()
{
    std::vector<CPUStats> first =
        readCPUStats();

    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );

        std::vector<CPUStats> second =
            readCPUStats();

        std::cout << "\033[2J\033[H";

        // CPU temperature
        double temperature =
            getCPUTemperature();

        std::cout << "CPU Temperature: "
                  << temperature
                  << " °C\n\n";

        // CPU usage and frequency
        for (size_t i = 0;
             i < first.size();
             i++)
        {
            double usage =
                calculateCPUUsage(
                    first[i],
                    second[i]
                );

            std::cout << first[i].name
                      << ": "
                      << usage
                      << "%";

            // Skip frequency for overall CPU
            if (i > 0)
            {
                long long frequency =
                    getCPUFrequency(i - 1);

                std::cout << " | "
                          << frequency / 1000.0
                          << " MHz";
            }

            std::cout << "\n";
        }

        first = second;
    }

    return 0;
}