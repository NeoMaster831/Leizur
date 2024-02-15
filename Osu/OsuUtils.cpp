#include "OsuUtils.h"
#include <fstream>
#include <ranges>
#include <sstream>
#include <excpt.h>

enum class Section {
	NullSection, General, Editor, Metadata, Difficulty, Events, TimingPoints, HitObjects
};

std::vector<std::string> Split(const std::string& input, char delimiter) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) {
		result.push_back(temp);
	}

	return result;
}

bool Check(std::string& bufferString, std::string findString) {
	return bufferString.find(findString) != std::string::npos;
}

// NEVER TRUST USER INPUTS; WE KNOW CUZ WE HACK SOMETHING
bool CheckString(std::string& bufferString, char del, std::string findString) {
	auto pos = bufferString.find(del);
	if (pos == std::string::npos) return false;
	auto sbstr = bufferString.substr(0, pos);
	return sbstr == findString;
}

Section ParseSection(std::string& sectionName) {
	if (sectionName.find("General") != std::string::npos) return Section::General;
	if (sectionName.find("Editor") != std::string::npos) return Section::Editor;
	if (sectionName.find("Metadata") != std::string::npos) return Section::Metadata;
	if (sectionName.find("Difficulty") != std::string::npos) return Section::Difficulty;
	if (sectionName.find("Events") != std::string::npos) return Section::Events;
	if (sectionName.find("TimingPoints") != std::string::npos) return Section::TimingPoints;
	if (sectionName.find("HitObjects") != std::string::npos) return Section::HitObjects;
	return Section::NullSection;
}

int ParseInt(std::string& bufferString, std::string queryString) {
	return std::stoi(bufferString.substr(bufferString.find(queryString) + 1));
}

float ParseFloat(std::string& bufferString, std::string queryString) {
	return std::stof(bufferString.substr(bufferString.find(queryString) + 1));
}

double ParseDouble(std::string& bufferString, std::string queryString) {
	return std::stod(bufferString.substr(bufferString.find(queryString) + 1));
}

std::string ParseString(std::string& bufferString, std::string queryString) {
	return bufferString.substr(bufferString.find(queryString) + 1);
}

Beatmap::Beatmap(std::string& hash) {
	std::string stream;
	auto appDataPath = "C:\\Users\\user\\AppData\\Roaming"; // TODO: Change
	if (appDataPath == nullptr) throw 0x12000001;
	stream += appDataPath;
	stream += "\\osu\\files\\";
	stream += hash[0];
	stream += '\\';
	stream += hash[0];
	stream += hash[1];
	stream += '\\';
	stream += hash;
	if (!Parse(stream)) throw 0x12000002;
}

double TimingPoint::GetCurrentBeatLength() {
	TimingPoint* ptr = this;
	while (true) {
		if (ptr->beatLength > 0.0) break;
		ptr = ptr->inerhitPoint;
	}
	return ptr->beatLength;
}

void TimingPoint::Parse(std::string& str, TimingPoint* previousTimingPoint) {
	auto obj = Split(str, ',');
	this->time = std::stod(obj[0]);
	this->beatLength = std::stod(obj[1]);
	this->uninherited = std::stoi(obj[6]);
	if (!uninherited) this->inerhitPoint = previousTimingPoint;
}

std::vector<Vector2> simplify(std::vector<Vector2> &frames, int to) {
	size_t size = frames.size();
	if (size <= to) return frames;
	double multiplier = (double)size / (double)to;

	std::vector<Vector2> newVec;
	for (int i = 0; i <= to; i++) {
		int step = (int)((double)i * multiplier);
		if (step >= frames.size()) step = frames.size() - 1;
		if (step < 0) step = 0;
		newVec.push_back(frames[step]);
	}

	return newVec;
}

void Slider::ParseLinear() {
	if (this->curvePoints.size() > 2) {
		std::vector<Vector2> newCurvePoints;
		newCurvePoints.push_back(curvePoints[0]);
		for (int i = 1; i < curvePoints.size() - 1; i++) {
			newCurvePoints.push_back(curvePoints[i]);
			newCurvePoints.push_back(curvePoints[i]);
		}
		newCurvePoints.push_back(curvePoints[curvePoints.size() - 1]);
		this->curvePoints = newCurvePoints;
		ParseBezier();
		return;
	}
	auto linear = Linear(curvePoints[0], curvePoints[1]);
	auto extract = linear.Extract();

	if (this->length - extract.second < 0.0) {
		int cut = (int)((this->length / extract.second) * (double)frames);
		for (int i = 0; i < cut; i++) {
			this->pixelPoints.push_back(extract.first[i]);
		}
	}
	else {
		for (auto frame : extract.first) {
			this->pixelPoints.push_back(frame);
		}
	}
}

std::vector<Vector2> syncronizeFrames(std::vector<Vector2>& rawFrames, double length, double standard = 100.0) {
	double vi = standard / length;
	std::vector<Vector2> ret;
	for (double i = 0.0; i < rawFrames.size(); i += vi) {
		size_t szi = (size_t)i;
		ret.push_back(rawFrames[szi]);
	}
	return ret;
}

