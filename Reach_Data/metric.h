#pragma once
#ifndef METRIC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

float metric_area_filled(const cv::Mat& reach_filled, const cv::Mat& danger_filled) {
	int reach_count = 0;
	int effective_count = 0;

	for (int x = 0; x < reach_filled.rows; x++)
		for (int y = 0; y < reach_filled.cols; y++) {
			reach_count += (reach_filled.at<uchar>(x, y) != 0);
			effective_count += (reach_filled.at<uchar>(x, y) && danger_filled.at<uchar>(x, y));
		}

	return static_cast<float>(effective_count) / reach_count;
}

float metric_area_gradient(const cv::Mat& reach_gradient, int platform_pixel_count, const cv::Mat& danger_filled) {
	float reach_count = 0.f;
	float effective_count = 0.f;

	for (int x = 0; x < reach_gradient.rows; x++)
		for (int y = 0; y < reach_gradient.cols; y++) {
			float temp = static_cast<float>(reach_gradient.at<int>(x, y)) / platform_pixel_count;
			reach_count += temp;
			effective_count += temp / 255.f * danger_filled.at<uchar>(x, y);
		}

	return effective_count / reach_count;
}

bool is_border(const cv::Mat& danger_filled_image, const cv::Point& pixel) {
	bool result = false;
	if (danger_filled_image.at<uchar>(pixel.x, pixel.y) == 0) {
		if (pixel.x > 0 && pixel.y > 0 && danger_filled_image.at<uchar>(pixel.x - 1, pixel.y - 1) > 0) result = true;
		if (pixel.x > 0 && danger_filled_image.at<uchar>(pixel.x - 1, pixel.y) > 0) result = true;
		if (pixel.x > 0 && pixel.y < danger_filled_image.cols - 1 && danger_filled_image.at<uchar>(pixel.x - 1, pixel.y + 1)) result = true;

		if (pixel.y > 0 && danger_filled_image.at<uchar>(pixel.x, pixel.y - 1) > 0) result = true;
		if (pixel.y < danger_filled_image.cols - 1 && danger_filled_image.at<uchar>(pixel.x, pixel.y + 1) > 0) result = true;

		if (pixel.x < danger_filled_image.rows - 1 && pixel.y > 0 && danger_filled_image.at<uchar>(pixel.x + 1, pixel.y - 1) > 0) result = true;
		if (pixel.x < danger_filled_image.rows - 1 && danger_filled_image.at<uchar>(pixel.x + 1, pixel.y) > 0) result = 0;
		if (pixel.x < danger_filled_image.rows - 1 && pixel.y < danger_filled_image.cols - 1 && danger_filled_image.at<uchar>(pixel.x + 1, pixel.y + 1) > 0) result = true;
	}
	return result;
}

float metric_premimeter_filled(const cv::Mat& reach_filled, const cv::Mat& danger_filled) {
	float result = 0.f;

	for (int x = 0; x < reach_filled.rows; x++)
		for (int y = 0; y < reach_filled.cols; y++)
			if (reach_filled.at<uchar>(x, y) && is_border(danger_filled, cv::Point(x, y)))
				result += 1.f;
	return result;
}

float metric_premimeter_gradient(const cv::Mat& reach_gradient, int platform_pixel_count, const cv::Mat& danger_filled) {
	float result = 0.f;

	for (int x = 0; x < reach_gradient.rows; x++)
		for (int y = 0; y < reach_gradient.cols; y++)
			if (reach_gradient.at<int>(x, y) && is_border(danger_filled, cv::Point(x, y)))
				result += static_cast<float>(reach_gradient.at<int>(x, y)) / platform_pixel_count;
	return result;
}

#endif // !METRIC_H