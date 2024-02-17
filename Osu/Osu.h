#pragma once
#pragma warning( disable : 4267 4244 4101 )
// Osu.h: About osu process itself, and osu hack.
// Errors
/*
 * - 0: couldn't initialize memory
 */

#include "../Process.h"
#include "../Defines.h"
#include "OsuUtils.h"
#define INVALID_COORDS Vector2{ -69697474.0f, -747476969.0f }

#define TOOSU_S 0.4444444
#define TOOSU_XO -170.6666
#define TOOSU_YO -56

#define TOREAL_S 2.25
#define TOREAL_XO 384
#define TOREAL_YO 126

const LPCWSTR OSU_PROCESS_NAME = L"osu!.exe";
const LPCWSTR OSU_RESOURCES_DLL = L"osu.Game.Resources.dll";

static double CS2Radius(double cs) {
	return 54.4 - 4.48 * cs;
}

class Osu
{

	// Overcalculated by 15.0
	std::vector<uint64_t> ElaspedTimeGlobal =
	{ 0x5EABD54, 0xB0, 0xD0, 0x4B8, 0x1F8, 0x8, 0x8, 0x38 };

	std::vector<uint64_t> IsActive =
	{ 0x5EABD54, 0x80, 0xD8, 0x310, 0x40, 0x18, 0x10 };

	std::vector<uint64_t> CvrtPosition =
	{ 0x5EABD54, 0x80, 0xD8, 0x388, 0x10, 0x388, 0x10, 0x318 };

	std::vector<uint64_t> CurrentBeatmapHash =
	{ 0x5EABD54, 0xB0, 0x40, 0xC0, 0xA08, 0x550, 0x358, 0x20, 0x8, 0x50, 0xC };

	Process osuProcess;

public:

	Osu() : osuProcess(OSU_PROCESS_NAME) {

	}

	double GetElaspedTime() {
		return osuProcess.GetPointerChainVal<double>(
			OSU_RESOURCES_DLL, ElaspedTimeGlobal) - 65.0;
	}

	bool GetActive() {
		return osuProcess.GetPointerChainVal<bool>(
			OSU_RESOURCES_DLL, IsActive);
	}

	Vector2 GetCvrtPosition() {
		try {
			return osuProcess.GetPointerChainVal<Vector2f>(
				OSU_RESOURCES_DLL, CvrtPosition).Translate();
		}
		catch (int expn) {
			if (expn == 0x4b000004) return INVALID_COORDS;
		}
		return INVALID_COORDS;
	}

	std::string GetCurrentBeatmapHash() {
		return osuProcess.GetPointerChainVal<SHA256Str>(
			OSU_RESOURCES_DLL, CurrentBeatmapHash).Convert();
	}

};

namespace OsuLive {

	inline Osu osu;
	inline Beatmap currentBeatmap = Beatmap();
	inline std::string lastBeatmapHash = "No Beatmap";
	inline double lastCS = 0.0;
	inline Vector2 lastReq = INVALID_COORDS;

	Vector2 Translate2OsuCoords(Vector2 realCoords);
	Vector2 Translate2RealCoords(Vector2 osuCoords);

	double Translate2OsuLen(double realLen);
	double Translate2RealLen(double osuLen);

	void Start();
	void Update();
}