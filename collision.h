#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <string>

struct collision_t {
	int x, y, height, width;
};

std::string to_string(const collision_t& c) {
	return std::string(std::to_string(c.x) + "," + std::to_string(c.y) + "," + std::to_string(c.height) + "," + std::to_string(c.width));
}

collision_t from_string(const std::string& s) {
	int a = s.find(',', 0);
	int b = s.find(',', a);
	int c = s.find(',', b);

	collision_t result;
	result.x = std::stoi(s.substr(0, a));
	result.y = std::stoi(s.substr(a + 1, b - a - 1));
	result.height = std::stoi(s.substr(b + 1, c - b - 1));
	result.width = std::stoi(s.substr(c + 1));
	return result;
}

bool comp(const collision_t& a, const collision_t& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}


#endif // !COLLISION_H
#pragma once
