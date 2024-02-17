#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#pragma warning( disable : 4267 4244 )

#define TGFRAME_CURVES 240

struct Vector2 {
	double x, y;

	bool operator==(const Vector2& other) const {
		return abs(x - other.x) < 1e-5 && abs(y - other.y) < 1e-5;
	}
};

struct Vector2f {
	float x, y;
	Vector2 Translate() {
		return Vector2{ (double)x, (double)y };
	}
};

typedef struct _SHA256Str {
	char Buffer[128];
public:
	std::string Convert() {
		std::string stream = "";
		for (int i = 0; i < 128; i += 2) stream += Buffer[i];
		return stream;
	}
} SHA256Str;

static int binomialCoeff(int n, int k) {
	int res = 1;
	if (k > n - k) {
		k = n - k;
	}
	for (int i = 0; i < k; ++i) {
		res *= (n - i);
		res /= (i + 1);
	}
	return res;
}

static double lengthPoints(Vector2 from, Vector2 to) {
	return sqrt(pow(from.x - to.x, 2) + pow(from.y - to.y, 2));
}

static Vector2 rotate(Vector2 center, Vector2 target, double radians) {
	auto c = cos(radians);
	auto s = sin(radians);

	return Vector2{
		(float)((c * (target.x - center.x)) - (s * (target.y - center.y)) + center.x),
		(float)((s * (target.x - center.x)) + (c * (target.y - center.y)) + center.y)
	};
}

static Vector2 GetRealPosition() {
	POINT pt;
	GetCursorPos(&pt);
	auto ret = Vector2{ (double)pt.x, (double)pt.y };
	return ret;
}

static Vector2 vectorBetweenPoints(Vector2 a, Vector2 b) {
	return Vector2{ b.x - a.x, b.y - a.y };
}

static double vectorMagnitude(Vector2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

static double dotProduct(Vector2 a, Vector2 b) {
	return a.x * b.x + a.y * b.y;
}

static double angleBetweenVectors(Vector2 a, Vector2 b) {
	double dot = dotProduct(a, b);
	double mag1 = vectorMagnitude(a);
	double mag2 = vectorMagnitude(b);
	double cosTheta = dot / (mag1 * mag2);
	return acos(cosTheta);
}

static bool getDirection(Vector2 a, Vector2 b, Vector2 c) {
	return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) < 0;
}

class Linear {
	Vector2 from, to;
	int frames = TGFRAME_CURVES;
public:
	Linear(Vector2 from, Vector2 to) {
		this->from = from; this->to = to;
	}
	std::pair<std::vector<Vector2>, double> Extract() {
		std::vector<Vector2> extracted;
		for (int i = 0; i <= frames; i++) {
			extracted.push_back({
				from.x + (to.x - from.x) * (float)i / frames,
				from.y + (to.y - from.y) * (float)i / frames
				});
		}
		double len = lengthPoints(from, to);
		return { extracted, len };
	}
};

class Bezier {
	std::vector<Vector2> curves;
	int frames = TGFRAME_CURVES;

public:
	Bezier(std::vector<Vector2>& curves) {
		for (auto curve : curves) this->curves.push_back(curve);
	}

	Vector2 calculateBezierPoint(double t) {
		Vector2 B;
		B.x = 0; B.y = 0;
		size_t n = curves.size() - 1;
		for (int i = 0; i <= n; i++) {
			double binCoeff = binomialCoeff(n, i);
			double term = binCoeff * pow(1 - t, n - i) * pow(t, i);
			B.x += term * curves[i].x;
			B.y += term * curves[i].y;
		}
		return B;
	}

	std::pair<std::vector<Vector2>, double> Extract() {
		if (curves.size() == 1) 
			throw 0x12000004;
		else if (curves.size() == 2) return Linear(curves[0], curves[1]).Extract();
		std::vector<Vector2> extracted;
		Vector2 prev = this->curves[0];
		double len = 0.0;

		for (int i = 0; i <= frames; i++) {

			double t = (double)i / (double)frames;

			auto present = calculateBezierPoint(t);
			len += lengthPoints(prev, present);
			prev = present;

			extracted.push_back(calculateBezierPoint(t));
		}
		return { extracted, len };
	}
};

class Perfect {
	Vector2 p1, p2, p3;
	int frames = TGFRAME_CURVES;
	double length;

	std::pair<Vector2, double> getCircumCircle() {
		
		auto distance = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
		
		auto ux = ((p1.x * p1.x + p1.y * p1.y) * (p2.y - p3.y) +
			(p2.x * p2.x + p2.y * p2.y) * (p3.y - p1.y) +
			(p3.x * p3.x + p3.y * p3.y) * (p1.y - p2.y)) / distance;

		auto uy = ((p1.x * p1.x + p1.y * p1.y) * (p3.x - p2.x) +
			(p2.x * p2.x + p2.y * p2.y) * (p1.x - p3.x) +
			(p3.x * p3.x + p3.y * p3.y) * (p2.x - p1.x)) / distance;

		auto px = ux - p1.x;
		auto py = uy - p1.y;
		auto radius = sqrt(px * px + py * py);

		return { Vector2{ ux, uy }, radius };

	}

public:

	Perfect(Vector2 p1, Vector2 p2, Vector2 p3, double length) {
		this->p1 = p1; this->p2 = p2; this->p3 = p3;
		this->length = length;
	}

	std::vector<Vector2> Extract() {
		auto circle = getCircumCircle();
		Vector2 mid = circle.first;
		double radius = circle.second;

		auto radians = this->length / radius;
		if (getDirection(p1, p2, p3)) radians *= -1;

		auto step = radians / (double)frames;
		std::vector<Vector2> extracted;
		for (int i = 0; i < frames; i++) {
			extracted.push_back(rotate(mid, p1, step * (double)i));
		}
		return extracted;
	}
};