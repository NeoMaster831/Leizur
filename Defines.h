#pragma once
#include <string>

typedef struct _Vector2 {
	float x, y;
} Vector2;

typedef struct _SHA256Str {
	char Buffer[128];
public:
	std::string Convert() {
		std::string stream = "";
		for (int i = 0; i < 128; i += 2) stream += Buffer[i];
		return stream;
	}
} SHA256Str;