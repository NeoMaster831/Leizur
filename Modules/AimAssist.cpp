#include "AimAssist.h"
#include "../Defines.h"
#include "../GUI/Gdi.h"
#include <thread>

void GdiUtils::DrawCircle(Gdiplus::Graphics* pGraphics, Vector2 at, double radius, Color color);

bool AimAssistV1::Check() {
	try {
		Vector2 vCurRealPos = GetRealPosition();
		auto vCurPos = OsuLive::Translate2OsuCoords(vCurRealPos);
		auto reqPos = OsuLive::lastReq;

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
		auto vCurPos = OsuLive::Translate2OsuCoords(vRealCurPos);
		auto reqPos = OsuLive::lastReq;
		auto realReqPos = OsuLive::Translate2RealCoords(reqPos);

		auto lCurrentCircleRadius = CS2Radius(OsuLive::lastCS);
		auto finalStopRad = lCurrentCircleRadius * dStopCircleMultiplier * uStopRadMul;
		
		double deltaX = 0.0;
		double deltaY = 0.0;
		if (lengthPoints(reqPos, vCurPos) > finalStopRad
			&& lengthPoints(vRealCurPos, realReqPos) < lengthPoints(lLastFrameCursor, realReqPos)) {
			deltaX = (realReqPos.x - vRealCurPos.x) / 100.0;
			deltaY = (realReqPos.y - vRealCurPos.y) / 100.0;
			auto varyX = (vRealCurPos.x - lLastFrameCursor.x) / 100.0;
			auto varyY = (vRealCurPos.y - lLastFrameCursor.y) / 100.0;
			deltaX = (deltaX + varyX) * uSpeed;
			deltaY = (deltaY + varyY) * uSpeed;
		}
		else {
			deltaX = (lLastFrameCursor.x - vRealCurPos.x) / 10.0;
			deltaY = (lLastFrameCursor.y - vRealCurPos.y) / 10.0;
		}

		Vector2 cursorPoint = Vector2{
			vRealCurPos.x + deltaX,
			vRealCurPos.y + deltaY
		};

		int x = (int)cursorPoint.x;
		int y = (int)cursorPoint.y;
		if (x < 0 || x >= 1920 || y < 0 || y >= 1080) return;

		SetCursorPos(x, y);
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
		auto vCurPos = OsuLive::Translate2OsuCoords(vCurRealPos);
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
		lLastFrameCursor = GetRealPosition();
	}
}