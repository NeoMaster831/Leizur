#pragma once
#pragma warning( disable : 4267 4244 4101 )
// AimAssist.h: Yep, Aim Assist. It uses SetCursorPos to set position

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
	inline double dWorkCursorRadius = 64.0;

	// Refresh Rate in microseconds.
	inline long long dRefreshRate = 1'000'000 / 288;

	/*
	Rulesets.
	Users can edit this area.
	*/

	// Stop Circle user multiplier. The final expression is:
	// CurrentCircleRadius * StopCircleMultiplier * StopRadMul = FinalStopRad
	inline double uStopRadMul = 1.0;

	// Work Cursor user multiplier. The final expression is:
	// WorkCursorRadius * WorkRadMul = FinalWorkRad
	inline double uWorkRadMul = 1.25;

	inline double uSpeed = 3.1;

	inline bool uAAToggle = true;
	inline bool uAAToggleGdi = false;

	// For GUI
	inline float _uFStopRadMul = 1.0f;
	inline float _uFWorkRadMul = 1.25f;
	inline float _uFSpeed = 3.1f;

	void Routine();

	bool Check();
	void Move();
	void RenderGdi(Gdiplus::Graphics* pGraphics);
	void RenderGui();

};

