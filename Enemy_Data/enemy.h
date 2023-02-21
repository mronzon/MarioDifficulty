#pragma once

#include <string>


struct enemy {
	int x;
	int y;
	int height;
	int width;
	std::string type;
	bool isWalkingLeft = true;
};
