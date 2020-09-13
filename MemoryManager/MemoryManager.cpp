

//***************************
// Fix for min/max macro conflict with algorithm library
// Needs to be included first, then game engine (PGE uses std::min/max)
#define NOMINMAX
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include "windows.h"
// Thank you, https://stackoverflow.com/questions/4913922/possible-problems-with-nominmax-on-visual-c
//***************************
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "MMSysInfo.h"
#include "Utils.h"

#include <iostream>

constexpr float one16th = 1.0f / 16.0f;
constexpr float two16th = 2.0f / 16.0f;
constexpr float three16th = 3.0f / 16.0f;
constexpr float four16th = 4.0f / 16.0f;
constexpr float five16th = 5.0f / 16.0f;
constexpr float six16th = 6.0f / 16.0f;
constexpr float seven16th = 7.0f / 16.0f;
constexpr float eight16th = 8.0f / 16.0f;
constexpr float nine16th = 9.0f / 16.0f;
constexpr float ten16th = 10.0f / 16.0f;
constexpr float eleven16th = 11.0f / 16.0f;
constexpr float twelve16th = 12.0f / 16.0f;
constexpr float thirteen16th = 13.0f / 16.0f;
constexpr float fourteen16th = 14.0f / 16.0f;
constexpr float fifthteen16th = 15.0f / 16.0f;

constexpr float ratio = 1024.0f * 1024.0f * 1024.0f; // # of bytes in gigabyte

// Might use this struct if need be
struct Rect
{
	float x, y, w, h;
};

// Assume p1 and p2 connect to make a rectangle, check if particle in rect p1 p2
bool InRect(const olc::vf2d& particle, const olc::vf2d& start, const olc::vf2d& sizeWH)
{
	using util::Between;
	float x2 = start.x + sizeWH.x;
	float y2 = start.y + sizeWH.y;
	return (Between(particle.x, start.x, x2) && Between(particle.y, start.y, y2));
}

class MemoryManager : public olc::PixelGameEngine
{
public:
	MemoryManager()
	{
		sAppName = "Memory Manager";
	}

public:
	bool OnUserCreate() override
	{
		swf = (float)ScreenWidth();
		shf = (float)ScreenHeight();

		// Menu button dimensions
		v2dOptionStart = { swf * 6.0f / 8.0f, shf * 15.0f / 16.0f };
		v2dOptionWH = { swf * 2.0f / 8.0f, shf * 1.0f / 16.0f };
		v2dOptionWH.x -= 5.0f; v2dOptionWH.y -= 5.0f;

		fBarLength = swf * 2.0f / 3.0f;
		v2dMemoryBarStart = { swf * 1.0f / 32.0f, shf * 1.0f / 3.0f };
		v2dMemoryBarWH = { fBarLength, shf * 1.0f / 3.0f };


		return true;
	}
	// Screen Height and Width
	float swf; // ScreenWidth as float
	float shf; // ScreenHeight as float

	// Strings for percents
	std::string sZeroPercent = "0%";
	std::string sOneHundredPercent = "100%";

	// Points
	olc::vf2d v2dOptionStart;
	olc::vf2d v2dOptionWH;
	olc::vf2d v2dMousePos = { 0.0f, 0.0f };
	olc::vf2d v2dMemoryBarStart;
	olc::vf2d v2dMemoryBarWH;
	float fBarLength;

	// Flags
	bool bHoveringOverOptions;

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
	* Position of centered text within a rectangle.
	*/
	olc::vf2d CenterTextPosInRect(const std::string& s, const olc::vf2d& start, const olc::vf2d& wh, uint32_t scale = 1)
	{
		return olc::vf2d(
			0.5f * wh.x - (s.size() * 8 * scale) / 2.0f,
			0.5f * wh.y - 3 * scale // should be 4 * scale but 3 looks better for some reason
		) + start;
	}

	/**
	* Main program loop.
	*/
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Logic

		// Check mouse location
		bHoveringOverOptions = false;
		v2dMousePos.x = (float)GetMouseX();
		v2dMousePos.y = (float)GetMouseY();
		if (InRect(v2dMousePos, v2dOptionStart, v2dOptionWH))
		{
			bHoveringOverOptions = true;
		}

		// Memory information
		MEMORYSTATUSEX memInfo = MM::MemInfo();
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		float percMemUsed = (float)physMemUsed / (float)totalPhysMem * 100.0f;

		// CPU information
		SYSTEM_INFO sysInfo = MM::SysInfo();



		// Drawing
		Clear(olc::VERY_DARK_BLUE);

		// Memory Usage Bar
		std::string sPerc = std::to_string((int)percMemUsed);
		v2dMemoryBarWH.x = fBarLength * percMemUsed / 100.0f;
		DrawRect(v2dMemoryBarStart, v2dMemoryBarWH);
		DrawString(
			CenterTextPosInRect(sPerc, v2dMemoryBarStart, v2dMemoryBarWH, 2U),
			sPerc,
			olc::WHITE,
			2U
		);


		if (bHoveringOverOptions)
		{
			if (GetMouse(0).bHeld)
			{
				FillRect(v2dOptionStart, v2dOptionWH, olc::DARK_GREY);
			}
			else
			{
				FillRect(v2dOptionStart, v2dOptionWH);
			}
		}
		DrawRect(v2dOptionStart, v2dOptionWH, (bHoveringOverOptions ? olc::DARK_GREY : olc::WHITE));


		std::string str = "MENUS";
		DrawString(
			CenterTextPosInRect(str, v2dOptionStart, v2dOptionWH),
			str,
			(bHoveringOverOptions ? olc::BLACK : olc::GREY)
		);


		return true;
	}

};


/**
* Main function.
*/
int main(void)
{
	MemoryManager mm;
	if (mm.Construct(256, 240, 1, 1))
		mm.Start();

	return 0;
}