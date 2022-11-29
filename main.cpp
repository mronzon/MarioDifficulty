/*
	Inputs: a level image and a set of collision textures.
	Output: a text file with all collision hits.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <vector>

#include <jsoncons/json.hpp>

using namespace jsoncons;

#include "collision.h"
#include "pipe.h"
#include "enemy.h"
#include "platforms.h"

JSONCONS_ALL_MEMBER_TRAITS(collision_t, x, y, height, width);
JSONCONS_ALL_MEMBER_TRAITS(pipe_t, x, y, height, width, id, inside, go_id);
JSONCONS_ALL_MEMBER_TRAITS(moving_lift_t, x, y, width, height, axe, first_limit, second_limit, initial_velocity);
JSONCONS_ALL_MEMBER_TRAITS(balanced_lift_t, x_first, y_first, width_first, height_first, x_second, y_second, width_second, height_second, total_length, max_x);

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;

	std::string level_path = "\\Niveau_4_3";
	std::string sprite_path = "\\Sprite";
	std::string base_path = argv[1];
	std::string level_image_path = base_path + level_path + "\\level.png";
	std::string collision_textures_path = base_path + sprite_path + "\\CollisionBlock";
	std::string spawn_textures_path = base_path + sprite_path + "\\Spawn";
	std::string end_textures_path = base_path + sprite_path + "\\End";
	std::string pipe_textures_path = base_path + sprite_path + "\\Pipes";
	std::string enemies_textures_path = base_path + sprite_path + "\\Enemies";
	std::string platform_textures_path = base_path + sprite_path + "\\Platforms";
	
	// Fetch level image.
	cv::Mat level_image = cv::imread(level_image_path);

	// Get the spawn position of Mario and the End of the level.
	std::vector<cv::Mat> end_textures = find_textures(end_textures_path);
	std::vector<cv::Mat> spawn_textures = find_textures(spawn_textures_path);

	// Fetch collision textures from folder.
	std::vector<cv::Mat> collision_textures = find_textures(collision_textures_path);

	// Fetch the Pipe textures from folder.
	std::vector<cv::Mat> pipe_textures = find_textures(pipe_textures_path);

	// Fetch Enemies textures from folder.
	std::vector<cv::Mat> enemies_textures = find_textures(enemies_textures_path);

	// Fetch platforms textures from folder.
	std::vector<cv::Mat> platforms_textures = find_textures(platform_textures_path);

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
		for (int x = 0; x < hits.rows; x++)
		{
			for (int y = 0; y < hits.cols; y++)
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

	// Identify enemies in level image.
	std::vector<enemy> enemy_raw;
	for (const cv::Mat& enemy_texture : enemies_textures) {
		cv::Mat enemy_hits;
		cv::matchTemplate(level_image, enemy_texture, enemy_hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < enemy_hits.rows - 8; x++)
			for (int y = 0; y < enemy_hits.cols - 8; y++)
				if (enemy_hits.at<float>(x, y) >= 0.70f)
				{
					//enemy_raw.push_back(enemy{ x, y, type });
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


	// Platforms code:
	// We detect all the platform first and after that we try to put them to the right type.

	std::vector<collision_t> raw_platforms;
	for (const cv::Mat& platform_texture : platforms_textures) {
		cv::Mat hits;
		cv::matchTemplate(level_image, platform_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
			for (int y = 0; y < hits.cols - 8; y++)
				if (hits.at<float>(x, y) >= 0.85f)
				{
					raw_platforms.push_back(collision_t{ x, y, platform_texture.rows, platform_texture.cols });
				}
	}

	raw_platforms = merge_collisions(raw_platforms);

	//Now we have all the platforms. We can detect the balanced platforms using the dot on the top
	std::vector<balanced_lift_t> balanced_lifts;
	std::vector<moving_lift_t> normal_lifts;
	std::string balance_platforms_path = platform_textures_path + "\\BalancePoint";
	std::vector<position_t> pos_balance_point_right;
	std::vector<cv::Mat> balance_right_textures = get_textures(balance_platforms_path, "right");
	std::vector<cv::Mat> balance_left_textures = get_textures(balance_platforms_path, "left");
	for(const cv::Mat& balance_texture : balance_left_textures)
	{
		cv::Mat hits;
		cv::matchTemplate(level_image, balance_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
		{
			for (int y = 0; y < hits.cols - 8; y++)
			{
				if (hits.at<float>(x, y) >= 0.85f)
				{
					int i = 0;
					while(i < raw_platforms.size())
					{
						if((raw_platforms[i].y <= y) && y <= (raw_platforms[i].y + raw_platforms[i].width))
						{
							balanced_lifts.push_back(balanced_lift_t{ raw_platforms[i].x, raw_platforms[i].y, raw_platforms[i].width, raw_platforms[i].height,0,0,0,0, raw_platforms[i].x - x, x-balance_texture.rows});
							pos_balance_point_right.push_back(position_t{ x,y });
							break;
						}
						i++;
					}
					if(i != raw_platforms.size())
					{
						raw_platforms.erase(raw_platforms.begin() + i);
					}
				}
			}
		}
	}

	for (const cv::Mat& balance_texture : balance_right_textures)
	{
		cv::Mat hits;
		cv::matchTemplate(level_image, balance_texture, hits, cv::TM_CCOEFF_NORMED);
		for (int x = 0; x < hits.rows - 8; x++)
		{
			for (int y = 0; y < hits.cols - 8; y++)
			{
				if (hits.at<float>(x, y) >= 0.85f)
				{
					int i = 0;
					while (i < raw_platforms.size())
					{
						if ((raw_platforms[i].y <= y) && y <= (raw_platforms[i].y + raw_platforms[i].width))
						{
							int old_y = 20000000;
							int good_index = -1;
							for(int k = 0; k < pos_balance_point_right.size(); k++)
							{
								if(pos_balance_point_right[k].x == x)
								{
									if((pos_balance_point_right[k].y - y) > 0 && (pos_balance_point_right[k].y - y) < old_y)
									{
										old_y = (pos_balance_point_right[k].y - y);
										good_index = k;
									}
								}
							}
							if(good_index != -1)
							{
								balanced_lifts[good_index].x_second = raw_platforms[i].x;
								balanced_lifts[good_index].y_second = raw_platforms[i].y;
								balanced_lifts[good_index].height_second = raw_platforms[i].height;
								balanced_lifts[good_index].width_second = raw_platforms[i].width;
								balanced_lifts[good_index].total_length += raw_platforms[i].x - x;
							}
							break;
						}
						i++;
					}
					if (i != raw_platforms.size())
					{
						raw_platforms.erase(raw_platforms.begin() + i);
					}
				}
			}
		}
	}

	for(const collision_t& lift : raw_platforms)
	{
		normal_lifts.push_back(moving_lift_t{ lift.x, lift.y, lift.width, lift.height, -1, 0, 0, 0 });
	}

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

		//Display of the lifts
		for(const moving_lift_t& lift : normal_lifts)
		{
			cv::Rect rect(lift.y, lift.x, lift.width, lift.height);
			cv::rectangle(collision_filled_image, rect, cv::Scalar(47, 173, 208), cv::FILLED);
		}
		for (const balanced_lift_t& lift : balanced_lifts)
		{
			cv::Rect rectLeft(lift.y_first, lift.x_first, lift.width_first, lift.height_first);
			cv::rectangle(collision_filled_image, rectLeft, cv::Scalar(208, 82, 47), cv::FILLED);
			cv::Rect rectRight(lift.y_second, lift.x_second, lift.width_second, lift.height_second);
			cv::rectangle(collision_filled_image, rectRight, cv::Scalar(208, 82, 47), cv::FILLED);
		}
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
		json_content["dynamic"]["platform"]["moving"] = normal_lifts;
		json_content["dynamic"]["platform"]["balanced"] = balanced_lifts;

		json_file_merged_final << pretty_print(json_content);
		json_file_merged_final.close();
	}
	
	cv::waitKey();
	return 0;
}