void Slider::ParseBezier() {

	auto totalLength = this->length;
	int i = 1;
	int size = this->curvePoints.size();
	auto prev = this->curvePoints[0];
	auto q = curvePoints;

	while (i < size) {
		auto present = q[i];
		if (prev == present) { // then it's end of bezier, also start of bezier

			std::vector<Vector2> v;
			for (int j = 0; j < i; j++) {
				v.push_back(q[j]);
			}

			if (v.size() >= 2) {
				auto bezier = Bezier(v);
				auto res = bezier.Extract();
				auto synced = syncronizeFrames(res.first, res.second);

				if (totalLength - res.second < 0.0) {
					int cut = (int)((totalLength / res.second) * (double)synced.size());
					for (int i = 0; i < cut; i++) {
						this->pixelPoints.push_back(synced[i]);
					}
					return;
				}

				for (auto frame : synced) {
					this->pixelPoints.push_back(frame);
				}
				totalLength -= res.second;
			}

			std::vector<Vector2> newq;
			for (int j = i; j < q.size(); j++) {
				newq.push_back(q[j]);
			}

			i = 0;
			q = newq;
			size = q.size();

		}
		prev = present;
		i++;
	}
	if (q.size() <= 1) return;
	auto lastBezier = Bezier(q);
	auto lastRes = lastBezier.Extract();
	auto synced = syncronizeFrames(lastRes.first, lastRes.second);

	if (totalLength - lastRes.second < 0.0) {
		int cut = (int)((totalLength / lastRes.second) * (double)synced.size());
		for (int i = 0; i < cut; i++) {
			this->pixelPoints.push_back(synced[i]);
		}
	}
	else {
		for (auto frame : synced) {
			this->pixelPoints.push_back(frame);
		}
	}
}

void Slider::ParsePerfect() {
	if (this->curvePoints.size() < 2) throw 0x12000006;
	else if (this->curvePoints.size() == 2) {
		ParseLinear();
		return;
	}
	else if (this->curvePoints.size() > 3) {
		ParseBezier();
		return;
	}
	this->pixelPoints
		= Perfect(curvePoints[0], curvePoints[1], curvePoints[2], this->length).Extract();
}

Vector2 Slider::at(double progress) {
	auto step = 1.0 / (double)pixelPoints.size();
	auto idx = (size_t)(progress / step);
	if (idx >= pixelPoints.size()) idx = pixelPoints.size() - 1;
	if (idx < 0) idx = 0;
	return this->pixelPoints[idx];
}

double Slider::calculateDuration(double sm, double velocity, double beatLength) {
	auto pixelPerBeat = sm * 100 * velocity;
	auto beats = this->length * this->slides / pixelPerBeat;
	return std::ceil(beats * beatLength);
}

Vector2 Slider::calculateDuringPos(double time, double sm, double velocity, double beatLength) {
	double duration = calculateDuration(sm, velocity, beatLength);
	if (time > duration) time = duration;
	if (time < 0.0) time = 0.0;
	return at(time / duration);
}

void Slider::Parse(std::string& str, Vector2 startPoint) {
	auto obj = Split(str, ',');
	auto curves = Split(obj[5], '|');
	this->curveType = curves[0].c_str()[0];
	
	if (curveType == 'C') throw 0x12000003;

	this->curvePoints.push_back(startPoint);
	for (int i = 1; i < curves.size(); i++) {
		auto curvePoint = Split(curves[i], ':');
		this->curvePoints.push_back(Vector2{ std::stof(curvePoint[0]), std::stof(curvePoint[1]) });
	}

	this->slides = std::stoi(obj[6]);
	this->length = std::stod(obj[7]);

	if (curveType == 'B') ParseBezier();
	else if (curveType == 'P') ParsePerfect();
	else if (curveType == 'L') ParseLinear();
	this->pixelPoints = simplify(this->pixelPoints, frames / slides + 1);

	auto rev = this->pixelPoints;
	std::reverse(rev.begin(), rev.end());
	auto ver = this->pixelPoints;
	this->pixelPoints.clear();

	for (int i = 0; i < slides; i++) {
		if (i % 2 == 0) for (auto obj : ver) pixelPoints.push_back(obj);
		else for (auto obj : rev) pixelPoints.push_back(obj);
	}

	this->pixelPoints = simplify(this->pixelPoints, frames);

}

void HitObject::Parse(std::string& str) {
	auto obj = Split(str, ',');
	this->c = Vector2{ std::stof(obj[0]), std::stof(obj[1]) };
	this->time = std::stod(obj[2]);
	this->type = std::stoi(obj[3]);

	if (this->type & (1 << 1)) this->sliderParam = Slider(str, c);
	else if (this->type & (1 << 3)) this->spinnerParam = Spinner(str);
}

void Spinner::Parse(std::string& str) {
	auto obj = Split(str, ',');
	this->endTime = std::stod(obj[5]);
}

bool timingPointCmp(const TimingPoint& tp, double time) {
	return tp.time < time;
}

