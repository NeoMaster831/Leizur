#pragma once

// Osu.h: About osu process itself, and osu hack.

#include "../Process.h"
#include "../Defines.h"

const LPCWSTR OSU_PROCESS_NAME = L"osu!.exe";
const LPCWSTR OSU_RESOURCES_DLL = L"osu.Game.Resources.dll";

class Osu
{

	// Overcalculated by 15.0
	std::vector<uint64_t> ElaspedTimeGlobal =
	{ 0x5EABD54, 0xB0, 0xD0, 0x4B8, 0x1F8, 0x8, 0x8, 0x38 };

	std::vector<uint64_t> IsActive =
	{ 0x5EABD54, 0x80, 0xD8, 0x310, 0x40, 0x18, 0x10 };

	std::vector<uint64_t> CvrtPosition =
	{ 0x5EABD54, 0x80, 0xD8, 0x388, 0x10, 0x300, 0x10, 0x318 };

	std::vector<uint64_t> CurrentBeatmapHash =
	{ 0x5EABD54, 0xB0, 0x40, 0xC0, 0xA08, 0x550, 0x358, 0x20, 0x8, 0x50, 0xC };

	Process osuProcess;

public:

	Osu() : osuProcess(OSU_PROCESS_NAME) {
		
	}

	double GetElaspedTime() {
		return osuProcess.GetPointerChainVal<double>(
			OSU_RESOURCES_DLL, ElaspedTimeGlobal) - 15.0;
	}

	bool GetActive() {
		return osuProcess.GetPointerChainVal<bool>(
			OSU_RESOURCES_DLL, IsActive);
	}

	Vector2 GetCvrtPosition() {
		try {
			return osuProcess.GetPointerChainVal<Vector2>(
				OSU_RESOURCES_DLL, CvrtPosition);
		}
		catch (int expn) {
			if (expn == 0x4b000004) return Vector2{ 0.0f, 0.0f };
		}
	}

	std::string GetCurrentBeatmapHash() {
		return osuProcess.GetPointerChainVal<SHA256Str>(
			OSU_RESOURCES_DLL, CurrentBeatmapHash).Convert();
	}

};

