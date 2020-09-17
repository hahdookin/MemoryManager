#ifndef _MMDRAWINGROUTINES_H
#define _MMDRAWINGROUTINES_H

#include "MMSysInfo.h"

#include "olcPixelGameEngine.h"
#include "Utils.h"

#include <vector>


struct Rect
{
	float x, y, w, h;
	bool bMouseHovering = false;
	// std::string text;
	// olc::Pixel col;
};

typedef std::vector<Rect> VectorRect;

namespace MM
{
	bool CheckMouseInRect(const olc::vf2d& mouse, VectorRect& rects);
	void DrawButtons(olc::PixelGameEngine* pge, const VectorRect& rects);
	void DrawRect(olc::PixelGameEngine* pge, const Rect& r, olc::Pixel p = olc::WHITE);
	void FillRect(olc::PixelGameEngine* pge, const Rect& r, olc::Pixel p = olc::WHITE);
	olc::vf2d CenterTextPosInRect(const std::string& s, const olc::vf2d& start, const olc::vf2d& wh, uint32_t scale = 1U);
	olc::vf2d CenterTextPosInRect(const std::string& s, const Rect& r, uint32_t scale = 1U);
	bool InRect(const olc::vf2d& particle, const Rect& rect);
	bool InRect(const olc::vf2d& particle, const olc::vf2d& start, const olc::vf2d& sizeWH);
}

// Take in some sort of data set of buttons + flags
// Realistically, only one button can be highlighted at once
bool MM::CheckMouseInRect(const olc::vf2d& mouse, VectorRect& rects)
{
	for (Rect& rect : rects)
	{
		rect.bMouseHovering = false;
	}

	for (Rect& rect : rects)
	{
		if (MM::InRect(mouse, rect))
		{
			rect.bMouseHovering = true;
			return true;
		}
	}
	return false;
}

// Drawing routine after CheckMouseInRect()
// Probably take pointer to PGE and data set of flags
void MM::DrawButtons(olc::PixelGameEngine* pge, const VectorRect& rects)
{
	// For each flag in flags
	//		if flag is true
	//			Draw highlighted button
	//          ??? Profit
	for (const Rect& rect : rects)
	{
		bool hovering = rect.bMouseHovering;
		olc::Pixel border;
		if (hovering)
		{
			if (pge->GetMouse(0).bHeld)
			{
				MM::FillRect(pge, rect, olc::DARK_GREY);
			}
			else
			{
				MM::FillRect(pge, rect);
			}
		}
		MM::DrawRect(pge, rect, (hovering ? olc::DARK_GREY : olc::WHITE));
	}
}

// Overloading PGE's DrawRect to take Rect obj
void MM::DrawRect(olc::PixelGameEngine* pge, const Rect& r, olc::Pixel p)
{
	pge->DrawRect(r.x, r.y, r.w, r.h, p);
}

// Overloading PGE's FillRect to take Rect obj
void MM::FillRect(olc::PixelGameEngine* pge, const Rect& r, olc::Pixel p)
{
	pge->FillRect(r.x, r.y, r.w, r.h, p);
}

//Position of centered text within a rectangle.
olc::vf2d MM::CenterTextPosInRect(const std::string& s, const olc::vf2d& start, const olc::vf2d& wh, uint32_t scale)
{
	return olc::vf2d(
		0.5f * wh.x - (float)(s.size() * 8 * scale) / 2.0f,
		0.5f * wh.y - 3 * scale // should be 4 * scale but 3 looks better for some reason
	) + start;
}

// Overload of above function for rect struct
olc::vf2d MM::CenterTextPosInRect(const std::string& s, const Rect& r, uint32_t scale)
{
	olc::vf2d start = { r.x, r.y };
	return olc::vf2d(
		0.5f * r.w - (float)(s.size() * 8 * scale) / 2.0f,
		0.5f * r.h - 3 * scale // should be 4 * scale but 3 looks better for some reason
	) + start;
}

// Check if particle is in a rectangle
bool MM::InRect(const olc::vf2d& particle, const Rect& rect)
{
	using util::Between;
	float x2 = rect.x + rect.w;
	float y2 = rect.y + rect.h;
	return (Between(particle.x, rect.x, x2) && Between(particle.y, rect.y, y2));
}

// Assume p1 and p2 connect to make a rectangle, check if particle in rect p1 p2
bool MM::InRect(const olc::vf2d& particle, const olc::vf2d& start, const olc::vf2d& sizeWH)
{
	using util::Between;
	float x2 = start.x + sizeWH.x;
	float y2 = start.y + sizeWH.y;
	return (Between(particle.x, start.x, x2) && Between(particle.y, start.y, y2));
}

#endif // !_MMDRAWINGROUTINES_H

