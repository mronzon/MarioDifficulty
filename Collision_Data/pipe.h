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

	// Merge vertically the connected collision
	std::vector<pipe_t> pipe_merged_final;
	pipe_t result;
	bool pass = false;
	for (int cur = 0; cur < pipes.size(); cur++)
	{
		//Check if the current collision has already been merged with another one.
		pass = false;
		for (const pipe_t& pipe : pipe_merged_final)
		{
			if (pipes[cur].y == pipe.y && pipes[cur].width == pipe.width)
			{
				if (pipes[cur].x < pipe.x + pipe.height)
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
		result = pipes[cur];
		for (int i = cur + 1; i < pipes.size(); i++)
		{
			if (pipes[i].y == result.y && pipes[i].width == result.width && pipes[i].x <= result.x + result.height)
			{
				result.height += pipes[i].height - ((result.x + result.height) - pipes[i].x);
			}
		}
		pipe_merged_final.push_back(result);
	}
	return pipe_merged_final;
}
