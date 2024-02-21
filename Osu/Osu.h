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
const LPCWSTR OSU_FRAMEWORK_DLL = L"osu.Framework.dll";

static double CS2Radius(double cs) {
	return 54.4 - 4.48 * cs;
}

class Osu
{

	// Overcalculated by 15.0
	std::vector<uint64_t> ElaspedTimeGlobal =
	{ 0x2024, 0xd0, 0xa0, 0x10, 0x4a8, 0x1e0, 0x8, 0x10, 0x20 };

	std::vector<uint64_t> IsActive =
	{ 0x2024, 0xd0, 0x4f8, 0x320, 0x588, 0x208, 0x40, 0x18, 0x10 };

	std::vector<uint64_t> CurrentBeatmapHash =
	{ 0x2024, 0xd0, 0x580, 0xC8, 0x220, 0x430, 0x20, 0x8, 0x50, 0xC };

	Process osuProcess;

public:

	Osu() : osuProcess(OSU_PROCESS_NAME) {

	}

	double GetElaspedTime() {
		return osuProcess.GetPointerChainVal<double>(
			OSU_FRAMEWORK_DLL, ElaspedTimeGlobal) - 30.0;
	}

	bool GetActive() {
		return osuProcess.GetPointerChainVal<bool>(
			OSU_FRAMEWORK_DLL, IsActive);
	}

	std::string GetCurrentBeatmapHash() {
		return osuProcess.GetPointerChainVal<SHA256Str>(
			OSU_FRAMEWORK_DLL, CurrentBeatmapHash).Convert();
	}

};

namespace OsuLive {

	inline Osu osu;
	inline Beatmap currentBeatmap = Beatmap();
	inline std::string lastBeatmapHash = "No Beatmap";
	inline double lastCS = 0.0;
	inline Vector2 lastReq = INVALID_COORDS;
	inline double elaspedTime = 0.0;

	Vector2 Translate2OsuCoords(Vector2 realCoords);
	Vector2 Translate2RealCoords(Vector2 osuCoords);

	double Translate2OsuLen(double realLen);
	double Translate2RealLen(double osuLen);

	void Start();
	void Update();
}