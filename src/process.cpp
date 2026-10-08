#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>

struct ProcessInfo
{
    int pid;
    std::string name;
    long long memory;
    double cpuUsage;
};

// Process Directory
bool isProcessDirectory(const std::filesystem::directory_entry& entry)
{
    if (!entry.is_directory())
        return false;

    std::string name = entry.path().filename().string();

    if (name.empty())
        return false;

    for (char c : name)
        if (!std::isdigit(c))
            return false;

    return true;
}


// Get process name
std::string getProcessName(int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream file(path);

    if (!file.is_open())
        return "";

    std::string name;
    std::getline(file, name);

    return name;
}


// Get process RAM usage
long long getProcessMemory(int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream file(path);

    if (!file.is_open())
        return 0;

    std::string name;
    long long value;
    std::string unit;

    while (file >> name >> value >> unit)
        if (name == "VmRSS:")
            return value;

    return 0;
}


// Get process CPU time
long long getProcessCPUTime(int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream file(path);

    if (!file.is_open())
        return 0;

    std::string line;
    std::getline(file, line);

    size_t openParen = line.find('(');
    size_t closeParen = line.rfind(')');

    if (openParen == std::string::npos || closeParen == std::string::npos)
        return 0;

    std::string remaining = line.substr(closeParen + 2);
    std::istringstream stream(remaining);

    char state;
    stream >> state;
    long long value;

    for (int i = 4; i <= 13; i++)
        stream >> value;

    long long utime;
    long long stime;

    stream >> utime >> stime;

    return utime + stime;
}


// Get total system CPU time
long long getTotalSystemCPUTime()
{
    std::ifstream file("/proc/stat");

    if (!file.is_open())
        return 0;

    std::string line;
    std::getline(file, line);
    std::istringstream stream(line);
    std::string cpuName;

    stream >> cpuName;

    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
    long long irq;
    long long softirq;
    long long steal;

    stream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    return user + nice + system + idle + iowait + irq + softirq + steal;
}


// Get number of logical CPUs
unsigned int getCPUCount()
{
    return std::thread::hardware_concurrency();
}


int main()
{
    std::unordered_map<int, long long> previousCPUTime;

    long long previousSystemCPU = getTotalSystemCPUTime();

    for (const auto& entry : std::filesystem::directory_iterator("/proc"))
    {
        if (!isProcessDirectory(entry))
            continue;

        int pid = std::stoi(entry.path().filename().string());
        previousCPUTime[pid] = getProcessCPUTime(pid);
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::cout << "\033[2J\033[H";

        long long currentSystemCPU = getTotalSystemCPUTime();
        long long systemDifference = currentSystemCPU - previousSystemCPU;

        std::vector<ProcessInfo> processes;
        std::unordered_set<int> currentPIDs;

        for (const auto& entry : std::filesystem::directory_iterator("/proc"))
        {
            if (!isProcessDirectory(entry))
                continue;

            int pid = std::stoi(entry.path().filename().string());

            currentPIDs.insert(pid);

            long long currentProcessCPU = getProcessCPUTime(pid);
            auto it = previousCPUTime.find(pid);

            if (it == previousCPUTime.end())
            {
                previousCPUTime[pid] = currentProcessCPU;
                continue;
            }

            long long processDifference = currentProcessCPU - it->second;
            double cpuUsage = 0.0;

            if (systemDifference > 0)
            {
                unsigned int cpuCount = getCPUCount();
                cpuUsage = (static_cast<double>(processDifference) / systemDifference) * cpuCount * 100.0;
            }

            ProcessInfo process;
            process.pid = pid;
            process.name = getProcessName(pid);
            process.memory = getProcessMemory(pid);
            process.cpuUsage = cpuUsage;
            processes.push_back(process);
            previousCPUTime[pid] = currentProcessCPU;
        }

        for (auto it = previousCPUTime.begin(); it != previousCPUTime.end();)
        {
            if (currentPIDs.find(it->first) == currentPIDs.end())
                it = previousCPUTime.erase(it);
            else
                ++it;
        }

        std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b)
            {
                return a.cpuUsage > b.cpuUsage;
            }
        );

        std::cout << std::left << std::setw(8) << "PID" << std::setw(25) << "NAME" << std::setw(15) << "RAM" << std::setw(10) << "CPU" << "\n";

        int count = 0;

        for (const auto& process :
             processes)
        {
            std::cout << std::left << std::setw(8) << process.pid;
            std::cout << std::setw(25) << process.name;
            std::cout << std::setw(15) << std::fixed << std::setprecision(2) << (process.memory / 1024.0) << " MB";
            std::cout << std::setw(10) << std::fixed << std::setprecision(2) << process.cpuUsage << "%";
            std::cout << "\n";

            count++;

            if (count == 10)
                break;
        }

        previousSystemCPU = currentSystemCPU;
    }

    return 0;
}