bool hitObjectCmp(const HitObject& ho, double time) {
	return ho.time < time;
}

TimingPoint Beatmap::GetCurrentTimingPoint(double elaspedTime) {
	auto it = std::lower_bound(
		this->timingPoints.begin(),
		this->timingPoints.end(),
		elaspedTime,
		timingPointCmp
	);
	if (it != this->timingPoints.begin() && it != this->timingPoints.end()) {
		return *std::prev(it);
	}
	else if (it == this->timingPoints.begin()) {
		throw 0x12000007;
	}
	else {
		return *std::prev(timingPoints.end());
	}
	
}

HitObject Beatmap::GetNextHitObject(double elaspedTime) {
	auto it = std::lower_bound(
		this->hitObjects.begin(),
		this->hitObjects.end(),
		elaspedTime,
		hitObjectCmp
	);

	auto currentTiming = GetCurrentTimingPoint(elaspedTime);
	auto bl = currentTiming.beatLength > 0.0 ? 1.0 : abs(100.0 / currentTiming.beatLength);

	if (it != this->hitObjects.begin()) {
		auto prev = std::prev(it);
		if (prev->type & (1 << 1)) { // Slider
			if (prev->sliderParam.calculateDuration(this->SliderMultiplier,
				bl,
				currentTiming.GetCurrentBeatLength()) + prev->time > elaspedTime)
				return *prev;
			else if (it == this->hitObjects.end()) throw 0x12000008;
			else return *it;
		}
		else if (prev->type & (1 << 3)) { // Spinner
			if (prev->spinnerParam.endTime > elaspedTime) return *prev;
			else if (it == this->hitObjects.end()) throw 0x12000008;
			else return *it;
		}
		else if (it == this->hitObjects.end()) throw 0x12000008;
		else return *it;
	}
	else {
		return *it;
	}
}

Vector2 Beatmap::GetReqCursorAtSpecificTime(double time) {
	auto hitObject = GetNextHitObject(time);
	auto timingPoint = GetCurrentTimingPoint(time);
	auto bl = timingPoint.beatLength > 0.0 ? 1.0 : abs(100.0 / timingPoint.beatLength);

	if (hitObject.type & (1 << 1)) {
		auto slider = hitObject.sliderParam;
		return slider.calculateDuringPos(time - hitObject.time,
			this->SliderMultiplier,
			bl,
			timingPoint.GetCurrentBeatLength());
	}
	else if (hitObject.type & (1 << 3)) {
		throw 0x12000009;
	}
	else {
		return hitObject.c;
	}
}

bool Beatmap::Parse(std::string& path) {
	std::ifstream beatmap(path);
	std::string buffer;
	Section currentSection = Section::NullSection;
	TimingPoint* previous = nullptr;

	if (!beatmap.is_open()) return false;

	while (std::getline(beatmap, buffer)) {

		if (BeatmapVersion != -1 && !IsSupported()) return false;

		if (buffer.size() == 0) continue; // fuck off null
		if (buffer[0] == '[' && buffer[buffer.size() - 1] == ']') {
			currentSection = ParseSection(buffer);
			continue;
		}
		if (currentSection == Section::NullSection) {
			if (Check(buffer, "osu file format v")) 
				this->BeatmapVersion = ParseInt(buffer, "v");
		}
		else if (currentSection == Section::General) {
			if (Check(buffer, "StackLeniency"))
				this->StackLeniency = ParseDouble(buffer, ":");
		}
		else if (currentSection == Section::Metadata) {
			if (CheckString(buffer, ':', "Title"))
				this->Title = ParseString(buffer, ":");
			if (CheckString(buffer, ':', "Artist"))
				this->Artist = ParseString(buffer, ":");
			if (CheckString(buffer, ':', "Version"))
				this->Version = ParseString(buffer, ":");
		}
		else if (currentSection == Section::Difficulty) {
			if (Check(buffer, "HPDrainRate"))
				this->HPDrainRate = ParseFloat(buffer, ":");
			if (Check(buffer, "CircleSize"))
				this->CircleSize = ParseFloat(buffer, ":");
			if (Check(buffer, "OverallDifficulty"))
				this->OverallDifficulty = ParseFloat(buffer, ":");
			if (Check(buffer, "ApproachRate"))
				this->ApproachRate = ParseFloat(buffer, ":");
			if (Check(buffer, "SliderMultiplier"))
				this->SliderMultiplier = ParseDouble(buffer, ":");
			if (Check(buffer, "SliderTickRate"))
				this->SliderTickRate = ParseDouble(buffer, ":");
		}
		else if (currentSection == Section::TimingPoints) {
			auto newTimingPoint = new TimingPoint(buffer, previous);
			this->timingPoints.push_back(*newTimingPoint);
			previous = newTimingPoint;
		}
		else if (currentSection == Section::HitObjects) {
			try {
				this->hitObjects.push_back(HitObject(buffer));
			}
			catch (int expn) {
				if (expn == 0x12000004 || expn == 0x12000003) return false;
			}
		}
	}

	return true;
}