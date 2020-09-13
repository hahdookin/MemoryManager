#ifndef _MMSYSINFO_H
#define _MMSYSINFO_H

#include "Utils.h"
#include <Windows.h>
#include <string>

// Forward declares (if need be)
namespace MM {
	SYSTEM_INFO SysInfo();
	std::string GetProcessorArchitectureStr(SYSTEM_INFO* sysinfo);
	SYSTEMTIME SysTimeLocal();
	MEMORYSTATUSEX MemInfo();
	DWORDLONG PhysInstalledRam();
}

// Return system info obj
SYSTEM_INFO MM::SysInfo()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo;
}

// Return string repr. of processor architecture
std::string MM::GetProcessorArchitectureStr(SYSTEM_INFO* sysinfo)
{
	std::string type;
	switch (sysinfo->wProcessorArchitecture)
	{
	case 0:
		type = "x86"; break;
	case 5:
		type = "ARM"; break;
	case 6:
		type = "Itanium-Based"; break;
	case 9:
		type = "x64"; break;
	case 12:
		type = "ARM64"; break;
	default:  // Catches 0xffff
		type = "Unknown"; break;
	}
	return type;
}

// Return system local time obj
SYSTEMTIME MM::SysTimeLocal()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	return sysTime;
}


// Return memory info obj
MEMORYSTATUSEX MM::MemInfo()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return memInfo;
}

// Return physically installed ram in kb
DWORDLONG MM::PhysInstalledRam()
{
	DWORDLONG dwPhysRamKB;
	GetPhysicallyInstalledSystemMemory(&dwPhysRamKB);
	return dwPhysRamKB;
}

#endif // !_MMSYSINFO_H

