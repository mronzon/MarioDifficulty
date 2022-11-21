#pragma once

#include <vector>
#include <algorithm>
#include <string>


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>


struct pipe_t
{
	int x;
	int y;
	int height;
	int width;
	int id;
	bool inside;
	int go_id;
	bool dynamic_pipe;
};


bool inline comp_pipe(const pipe_t& a, const pipe_t& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

std::vector<cv::Mat> inline get_textures(const std::string& path, const std::string& to_find)
{
	std::vector<cv::Mat> textures;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
		if (entry.path().extension().string() != ".png") continue;
		if (entry.path().string().find(to_find) == std::string::npos) continue;
		cv::Mat texture = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
		textures.push_back(texture);
	}
	return textures;
}

std::vector<pipe_t> inline merge_pipe(std::vector<pipe_t>& pipes)
{
	// Sort unique pipe for rows (top -> bot), then cols (left -> right).
	std::sort(pipes.begin(), pipes.end(), comp_pipe);

	// Merge horizontaly connected collisions.
	std::vector<pipe_t> pipe_merged;
	pipe_t result;
	for (int cur = 0; cur < pipes.size();) {
		result = pipes[cur++];
		while (cur < pipes.size() && pipes[cur].x == result.x && pipes[cur].height == result.height && pipes[cur].y <= result.y + result.width) {
			result.width += pipes[cur].width - ((result.y + result.width) - pipes[cur].y);
			cur++;
		}
		pipe_merged.push_back(result);
	}

	// Merge vertically the connected collision
	std::vector<pipe_t> pipe_merged_final;
	bool pass = false;
	for (int cur = 0; cur < pipe_merged.size(); cur++)
	{
		//Check if the current collision has already been merged with another one.
		pass = false;
		for (const pipe_t& pipe : pipe_merged_final)
		{
			if (pipe_merged[cur].y == pipe.y && pipe_merged[cur].width == pipe.width)
			{
				if (pipe_merged[cur].x < pipe.x + pipe.height)
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
		result = pipe_merged[cur];
		for (int i = cur + 1; i < pipe_merged.size(); i++)
		{
			if (pipe_merged[i].y == result.y && pipe_merged[i].width == result.width && pipe_merged[i].x <= result.x + result.height)
			{
				result.height += pipe_merged[i].height - ((result.x + result.height) - pipe_merged[i].x);
			}
		}
		pipe_merged_final.push_back(result);
	}
	return pipe_merged_final;
}
