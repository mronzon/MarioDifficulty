#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "general.h"

#include <filesystem>
#include <string>
#include <fstream>

#ifndef NDEBUG
#define dbg_write_img(rows, cols, collisions, file_path)\
	write_collision_img((rows), (cols), (collisions), (file_path))
#else
#define dbg_write_img(rows, cols, collisions, file_path) (void(0))
#endif // !NDEBUG



struct collision_t {
	int32_t x, y, h, w;
};

void write_collision_cache(
	const std::vector<collision_t>& collisions,
	const std::string& file_path)
{
	std::ofstream file(file_path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	if (!file.is_open()) {
		dbg_printf("File \"%s\" could not be oppened for writing.\n", file_path.c_str());
		return;
	}

	file.write(reinterpret_cast<const char*>(collisions.data()), sizeof(collision_t) * collisions.size());
	file.close();
}

bool read_collision_cache(
	std::vector<collision_t>& collisions,
	const std::string& file_path)
{
	std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		dbg_printf("File \"%s\" could not be oppened for reading.\n", file_path.c_str());
		return false;
	}

	file.seekg(0, std::ios::end);
	size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	collisions.resize(file_size / sizeof(collision_t));
	file.read(reinterpret_cast<char*>(collisions.data()), file_size);
	return true;
}

bool collision_compare(const collision_t& a, const collision_t& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

void write_collision_img(
	int rows,
	int cols,
	const std::vector<collision_t>& collisions,
	const std::string& file_path)
{
	cv::Mat result(rows, cols, CV_8UC1, cv::Scalar(0));
	for (const collision_t& collision : collisions) {
		cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
		cv::rectangle(result, rect, cv::Scalar(255));
	}
	cv::imwrite(file_path, result);
}

#endif // !COLLISIONS_HPP
