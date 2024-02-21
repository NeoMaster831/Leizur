#include "Relax.h"
#include <thread>

void RelaxV1::Click(double delayMilli, int vKey) {

	INPUT input[2] = {};
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = MapVirtualKeyA(vKey, MAPVK_VK_TO_VSC);
	input[0].ki.dwFlags = KEYEVENTF_SCANCODE;

	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = 0;
	input[1].ki.wScan = MapVirtualKeyA(vKey, MAPVK_VK_TO_VSC);
	input[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

	long long inMicro = (long long)(delayMilli * 1'000);

	SendInput(1, input, sizeof(INPUT));
	std::this_thread::sleep_for(std::chrono::microseconds(inMicro));
	SendInput(1, input + 1, sizeof(INPUT));

}

void RelaxV1::Routine() {
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

		double elaspedTime = OsuLive::elaspedTime;
		HitObject currentObject = HitObject();
		try {
			currentObject = OsuLive::currentBeatmap.GetNextHitObject(elaspedTime);
		}
		catch (int expn) {
			continue;
		}
		auto currentTimingPoint = OsuLive::currentBeatmap.GetCurrentTimingPoint(elaspedTime);

		auto currentObjTime = currentObject.time;

		if (!lQ.empty() && lQ.top().first < elaspedTime) {
			if (!lAsync1.valid() ||
				lAsync1.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
				
				lAsync1 = std::async(std::launch::async, Click, lQ.top().second, dKey1);
			}
			else if (!lAsync2.valid() || 
				lAsync2.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
				
				lAsync2 = std::async(std::launch::async, Click, lQ.top().second, dKey2);
			}
			else { // We pass if no tasks available.
				lAsync1.get();
				continue;
			}
			while (!lQ.empty() && lQ.top().first < elaspedTime) {
				lQ.pop();
			}
		}

		if (abs(currentObjTime - lLastHitObjectTime) > 1e-4) { // It means last object is changed
			if (currentObject.type & (1 << 1)) { // Slider
				lQ.push({
					currentObjTime,
					currentObject.sliderParam.calculateDuration(OsuLive::currentBeatmap.SliderMultiplier,
						currentTimingPoint.beatLength > 0.0 ? 1.0 : abs(100.0 / currentTimingPoint.beatLength),
						currentTimingPoint.GetCurrentBeatLength()) });
			}
			else if (currentObject.type & (1 << 3)) { // Spinner
				lQ.push({
					currentObjTime,
					max(15.0, currentObject.spinnerParam.endTime - elaspedTime) });
			}
			else {
				lQ.push({
					currentObjTime,
					uClickDelay });
			}
		}

		lLastHitObjectTime = currentObjTime;
	}
}