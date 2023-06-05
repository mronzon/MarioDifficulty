#pragma once
#ifndef ENEMY_BASE_H

#include <string>
#include <vector>
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <jsoncons/json.hpp>


using namespace jsoncons;


class Enemy_Base
{
public:
	Enemy_Base();
	~Enemy_Base();
	Enemy_Base(int x, int y, int height, int width, std::string type);
	virtual void Move(std::map<std::pair<int, int>, int>& nb_pos_enemy, std::vector<std::pair<int, int>> list_pixel)=0;
	virtual void GenerateImage(std::map<std::pair<int, int>, int> nb_pos_enemy, cv::Mat Image, int height, int width)=0;
	template <typename T> void get_Json(std::string type, std::vector<T>& liste_ennemy, json j);
	int get_x() const { return _x; };
	int get_y() const { return _y; };
	int get_height() const { return _height; };
	int get_width() const { return _width; };
	std::string get_type() const { return _type; };
	bool get_isWalkingLeft() const { return _isWalkingLeft; };
	void set_x(int x) { _x = x; };
	void set_y(int y) { _y = y; };
	void set_height(int height) { _height = height; };
	void set_width(int width) { _width = width; };
	void set_type(std::string type) { _type = type; };
	void set_isWalkingLeft(bool isWalkingLeft) { _isWalkingLeft = isWalkingLeft; };


private:
	int _x;
	int _y;
	int _height;
	int _width;
	std::string _type;
	bool _isWalkingLeft = true;
};

Enemy_Base::Enemy_Base()
{
	_x = 0;
	_y = 0;
	_height = 16;
	_width = 16;
	_type = "";
}

Enemy_Base::Enemy_Base(int x, int y, int height, int width, std::string type) {
	_x = x;
	_y = y;
	_height = height;
	_width = width;
	_type = type;
}


Enemy_Base::~Enemy_Base()
{
}

#endif

