#include "collisions.h"

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
	if (argc < 2) return -1;

	std::string folder_path = argv[1];

	cv::Mat level_image = cv::imread(folder_path + "\\level.png", cv::IMREAD_COLOR);

	// Get collisions from cached file or calculate them if chache file cannot be read.
	std::vector<collision_t> collisions_merged;
	if (!read_collision_cache(collisions_merged, folder_path + "\\collisions.bin")) {
		std::vector<collision_t> collisions_aligned;

		// Fetch aligned 16*16 collisions from the level image.
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folder_path + "\\platforms")) {
			cv::Mat texture = cv::imread(entry.path().string().c_str(), cv::IMREAD_COLOR);
			cv::Mat hit_result;

			cv::matchTemplate(level_image, texture, hit_result, cv::TM_CCOEFF_NORMED);
			for (int x = 8; x < hit_result.rows; x += 16) { // Row requires an offset of 8 to fit the image.
				for (int y = 0; y < hit_result.cols; y += 16) {
					if (hit_result.at<float>(x, y) >= 0.85f) collisions_aligned.push_back(collision_t{ x,y,16,16 });
				}
			}
		}
		std::sort(collisions_aligned.begin(), collisions_aligned.end(), collision_compare);

		// Merge the aligned collisions into horizontal collisions.
		for (size_t i = 0; i < collisions_aligned.size(); i++) {
			collision_t result = collisions_aligned[i++];
			while (i < collisions_aligned.size() && collisions_aligned[i].y == result.y + result.w) {
				result.w += 16;
				i++;
			}
			collisions_merged.push_back(result);
		}

		// Save the merged collisions for future executions.
		write_collision_cache(collisions_merged, folder_path + "\\collisions.bin");
		collisions_merged.shrink_to_fit();

		dbg_write_img(level_image.rows, level_image.cols, collisions_aligned, folder_path + "\\collision_aligned.png");
		dbg_write_img(level_image.rows, level_image.cols, collisions_merged, folder_path + "\\collision_merged.png");
	}

	// Create an image with filled in collisions, used for collision detection.
	cv::Mat collision_filled(level_image.rows, level_image.cols, CV_8UC1, cv::Scalar(0));
	for (const collision_t& collision : collisions_merged) {
		cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
		cv::rectangle(collision_filled, rect, cv::Scalar(255), cv::FILLED);
	}
	dbg_imwrite(folder_path + "\\collision_filled.png", collision_filled);

	//
	cv::Mat danger(level_image.rows, level_image.cols, CV_8UC1, cv::Scalar(0));
	for (int y = 0; y < danger.cols; y++) {
		propagate_danger(collision_filled, danger, level_image.rows - 1, y);
	}
	dbg_imwrite(folder_path + "\\danger.png", danger);



	return 0;
}