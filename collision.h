#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>

struct collision_t {
	int x, y, height, width;
};

std::string inline to_string(const collision_t& c) {
	return std::string(std::to_string(c.x) + "," + std::to_string(c.y) + "," + std::to_string(c.height) + "," + std::to_string(c.width));
}

collision_t inline from_string(const std::string& s) {
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

bool inline comp_collision(const collision_t& a, const collision_t& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

std::vector<cv::Mat> inline find_textures(const std::string& path)
{
	std::vector<cv::Mat> textures;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
		if (entry.path().extension().string() != ".png") continue;
		cv::Mat texture = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
		textures.push_back(texture);
	}
	return textures;
}

collision_t inline find_a_position(const std::vector<cv::Mat>& textures, const cv::Mat& level_image)
{
	collision_t position = { -1,-1,-1,-1 };
	for (const cv::Mat& end_texture : textures)
	{
		cv::Mat collision_hits;
		cv::matchTemplate(level_image, end_texture, collision_hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < collision_hits.rows - 8; x++)
			for (int y = 0; y < collision_hits.cols - 8; y++)
				if (collision_hits.at<float>(x, y) >= 0.85f)
				{
					position = { x, y, end_texture.rows, end_texture.cols };
					return position;
				}
	}
	return position;
}

std::vector<collision_t> inline merge_collisions(std::vector<collision_t>& collisions)
{
	// Sort unique collisions for rows (top -> bot), then cols (left -> right).
	std::sort(collisions.begin(), collisions.end(), comp_collision);

	// Merge horizontaly connected collisions.
	std::vector<collision_t> collision_merged;
	collision_t result;
	for (int cur = 0; cur < collisions.size();) {
		result = collisions[cur++];
		while (cur < collisions.size() && collisions[cur].x == result.x && collisions[cur].height == result.height && collisions[cur].y <= result.y + result.width) {
			result.width += collisions[cur].width - ((result.y + result.width) - collisions[cur].y);
			cur++;
		}
		collision_merged.push_back(result);
	}

	// Merge vertically the connected collision
	std::vector<collision_t> collision_merged_final;
	bool pass = false;
	for (int cur = 0; cur < collision_merged.size(); cur++)
	{
		//Check if the current collision has already been merged with another one.
		pass = false;
		for (const collision_t& collision : collision_merged_final)
		{
			if (collision_merged[cur].y == collision.y && collision_merged[cur].width == collision.width)
			{
				if (collision_merged[cur].x < collision.x + collision.height)
				{
					pass = true;
					break;
				}
			}
		}
		if (pass)
		{
			continue;
		}
		result = collision_merged[cur];
		for (int i = cur + 1; i < collision_merged.size(); i++)
		{
			if (collision_merged[i].y == result.y && collision_merged[i].width == result.width && collision_merged[i].x <= result.x + result.height)
			{
				result.height += collision_merged[i].height - ((result.x + result.height) - collision_merged[i].x);
			}
		}
		collision_merged_final.push_back(result);
	}
	return collision_merged_final;
}

#endif // !COLLISION_H
#pragma once
