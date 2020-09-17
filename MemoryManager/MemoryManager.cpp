// Project related includes
#include "MMSysInfo.h"
#include "MMDrawingRoutines.h"
#include "Utils.h"

// Third party dependencies
#include "olcPixelGameEngine.h"
#include <Windows.h>

// Standard includes
#include <iostream>
#include <vector>


constexpr float ratio = 1024.0f * 1024.0f * 1024.0f; // # of bytes in gigabyte


class MemoryManager : public olc::PixelGameEngine
{
public:
	MemoryManager()
	{
		sAppName = "Memory Manager";
	}

public:
	// Screen Height and Width
	float swf; // ScreenWidth as float
	float shf; // ScreenHeight as float

	// Strings for percents
	std::string sZeroPercent = "0%";
	std::string sOneHundredPercent = "100%";

	// Rects
	Rect rectMenu;
	Rect rectMemoryBar;

	olc::vf2d v2dMousePos = { 0.0f, 0.0f };
	float fBarLength;

	// Non-updating system info
	std::string sProcArch;
	DWORDLONG dwlPhysInstalledRam;

	// Testing this data struct
	VectorRect arrRectButtons;

	/**
	* Position of text centered in screen.
	*/
	olc::vf2d CenterTextPosistion(const std::string& s, uint32_t scale = 1, olc::vf2d offset = { 0.0f, 0.0f })
	{
		return olc::vf2d(
			((float)ScreenWidth() / 2.0f) - (float)(s.size() * 8 * scale) / 2.0f,
			((float)ScreenHeight() / 2.0f) - (float)(8.0f * scale)
		) + offset;
	}

	/**
	* Creating things here.
	*/
	bool OnUserCreate() override
	{
		swf = (float)ScreenWidth();
		shf = (float)ScreenHeight();

		// Menu button dimensions
		rectMenu = { 
			swf * 6.0f / 8.0f, 
			shf * 15.0f / 16.0f,
			(swf * 2.0f / 8.0f) - 5.0f,
			(shf * 1.0f / 16.0f) - 5.0f
		};

		// Memory bar dimensions
		fBarLength = swf * 15.0f / 16.0f;
		rectMemoryBar = {
			swf * 1.0f / 32.0f, 
			shf * 1.0f / 3.0f,
			fBarLength, 
			shf * 1.0f / 3.0f
		};

		// Add buttons to vector
		arrRectButtons.push_back(rectMenu);
		arrRectButtons.push_back(rectMemoryBar);

		// Set information that doesn't need to update
		SYSTEM_INFO sys = MM::SysInfo();
		sProcArch = MM::GetProcessorArchitectureStr(&sys);
		dwlPhysInstalledRam = MM::PhysInstalledRam();


		return true;
	}
	
	
	/**
	* Main program loop.
	*/
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Logic

		// Get mouse location
		v2dMousePos.x = (float)GetMouseX();
		v2dMousePos.y = (float)GetMouseY();

		// Check if mouse hovering over any buttons
		MM::CheckMouseInRect(v2dMousePos, arrRectButtons);


		// Memory information
		MEMORYSTATUSEX memInfo = MM::MemInfo();
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		float percMemUsed = (float)physMemUsed / (float)totalPhysMem * 100.0f;

		// CPU information
		SYSTEM_INFO sysInfo = MM::SysInfo();

		

		// Drawing
		Clear(olc::VERY_DARK_BLUE);

		// Update memory usage bar length
		arrRectButtons[1].w = fBarLength * percMemUsed / 100.0f;

		MM::DrawButtons(this, arrRectButtons);

		// Need to make these DrawString's part of draw function
		std::string sPerc = std::to_string((int)percMemUsed) + "%";
		DrawString(
			MM::CenterTextPosInRect(sPerc, arrRectButtons[1], 2U),
			sPerc,
			(arrRectButtons[1].bMouseHovering ? olc::BLACK : olc::WHITE),
			2U
		);

		std::string str = "MENUS";
		DrawString(
			MM::CenterTextPosInRect(str, arrRectButtons[0]),
			str,
			(arrRectButtons[0].bMouseHovering ? olc::BLACK : olc::WHITE)
		);


		return true;
	}

};


/**
* Main function.
*/
int main(void)
{
	int32_t width = 256;
	int32_t height = 240;
	int32_t pixel_width = 1;
	int32_t pixel_height = 1;
	MemoryManager mm;
	if (mm.Construct(width, height, pixel_width, pixel_height))
		mm.Start();

	return 0;
}