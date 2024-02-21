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

	// We can implement it by manual tapping, like SendInput.

	void Routine();

	void Click(double delayMilli, int vKey);

}