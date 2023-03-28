#pragma once
#ifndef METRIC_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <map>

float inline metric_area_filled(const cv::Mat& reach_filled, int start_y, int end_y, std::map<std::pair<int, int>, int>& enemy_map, float surface) {
	int reach_count = 0;
	int effective_count = 0;

	for (int x = 0; x < reach_filled.rows; x++)
		for (int y = start_y; y < end_y; y++) {
			reach_count += (reach_filled.at<uchar>(x, y) != 0);
			// si le pixel est dans reach + qu'il y a des pixels dessus
			if (reach_filled.at<uchar>(x, y) && enemy_map.find(std::pair<int, int>(x, y)) != enemy_map.end()) {
				effective_count += enemy_map[std::pair<int, int>(x, y)]*surface;
			}			
		}

	return static_cast<float>(effective_count) / static_cast<float>(reach_count);
}

#endif // !METRIC_H
