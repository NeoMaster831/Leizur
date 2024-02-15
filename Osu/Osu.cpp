#include "Osu.h"
#include "../Defines.h"
#include <thread>

void OsuLive::Start() {
	if (!MemInit()) throw 0x2f000000;
	while (true) {
		Update();
		std::this_thread::sleep_for(std::chrono::microseconds(69));
	}
}

void OsuLive::Update() {
	try {
		if (lastBeatmapHash == osu.GetCurrentBeatmapHash()) return;
		lastBeatmapHash = osu.GetCurrentBeatmapHash();
		currentBeatmap = Beatmap(lastBeatmapHash);
	}
	catch (int expn) {
		if (expn == 0x4b000004 || expn == 0x12000002) {
			lastBeatmapHash = "No Beatmap";
			currentBeatmap = Beatmap();
		}
	}
}

Vector2 translateCoords(Vector2 c, double xs, double ys, double xo, double yo) {
	return Vector2{
		c.x * xs + xo, c.y * ys + yo
	};
}

Vector2 OsuLive::Translate2OsuCoords(Vector2 realCoords) {
	return translateCoords(realCoords, TOOSU_XS, TOOSU_YS, TOOSU_XO, TOOSU_YO);
}

Vector2 OsuLive::Translate2RealCoords(Vector2 osuCoords) {
	return translateCoords(osuCoords, TOREAL_XS, TOREAL_YS, TOREAL_XO, TOREAL_YO);
}