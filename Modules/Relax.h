#pragma once
#pragma warning( disable : 4267 4244 4101 )
// Relax.h: Auto clicker. It clicks manually.

#include "../Osu/Osu.h"
#include <vector>
#include <future>
#include <queue>

/*
RelaxV1 - Made by Wane.
Description:
	Relax
*/

#define pdd std::pair<double, double>

namespace RelaxV1 {

	inline double lLastHitObjectTime = -100.0;
	inline std::priority_queue<pdd, std::vector<pdd>, std::greater<pdd> > lQ;

	inline std::future<void> lAsync1;
	inline std::future<void> lAsync2;

	inline int dKey1 = 0x44; // 'D'
	inline int dKey2 = 0x53; // 'S'

	inline long long dRefreshRate = 1'000'000 / 1'440; // Refrest rate in microseconds

	inline double uClickDelay = 35.0;
	inline double uRandomizer = 17.5;
	inline double uMaxAllow = 50.0;

	inline bool uRelaxToggle = true;

	// We can implement it by manual tapping, like SendInput.

	// Don't need to modify
	inline float _uFClickDelay = 35.0f;
	inline float _uFRandomizer = 17.5f;
	inline float _uFMaxAllow = 50.0f;

	void Routine();

	void Click(double delayMilli, int vKey);

	void RenderGui();
}