#pragma once
#pragma warning( disable : 4267 4244 4101 )
// AimAssist.h: Yep, Aim Assist. It uses

#include "../Osu/Osu.h"
#include <gdiplus.h>

/*
AimAssistV1 - Made by Wane.
Description:
	Linear interpolation
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
	inline double dStopCircleMultiplier = 1.0;

	// modify cursor area. Draws on cursor.
	inline double dWorkCursorRadius = 128.0;

	// Refresh Rate in microseconds.
	inline long long dRefreshRate = 1'000'000 / 288;

	/*
	Rulesets.
	Users can edit this area.
	*/

	// Stop Circle user multiplier. The final expression is:
	// CurrentCircleRadius * StopCircleMultiplier * StopRadMul = FinalStopRad
	inline double uStopRadMul = 0.5;

	// Work Cursor user multiplier. The final expression is:
	// WorkCursorRadius * WorkRadMul = FinalWorkRad
	inline double uWorkRadMul = 0.6;

	inline double uSpeed = 3.0;

	void Routine();

	bool Check();
	void Move();
	void RenderGdi(Gdiplus::Graphics* pGraphics);
	void RenderGui();

};

