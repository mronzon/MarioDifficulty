#include "collisions.h"
#include "iostream"

void propagate_danger(const std::vector<collision_t>& collision_merged, cv::Mat& danger, int row, int col) {
	if (row < 0 || col < 0 || col >= danger.cols || // Point out of bounds.
		danger.at<uint8_t>(row, col) != 0) return;		// Point is already marked as danger.

	for (auto collision: collision_merged)
	{
		if(is_inside(collision, row, col))
		{
			return;
		}
	}
	
	danger.at<uint8_t>(row, col) = 255;
	propagate_danger(collision_merged, danger, row - 1, col - 1); // Repeat upper-left.
	propagate_danger(collision_merged, danger, row - 1, col); // Repeat upper.
	propagate_danger(collision_merged, danger, row - 1, col + 1); // Repeat upper-right.
}

int main(int argc, char* argv[]) {
	if (argc < 3) return -1;

	std::string folder_path = argv[1];
	std::string level_folder = folder_path + "\\" + argv[2];

	// Get collisions from cached file or calculate them if chache file cannot be read.
	std::vector<collision_t> collisions_merged;
	if (!read_collision(collisions_merged, level_folder + "\\level.json")) { // If the json file is not here !
		return -2;
	}
	
	cv::Mat danger(level_image.rows, level_image.cols, CV_8UC1, cv::Scalar(0));
	for (int y = 0; y < danger.cols; y++) {
		propagate_danger(collisions_merged, danger, level_image.rows - 1, y);
	}
	dbg_imwrite(level_folder + "\\danger.png", danger);
	return 10;
}