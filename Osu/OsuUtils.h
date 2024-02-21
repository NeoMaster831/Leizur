#pragma once
#pragma warning( disable : 4267 4244 )
// OsuParser.h: Parse whole shit into beatmap.
// This Includes some utility stuffs.
/*
 * Errors:
 * - 1: couldn't parse beatmap; couldn't find appdata
 * - 2: couldn't parse beatmap; hash thingy, pull req plz
 * - 3: couldn't parse beatmap; catmull point found, it is not supported atm
 * - 4: bezier curve is strange
 * - 5: linear curve is strange
 * - 6: perfect curve is strange
 * - 7: abort; no timing point in the section
 * - 8: abort; no next hit object
 * - 9: abort; its spinner.
 * - a: abort; couldn't get beat length, idk why lol
 */

#include <vector>
#include <string>
#include <set>
#include "../Defines.h"

#define TGFRAME 240

const std::set<int> SUPPORTED_VERSIONS = {
	7, 8, 9, 10, 11, 12, 13, 14
};

struct Spinner {
	double endTime = -1.0;

	Spinner() {}
	Spinner(std::string& str) { Parse(str); }
	void Parse(std::string& str);
};

struct Slider {
private:

	int frames = TGFRAME; // 240hz, (steps).

	void ParseBezier();
	void ParseLinear();
	void ParsePerfect();

	Vector2 at(double progress);

public:
	char curveType = 'U';
	std::vector<Vector2> pixelPoints;
	std::vector<Vector2> curvePoints;
	int slides = 1;
	double length = 1.0;

	Slider() {}
	Slider(std::string& str, Vector2 startPoint) { Parse(str, startPoint); }
	void Parse(std::string& str, Vector2 startPoint);
	double calculateDuration(double sm, double velocity, double beatLength);
	Vector2 calculateDuringPos(double time, double sm, double velocity, double beatLength);
};

struct TimingPoint {

	TimingPoint* inerhitPoint = nullptr;

public:
	double time;
	double beatLength;
	bool uninherited;

	TimingPoint();
	TimingPoint(std::string& str, TimingPoint* previousTimingPoint) { Parse(str, previousTimingPoint); }
	double GetCurrentBeatLength();
	void Parse(std::string& str, TimingPoint* previousTimingPoint);
};

struct HitObject {
	Vector2 c;
	double time;
	int type;
	Slider sliderParam = Slider();
	Spinner spinnerParam = Spinner();

	HitObject();
	HitObject(std::string& str) { Parse(str); }
	void Parse(std::string& str);
};

class Beatmap {

	std::vector<TimingPoint> timingPoints;
	std::vector<HitObject> hitObjects;

public:
	int BeatmapVersion = -1;

	// General
	double StackLeniency = 0.7;

	// Metadata
	std::string Title = "null";
	std::string Artist = "null";
	std::string Version = "null";

	// Difficulty
	float HPDrainRate = 5.0f;
	float CircleSize = 5.0f;
	float OverallDifficulty = 5.0f;
	float ApproachRate = 5.0f;
	
	double SliderMultiplier = 1.4;
	double SliderTickRate = 1.0;

	TimingPoint GetCurrentTimingPoint(double elaspedTime);
	HitObject GetNextHitObject(double elaspedTime);

	Beatmap() {}
	Beatmap(std::string& hash);

	Vector2 GetReqCursorAtSpecificTime(double elaspedTime);

	bool IsSupported() {
		return SUPPORTED_VERSIONS.find(BeatmapVersion) != SUPPORTED_VERSIONS.end();
	}
	
	bool Parse(std::string& path);
};