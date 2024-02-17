#pragma once
#pragma warning( disable : 4267 4244 4101 )
// AimAssist.h: Yep, Aim Assist. It uses

#include "../Osu/Osu.h"
#include <gdiplus.h>

/*
AimAssistV1 - Made by Wane.
Description:
	idk why I made this cheat, its absolute hell tho
*/
namespace AimAssistV1
{
	/*
	Live values.
	These can changed by program itself but not users.
	*/

	// in osu pixels
	inline Vector2 lLastFrameCursor = { 0, 0 };

	/*
	Defines.
	Users cannot modify this area, as it is the default values of settings.
	*/

	// do not modify cursor area multiplier. draws on hitObject.
	inline double dStopCircleMultiplier = 0.5;

	// modify cursor area. Draws on cursor.
	inline double dWorkCursorRadius = 128.0;

	// Rotate Ratio
	inline double dRotateRatio = 0.0075;

	// Refresh Rate in microseconds.
	inline long long dRefreshRate = 1'000'000 / 2880;

	/*
	Rulesets.
	Users can edit this area.
	*/

	// Stop Circle user multiplier. The final expression is:
	// CurrentCircleRadius * StopCircleMultiplier * StopRadMul = FinalStopRad
	inline double uStopRadMul = 0.5;

	// Work Cursor user multiplier. The final expression is:
	// WorkCursorRadius * WorkRadMul = FinalWorkRad
	inline double uWorkRadMul = 3.5;

	// Rotate Ratio
	inline double uRotateRatio = 1.0;

	void Routine();

	bool Check();
	void Move();
	void RenderGdi(Gdiplus::Graphics* pGraphics);
	void RenderGui();

};

