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

JSONCONS_ALL_MEMBER_TRAITS(collision_t, x, y, height, width);

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;

	std::string level_path = "\\Niveau_4_1";
	std::string base_path = argv[1];
	std::string level_image_path = base_path + level_path + "\\level.png";
	std::string collision_textures_path = base_path + "\\CollisionBlock";

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
		while (cur < collision_raw.size() && collision_raw[cur].x == result.x && collision_raw[cur].height == result.height && collision_raw[cur].y <= result.y + result.width) {
			result.width += collision_raw[cur].width - ((result.y + result.width) - collision_raw[cur].y);
			cur++;
		}
		collision_merged.push_back(result);
	}

	// Merge vertically the connected collision
	std::vector<collision_t> collision_merged_final;
	bool pass = false;
	for(int cur = 0; cur < collision_merged.size(); cur++)
	{
		//Check if the current collision has already been merged with another one.
		pass = false;
		for(int i = 0; i < collision_merged_final.size(); i++)
		{
			if(collision_merged[cur].y == collision_merged_final[i].y && collision_merged[cur].width == collision_merged_final[i].width)
			{
				if(collision_merged[cur].x < collision_merged_final[i].x + collision_merged_final[i].height)
				{
					pass = true;
					break;
				}
			}
		}
		if(pass)
		{
			continue;
		}
		result = collision_merged[cur];
		for(int i = cur + 1; i < collision_merged.size(); i++)
		{
			if (collision_merged[i].y == result.y && collision_merged[i].width == result.width && collision_merged[i].x <= result.x + result.height)
			{
				result.height += collision_merged[i].height - ((result.x + result.height) - collision_merged[i].x);
			}
		}
		collision_merged_final.push_back(result);
	}


	/* Print out collisions images. */ {
		// Print out the unique collision image and the filled collision image.
		cv::Mat collision_merged_final_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		cv::Mat collision_filled_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collision_merged_final) {
			cv::Rect rect(collision.y, collision.x, collision.width, collision.height);
			cv::rectangle(collision_merged_final_image, rect, cv::Scalar(255, 255, 255));
			cv::rectangle(collision_filled_image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
		}
		cv::imwrite(base_path + level_path + "\\collision.png", collision_merged_final_image);
		cv::imwrite(base_path + level_path + "\\collision_filled.png", collision_filled_image);
	}

	/* Create the JSON file from the collision_merge vector. */
	{
		json json_content;

		std::ofstream json_file_merged_final(base_path + level_path + "\\level.json");
		std::string s;
		jsoncons::encode_json(collision_merged_final, s, jsoncons::indenting::indent);
		auto json_content_static = jsoncons::decode_json<std::vector<collision_t>>(s);

		json_content["static"] = json_content_static;

		json_file_merged_final << pretty_print(json_content);
		json_file_merged_final.close();
	}
	
	cv::waitKey();
	return 0;
}