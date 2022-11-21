/*
	Inputs: a level image and a set of collision textures.
	Output: a text file with all collision hits.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <vector>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv.hpp>

using namespace jsoncons;

#include "collision.h"
#include "pipe.h"
#include "enemy.h"

JSONCONS_ALL_MEMBER_TRAITS(collision_t, x, y, height, width);
JSONCONS_ALL_MEMBER_TRAITS(pipe_t, x, y, height, width, id, inside, go_id);

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;

	std::string level_path = "\\Niveau_1_2";
	std::string sprite_path = "\\Sprite";
	std::string base_path = argv[1];
	std::string level_image_path = base_path + level_path + "\\level.png";
	std::string collision_textures_path = base_path + sprite_path + "\\CollisionBlock";
	std::string spawn_textures_path = base_path + sprite_path + "\\Spawn";
	std::string end_textures_path = base_path + sprite_path + "\\End";
	std::string pipe_textures_path = base_path + sprite_path + "\\Pipes";
	
	// Fetch level image.
	cv::Mat level_image = cv::imread(level_image_path);

	// Get the spawn position of Mario and the End of the level.
	std::vector<cv::Mat> end_textures = find_textures(end_textures_path);
	std::vector<cv::Mat> spawn_textures = find_textures(spawn_textures_path);

	// Fetch collision textures from folder.
	std::vector<cv::Mat> collision_textures = find_textures(collision_textures_path);

	// Fetch the Pipe textures from folder.
	std::vector<cv::Mat> pipe_textures = find_textures(pipe_textures_path);

	//Detect the end of the level.
	collision_t end = find_a_position(end_textures, level_image);

	//Detect the spawn of the level.
	collision_t spawn = find_a_position(spawn_textures, level_image);

	// Identify the pipes in level image.
	std::vector<pipe_t> pipe_raw;
	for (const cv::Mat& pipe_texture : pipe_textures)
	{
		cv::Mat hits;
		cv::matchTemplate(level_image, pipe_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
		{
			for (int y = 0; y < hits.cols - 8; y++)
			{
				if (hits.at<float>(x, y) >= 0.85f)
				{
					pipe_raw.push_back(pipe_t{ x,y,pipe_texture.rows, pipe_texture.cols, -1, false, -2 , false });
				}
			}
		}
	}

	std::vector<pipe_t> pipe_merged = merge_pipe(pipe_raw);


	// We check the pipe that are used as an exit or an enter pipe.
	std::vector<cv::Mat> enter_textures = get_textures(pipe_textures_path, "enter");
	std::vector<cv::Mat> exit_textures = get_textures(pipe_textures_path, "exit");

	for (const cv::Mat& pipe_texture : enter_textures)
	{
		cv::Mat hits;
		cv::matchTemplate(level_image, pipe_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
		{
			for (int y = 0; y < hits.cols - 8; y++)
			{
				if (hits.at<float>(x, y) >= 0.85f)
				{
					for (pipe_t& pipe : pipe_merged)
					{
						if (pipe.y == y && x >= pipe.x && x <= pipe.x + pipe.height)
						{
							pipe.dynamic_pipe = true;
							pipe.inside = true;
						}
					}
				}
			}
		}
	}

	for (const cv::Mat& pipe_texture : exit_textures)
	{
		cv::Mat hits;
		cv::matchTemplate(level_image, pipe_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
		{
			for (int y = 0; y < hits.cols - 8; y++)
			{
				if (hits.at<float>(x, y) >= 0.85f)
				{
					for (pipe_t& pipe : pipe_merged)
					{
						if (pipe.y == y && x >= pipe.x && x <= pipe.x + pipe.height)
						{
							pipe.dynamic_pipe = true;
						}
					}
				}
			}
		}
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
	std::vector<pipe_t> pipe_final;
	for(pipe_t& pipe: pipe_merged)
	{
		if (!pipe.dynamic_pipe)
		{
			collision_raw.push_back(collision_t{ pipe.x, pipe.y, pipe.height, pipe.width });
		}
		else
		{
			pipe_final.push_back(pipe);
		}
	}
	std::vector<collision_t> collisions_merged = merge_collisions(collision_raw);


	/* Print out collisions images. */ {
		// Print out the unique collision image and the filled collision image.
		cv::Mat collision_merged_final_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		cv::Mat collision_filled_image(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		for (const collision_t& collision : collisions_merged) {
			cv::Rect rect(collision.y, collision.x, collision.width, collision.height);
			cv::rectangle(collision_merged_final_image, rect, cv::Scalar(255, 255, 255));
			cv::rectangle(collision_filled_image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
		}
		for (const pipe_t& pipe: pipe_final)
		{
			cv::Rect rect(pipe.y, pipe.x, pipe.width, pipe.height);
			cv::rectangle(collision_filled_image, rect, cv::Scalar(0, 0, 255), cv::FILLED);
		}
		cv::Rect rectEnd(end.y, end.x, end.width, end.height);
		cv::rectangle(collision_filled_image, rectEnd, cv::Scalar(255, 0, 0), cv::FILLED);
		cv::Rect rectSpawn(spawn.y, spawn.x, spawn.width, spawn.height);
		cv::rectangle(collision_filled_image, rectSpawn, cv::Scalar(0, 255, 0), cv::FILLED);
		cv::imwrite(base_path + level_path + "\\collision.png", collision_merged_final_image);
		cv::imwrite(base_path + level_path + "\\collision_filled.png", collision_filled_image);
	}

	/* Create the JSON file from the collision_merge vector. */
	{
		json json_content;

		std::ofstream json_file_merged_final(base_path + level_path + "\\level.json");

		json_content["static"]["positionCollision"] = collisions_merged;
		json_content["static"]["pipes"] = pipe_final;
		json_content["static"]["end"]["x"] = end.x;
		json_content["static"]["end"]["y"] = end.y;
		json_content["static"]["end"]["height"] = end.height;
		json_content["static"]["end"]["width"] = end.width;
		json_content["static"]["spawn"]["x"] = spawn.x;
		json_content["static"]["spawn"]["y"] = spawn.y;
		json_content["static"]["spawn"]["height"] = spawn.height;
		json_content["static"]["spawn"]["width"] = spawn.width;

		json_file_merged_final << pretty_print(json_content);
		json_file_merged_final.close();
	}
	
	cv::waitKey();
	return 0;
}