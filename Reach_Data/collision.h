#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <fstream>
#include <vector>
#include <string>

struct collision_t {
	int x, y, h, w;
};

collision_t from_string(const std::string& s) {
	int a = s.find(',', 0);
	int b = s.find(',', a + 1);
	int c = s.find(',', b + 1);

	collision_t result;
	result.x = std::stoi(s.substr(0, a));
	result.y = std::stoi(s.substr(a + 1, b - a - 1));
	result.h = std::stoi(s.substr(b + 1, c - b - 1));
	result.w = std::stoi(s.substr(c + 1));
	return result;
}

bool comp(const collision_t& a, const collision_t& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

std::vector<collision_t> from_file(const std::string& file_path) {
	std::vector<collision_t> result;
	std::ifstream collision_file(file_path);
	if (!collision_file) printf("A problem occured when trying to open file: %s.\n", file_path.c_str());
	else {
		std::string line;
		while (!collision_file.eof()) {
			std::getline(collision_file, line);
			if (line == "") break;
			result.push_back(from_string(line));
		}
		collision_file.close();
	}
	return result;
}

#endif // !COLLISION_H