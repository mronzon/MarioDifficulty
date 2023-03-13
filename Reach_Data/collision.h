#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <fstream>
#include <vector>
#include <string>
#include "jsoncons/json.hpp"
#include "jsoncons/basic_json.hpp"


struct collision_t {
	int x, y, h, w;
};

collision_t inline from_string(const std::string& s) {
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

bool inline comp(const collision_t& a, const collision_t& b) {
	return (a.y < b.y || (a.y == b.y && a.x < b.x));
}

std::vector<collision_t> inline from_file(const std::string& file_path) {
	std::vector<collision_t> result;
	std::ifstream collision_file(file_path);
	if (!collision_file) printf("A problem occured when trying to open file: %s.\n", file_path.c_str());
	else {
		jsoncons::json json_content = jsoncons::json::parse(collision_file);
		std::size_t n = json_content["static"]["positionCollision"].size();
		for(std::size_t i = 0; i < n ; i++)
		{
			result.push_back(collision_t{json_content["static"]["positionCollision"][i]["x"].as<int>(), json_content["static"]["positionCollision"][i]["y"].as<int>(), json_content["static"]["positionCollision"][i]["height"].as<int>(), json_content["static"]["positionCollision"][i]["width"].as<int>()});
		}
		collision_file.close();
	}
	return result;
}

bool inline get_dimension(std::pair<int, int>& dim,const std::string& json_path)
{
	std::ifstream file(json_path.c_str());

	if (!file.is_open()) {
		printf("File \"%s\" could not be opened for reading.\n", json_path.c_str());
		return false;
	}
	jsoncons::json json_content = jsoncons::json::parse(file);
	dim.first = json_content["levelRows"].as<int>();
	dim.second = json_content["levelCols"].as<int>();
	return true;
}

void inline modifyRectangleCorner(const int& x,const int& y, cv::Point& TopLeftCorner, cv::Point& BottomRightCorner)
{
	if(x < TopLeftCorner.x)
	{
		TopLeftCorner.x = x;
	} else if (x > BottomRightCorner.x)
	{
		BottomRightCorner.x = x;
	}
	if(y < TopLeftCorner.y)
	{
		TopLeftCorner.y = y;
	} else if (y > BottomRightCorner.y)
	{
		BottomRightCorner.y = y;
	}
}

#endif // !COLLISION_H