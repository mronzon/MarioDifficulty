#pragma once
#ifndef REACH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Returns the horizontal delta of the move necessary to perfrom a given vertical delta.
float horizontal_from_vertical(const cv::Point2f& velocity, float gravity, float delta_y) {
	float time = -(velocity.x + sqrtf(velocity.x * velocity.x + 2.f * gravity * delta_y)) / gravity;
	return velocity.y * time;
}

float time_from_to_small(const cv::Point2f& origin, const cv::Point2f& destination, const cv::Point2f& velocity, float gravity) {
	return -(velocity.x - sqrtf(velocity.x * velocity.x + 2.f * gravity * (origin.x - destination.x))) / gravity;
}

float time_from_to_large(const cv::Point2f& origin, const cv::Point2f& destination, const cv::Point2f& velocity, float gravity) {
	return -(velocity.x + sqrtf(velocity.x * velocity.x + 2.f * gravity * (origin.x - destination.x))) / gravity;
}

// Return the velocity at a destination point for a jump starting at an origin point with a velocity.
cv::Point2f descending_velocity(const cv::Point2f& origin, const cv::Point2f& destination, const cv::Point2f& velocity, float gravity) {
	float time = time_from_to_large(origin, destination, velocity, gravity);
	return cv::Point2f(velocity.x + gravity * time, (destination.y - origin.y) / time);
}

// Return the velocity at a destination point for a jump starting at an origin point with a velocity.
cv::Point2f ascending_velocity(const cv::Point2f& origin, const cv::Point2f& destination, const cv::Point2f velocity, float gravity) {
	float time = time_from_to_small(origin, destination, velocity, gravity);
	return cv::Point2f(velocity.x + gravity * time, (destination.y - origin.y) / time);
}

cv::Point2f ascending_summit(const cv::Point2f& origin, const cv::Point2f& velocity, float gravity) {
	float x = origin.x + (0.5f * velocity.x * velocity.x / gravity);
	float y = origin.y - velocity.y * velocity.x / gravity - 1;
	return cv::Point2f(x, y);
}

#endif // !REACH_H
