#include "Osu.h"
#include "../Defines.h"
#include <thread>

void OsuLive::Start() {
	if (!MemInit()) throw 0x2f000000;
	while (true) {
		Update();
		std::this_thread::sleep_for(std::chrono::microseconds(500));
	}
}

void OsuLive::Update() {
	try {
		if (lastBeatmapHash == osu.GetCurrentBeatmapHash()) goto JustUpdate;
		lastBeatmapHash = osu.GetCurrentBeatmapHash();
		currentBeatmap = Beatmap(lastBeatmapHash);
JustUpdate:
		auto elasped = osu.GetElaspedTime();
		elaspedTime = elasped;
		lastReq = currentBeatmap.GetReqCursorAtSpecificTime(elasped);
		lastCS = currentBeatmap.CircleSize;
	}
	catch (int expn) {
		if (expn == 0x12000002) {
			lastBeatmapHash = "No Beatmap";
			currentBeatmap = Beatmap();
			lastReq = INVALID_COORDS;
			lastCS = 0.0;
		}
	}
}

Vector2 translateCoords(Vector2 c, double xs, double ys, double xo, double yo) {
	return Vector2{
		c.x * xs + xo, c.y * ys + yo
	};
}

Vector2 OsuLive::Translate2OsuCoords(Vector2 realCoords) {
	return translateCoords(realCoords, TOOSU_S, TOOSU_S, TOOSU_XO, TOOSU_YO);
}

Vector2 OsuLive::Translate2RealCoords(Vector2 osuCoords) {
	return translateCoords(osuCoords, TOREAL_S, TOREAL_S, TOREAL_XO, TOREAL_YO);
}

double OsuLive::Translate2OsuLen(double realLen) {
	return realLen * TOOSU_S;
}

double OsuLive::Translate2RealLen(double osuLen) {
	return osuLen * TOREAL_S;
}