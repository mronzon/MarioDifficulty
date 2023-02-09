#include "collisions.h"
#include "iostream"

void propagate_danger(const cv::Mat& collision_filled, cv::Mat& danger, int row, int col) {
	if (row < 0 || col < 0 || col >= danger.cols || // Point out of bounds.
		collision_filled.at<uint8_t>(row, col) > 0 || // Point is a collision.
		danger.at<uint8_t>(row, col) != 0) return;		// Point is already marked as danger.

	danger.at<uint8_t>(row, col) = 255;
	propagate_danger(collision_filled, danger, row - 1, col - 1); // Repeat upper-left.
	propagate_danger(collision_filled, danger, row - 1, col); // Repeat upper.
	propagate_danger(collision_filled, danger, row - 1, col + 1); // Repeat upper-right.
}


const std::string collisions_cache = "collisions.bin";

int main(int argc, char* argv[]) {
	if (argc < 3) return -1;

	std::string folder_path = argv[1];
	std::string level_folder = folder_path + "\\" + argv[2];

	cv::Mat level_image = cv::imread(level_folder + "\\level.png", cv::IMREAD_COLOR);

	// Get collisions from cached file or calculate them if chache file cannot be read.
	std::vector<collision_t> collisions_merged;
	if (!read_collision(collisions_merged, level_folder + "\\level.json")) { // If the json file is not here !
		return -2;
	} 

	// Create an image with filled in collisions, used for collision detection.
	cv::Mat collision_filled(level_image.rows, level_image.cols, CV_8UC1, cv::Scalar(0));
	for (const collision_t& collision : collisions_merged) {
		cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
		cv::rectangle(collision_filled, rect, cv::Scalar(255), cv::FILLED);
	}

	//
	cv::Mat danger(level_image.rows, level_image.cols, CV_8UC1, cv::Scalar(0));
	for (int y = 0; y < danger.cols; y++) {
		propagate_danger(collision_filled, danger, level_image.rows - 1, y);
	}
	dbg_imwrite(level_folder + "\\danger.png", danger);



	return 10;
}