#include "AimAssist.h"
#include "../Defines.h"
#include "../GUI/Gdi.h"
#include <thread>

void GdiUtils::DrawCircle(Gdiplus::Graphics* pGraphics, Vector2 at, double radius, Color color);

bool AimAssistV1::Check() {
	try {
		auto reqPos = OsuLive::lastReq;
		auto lCurrentCircleRadius = CS2Radius(OsuLive::lastCS);
		auto finalStopRad = lCurrentCircleRadius * dStopCircleMultiplier * uStopRadMul;
		Vector2 vCurPos = OsuLive::osu.GetCvrtPosition();

		if (lengthPoints(reqPos, vCurPos) < finalStopRad) return false;

		auto finalWorkRad = dWorkCursorRadius * uWorkRadMul;
		if (lengthPoints(reqPos, vCurPos) > finalWorkRad) return false;

		return true;
	}
	catch (int expn) {
		return false;
	}
}

void AimAssistV1::Move() {
	try {
		auto vRealCurPos = GetRealPosition();
		auto reqPos = OsuLive::lastReq;
		auto realReqPos = OsuLive::Translate2RealCoords(reqPos);

		Vector2 BA = vectorBetweenPoints(lLastFrameCursor, vRealCurPos);
		Vector2 BC = vectorBetweenPoints(lLastFrameCursor, realReqPos);

		double angleABC = angleBetweenVectors(BA, BC);
		double finalMovingAmount = angleABC * dRotateRatio * uRotateRatio;

		if (getDirection(lLastFrameCursor, vRealCurPos, realReqPos)) {
			finalMovingAmount *= -1;
		}

		Vector2 cursorPoint = rotate(lLastFrameCursor, vRealCurPos, finalMovingAmount);
		int x = (int)cursorPoint.x;
		int y = (int)cursorPoint.y;
		if (x < 0 || x >= 1920 || y < 0 || y >= 1080) return;
		SetCursorPos((int)cursorPoint.x, (int)cursorPoint.y);
	}
	catch (int expn) { return; }
}

void AimAssistV1::RenderGdi(Gdiplus::Graphics* pGraphics) {
	
	try {
		if (!OsuLive::osu.GetActive()) return;
	}
	catch (int expn) {
		return;
	}

	try {
		Vector2 vCurRealPos = GetRealPosition();
		auto reqPos = OsuLive::lastReq;
		auto lCurrentCircleRadius = CS2Radius(OsuLive::lastCS);
		if (reqPos != INVALID_COORDS)
			GdiUtils::DrawCircle(pGraphics,
				OsuLive::Translate2RealCoords(reqPos),
				OsuLive::Translate2RealLen(lCurrentCircleRadius * dStopCircleMultiplier * uStopRadMul),
				Color(128, 255, 0, 0));
		GdiUtils::DrawCircle(pGraphics,
			vCurRealPos,
			OsuLive::Translate2RealLen(dWorkCursorRadius * uWorkRadMul),
			Color(128, 0, 255, 0));
	}
	catch (int _) { return; }
}

void AimAssistV1::RenderGui() {

}

void AimAssistV1::Routine() {

	while (true) {
		std::this_thread::sleep_for(std::chrono::microseconds(dRefreshRate));
		try {
			if (OsuLive::lastBeatmapHash == "No Beatmap"
				|| OsuLive::currentBeatmap.Title == "null"
				|| !OsuLive::osu.GetActive()) continue;
		}
		catch (int expn) {
			continue;
		}

		// Check
		
		if (Check())
			Move();
		else
			lLastFrameCursor = GetRealPosition();
	}
}