/*
	Inputs: a level image and a set of collision textures.
	Output: a text file with all collision hits.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv.hpp>

using namespace jsoncons;

#include "collision.h"
#include "enemy.h"

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;

	std::string level_path = "\\Niveau_1_1";
	std::string base_path = argv[1];
	std::string level_image_path = base_path + level_path + "\\level.png";
	std::string collision_textures_path = base_path + "\\platforms";

	// Fetch level image.
	cv::Mat level_image = cv::imread(level_image_path);

	
	// Fetch collision textures from folder.
	std::vector<cv::Mat> collision_textures;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(collision_textures_path)) {
		if (entry.path().extension().string() != ".png") continue;
		cv::Mat texture = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
		collision_textures.push_back(texture);
	}

	// Identify collisions in level image.
	std::vector<collision_t> collision_raw;
	for (const cv::Mat& collision_texture : collision_textures) {
		cv::Mat collision_hits;
		cv::matchTemplate(level_image, collision_texture, collision_hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < collision_hits.rows - 8; x++)
			for (int y = 0; y < collision_hits.cols - 8; y++)
				if (collision_hits.at<float>(x, y) >= 0.85f)
				{
					collision_raw.push_back(collision_t{ x, y, collision_texture.rows, collision_texture.cols });
				}
	}

	// Sort unique collisions for rows (top -> bot), then cols (left -> right).
	std::sort(collision_raw.begin(), collision_raw.end(), comp);

	// Merge horizontaly connected collisions.
	std::vector<collision_t> collision_merged;
	collision_t result;
	for (int cur = 0; cur < collision_raw.size();) {
		result = collision_raw[cur++];
		while (cur < collision_raw.size() && collision_raw[cur].x == result.x && collision_raw[cur].h == result.h && collision_raw[cur].y <= result.y + result.w) {
			result.w += collision_raw[cur].w - ((result.y + result.w) - collision_raw[cur].y);
			cur++;
		}
		collision_merged.push_back(result);
	}

	// Merge vertically the connected collision
	std::vector<collision_t> collision_merged_final;
	std::vector<std::tuple<int, int>> y_width_checked;
	y_width_checked.emplace_back(std::tuple<int, int>(-1, -1));
	std::tuple<int, int> to_check;
	for(int cur = 0; cur < collision_merged.size(); cur++)
	{
		to_check = std::tuple<int, int>(collision_merged[cur].y, collision_merged[cur].w);
		if (std::find(y_width_checked.begin(), y_width_checked.end(), to_check) != y_width_checked.end()) continue; // We check if this collision has already been merged.
		y_width_checked.push_back(to_check);
		result = collision_merged[cur];
		for(int i = cur + 1; i < collision_merged.size(); i++)
		{
			if (collision_merged[i].y == result.y && collision_merged[i].w == result.w && collision_merged[i].x <= result.x + result.h)
			{
				result.h += collision_merged[i].h - ((result.x + result.h) - collision_merged[i].x);
			}
		}
		collision_merged_final.push_back(result);
	}


	/* Print out collision images. */ {
		// Print out the raw collision image.
		cv::Mat collision_raw_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collision_raw) {
			cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
			cv::rectangle(collision_raw_image, rect, cv::Scalar(255, 255, 255));
		}
		cv::imwrite(base_path + level_path + "\\collision_raw.png", collision_raw_image);

		// Print out the unique collision image.
		cv::Mat collision_merged_final_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collision_merged_final) {
			cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
			cv::rectangle(collision_merged_final_image, rect, cv::Scalar(255, 255, 255));
		}
		cv::imwrite(base_path + level_path + "\\collision_merged_final.png", collision_merged_final_image);

		// Print out the merged collision image.
		cv::Mat collision_merged_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collision_merged) {
			cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
			cv::rectangle(collision_merged_image, rect, cv::Scalar(255, 255, 255));
		}
		cv::imwrite(base_path + level_path + "\\collision_merged.png", collision_merged_image);

		// Print out the filled collision image.
		cv::Mat collision_filled_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collision_merged) {
			cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
			cv::rectangle(collision_filled_image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
		}
		cv::imwrite(base_path + level_path + "\\collision_filled.png", collision_filled_image);
	}

	/* Print out collision files. */ {
		// Print out the raw collision file.
		std::ofstream collision_raw_file(base_path + level_path + "\\collision_raw.txt");
		collision_raw_file << "x, y, height, width\n";
		for (const collision_t& collision : collision_raw)	collision_raw_file << to_string(collision) << "\n";
		collision_raw_file.close();

		// Print out the unique collision file.
		std::ofstream collision_merged_final_file(base_path + level_path + "\\collision_merged_final.txt");
		collision_merged_final_file << "x, y, height, width\n";
		for (const collision_t& collision : collision_merged_final) collision_merged_final_file << to_string(collision) << "\n";
		collision_merged_final_file.close();

		// Print out the merged collision file.
		std::ofstream collision_merged_file(base_path + level_path + "\\collision_merged.txt");
		collision_merged_file << "x, y, height, width\n";
		for (const collision_t& collision : collision_merged) collision_merged_file << to_string(collision) << "\n";
		collision_merged_file.close();
	}

	/* Create the JSON file from the collision_merge vector. */
	{
		csv::csv_options options;
		options.assume_header(true)
			.trim(true)
			.ignore_empty_values(true)
			.column_types("integer,integer,integer,integer");

		std::ofstream json_file_merged_final(base_path + level_path + "\\collision_merged_final.json");
		std::ifstream is_merged_final(base_path + level_path + "\\collision_merged_final.txt");
		ojson tasks_merged_final = csv::decode_csv<ojson>(is_merged_final, options);
		json_file_merged_final << pretty_print(tasks_merged_final);
		json_file_merged_final.close();

		std::ofstream json_file_raw(base_path + level_path + "\\collision_raw.json");
		std::ifstream is_raw(base_path + level_path + "\\collision_raw.txt");
		ojson tasks_raw = csv::decode_csv<ojson>(is_raw, options);
		json_file_raw << pretty_print(tasks_raw);
		json_file_raw.close();

		std::ofstream json_file_merged(base_path + level_path + "\\collision_merged.json");
		std::ifstream is_merged(base_path + level_path + "\\collision_merged.txt");
		ojson tasks_merged = csv::decode_csv<ojson>(is_merged, options);
		json_file_merged << pretty_print(tasks_merged);
		json_file_merged.close();

	}
	
	cv::waitKey();
	return 0;
}