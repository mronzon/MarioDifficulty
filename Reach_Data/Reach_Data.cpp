#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "collision.h"
#include "metric.h"
#include "reach.h"

struct {
	cv::Point2f velocity = cv::Point2f(30.f, 10.f);
	float gravity = -5.f;
	float jump_height = -(velocity.x * velocity.x) / (2.f * gravity);
	float jump_half_width = -(velocity.y * velocity.x) / gravity;
} globals;


int main(int argc, char* argv[]) {
	std::vector<int> my_vec;

	if (argc != 2) return 0;

	clock_t start_time = clock();

	std::string base_path = argv[1];

	std::pair<int, int> dim_image;
	get_dimension(dim_image, base_path + "\\level.json");
	
	// Get the collision from the json file.
	std::vector<collision_t> collisions = from_file(base_path + "\\level.json");	
	
	std::vector<cv::Point> platform_pixels;
	/* Find all platform pixels. (aka all pixels where the player can jump form. */ {
		cv::Mat collision_filled_image = cv::imread(base_path + "\\collision_filled.png", cv::IMREAD_GRAYSCALE);
		for (const collision_t& collision : collisions)
			for (int y = collision.y; y < collision.y + collision.w; y++)
				if (collision_filled_image.at<uchar>(collision.x - 1, y) != 255)
					platform_pixels.emplace_back(cv::Point(collision.x - 1, y));

		cv::Mat platform_image(collision_filled_image.rows, collision_filled_image.cols, CV_8UC1, cv::Scalar(0));
		for (const cv::Point& pixel : platform_pixels)
			platform_image.at<uchar>(pixel.x, pixel.y) = 255;
		cv::imwrite(base_path + "\\platform_image.png", platform_image);
	}
	
	std::ifstream jump_up_image("jump_up.png");
	std::ifstream jump_down_image("jump_down.png");

	if(!(jump_down_image && jump_up_image))
	{
		cv::Mat jump_up((int) globals.jump_height, static_cast<int>(globals.jump_half_width)* 2, CV_8UC1, cv::Scalar(0));
		cv::Mat jump_down((int) globals.jump_height, static_cast<int>(globals.jump_half_width) * 2, CV_8UC1, cv::Scalar(0));
        for (int x = 0; x < jump_up.rows; x++) {
        	float hd = horizontal_from_vertical(cv::Point2f(0.f, globals.velocity.y), globals.gravity, static_cast<float>(-x));
        	for (int y = 0; y <= static_cast<int>(globals.jump_half_width - hd); y++) jump_up.at<uchar>(x, y) = 255;
        	for (int y = 0; y <= static_cast<int>(globals.jump_half_width + hd); y++) jump_down.at<uchar>(x, y) = 255;
        }
		cv::imwrite("jump_up.png", jump_up);
		cv::imwrite("jump_down.png", jump_down);
	}
	
	cv::Mat reach_image(dim_image.first, dim_image.second, CV_32SC1, cv::Scalar(0));

	int count = 0;
	
	for (const cv::Point& platform_pixel : platform_pixels) {
		cv::Point topLeftCornerRight(dim_image.first,dim_image.second);
		cv::Point bottomRightCornerRight(0, 0);
		cv::Mat jump_right_image(dim_image.first, dim_image.second, CV_8UC1, cv::Scalar(0));
		/* Calculates the right sided jump. */ {
		cv::Point summit(platform_pixel.x - globals.jump_height, platform_pixel.y + globals.jump_half_width);
		
		cv::Mat ascending_image(jump_right_image.rows, jump_right_image.cols, CV_8UC1, cv::Scalar(0));
		/* Paint the full ascending motion of the jump. */ {
			for (int x = __max(summit.x, 0) /* if jump is higher than the top of the image. */; x < platform_pixel.x; x++) {
				float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, globals.velocity.y), globals.gravity, summit.x - x);
				for (int y = platform_pixel.y; y <= __min(summit.y - horizontal_delta, ascending_image.cols - 1) /* if jump is too far right of the image. */; y++)
				{
					modifyRectangleCorner(x, y, topLeftCornerRight, bottomRightCornerRight);
					ascending_image.at<uchar>(x, y) = 255;
				}
			}
		}

		cv::Mat descending_image(jump_right_image.rows, jump_right_image.cols, CV_8UC1, cv::Scalar(0));
		/* Paint the full descending motion of the jump. */ {
			for (int x = __max(summit.x, 0) /* if jump is higher than the top of the image. */; x < descending_image.rows; x++) {
				float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, globals.velocity.y), globals.gravity, summit.x - x);
				for (int y = platform_pixel.y; y <= __min(summit.y + horizontal_delta, descending_image.cols - 1) /* if jump is too far right of the image. */; y++)
				{
					modifyRectangleCorner(x, y, topLeftCornerRight, bottomRightCornerRight);
					descending_image.at<uchar>(x, y) = 255;
				}
			}
		}

		for (const collision_t& collision : collisions) {
			bool process_ascending = true;
			bool process_descending = true;
			/* Cut-offs for optimisation (platforms that will have no impact on reach). */ {
				if (collision.y + collision.w <= platform_pixel.y) continue;   // The platform is to the left of the jump.
				if (collision.x + collision.h <= summit.x) continue;		   // The platform is above the jump height.
				if (collision.x > platform_pixel.x) process_ascending = false; // Ascending specific: the platform is below the starting point.
				if (collision.y > summit.y) process_ascending = false;		   // Ascending specific: the platform is farther right than the summit.

				float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, globals.velocity.y), globals.gravity, summit.x - descending_image.rows);
				if (collision.y > summit.y + horizontal_delta) process_descending = false; // Descending specific: the platform is farther right than the jump can reach.
			}

			if (process_ascending) {
				cv::Point   ul_corner(__max(collision.x, summit.x), collision.y - 1);
				cv::Point2f ul_velocity = ascending_velocity(platform_pixel, ul_corner, globals.velocity, globals.gravity);
				cv::Point2f ul_summit   = ascending_summit(ul_corner, ul_velocity, globals.gravity);

				cv::Point   lr_corner(collision.x + collision.h - 1, collision.y + collision.w);
				cv::Point2f lr_velocity = ascending_velocity(platform_pixel, lr_corner, globals.velocity, globals.gravity);
				cv::Point2f lr_summit   = ascending_summit(lr_corner, lr_velocity, globals.gravity);

				if (ul_velocity.y <= globals.velocity.y) {
					int start_x = __max(ul_summit.x, lr_summit.x); // Get rid of floating-point errors.
					start_x = __max(start_x, 0); // if jump is higher than the top of the image.
					for (int x = start_x; x <= lr_corner.x; x++) {
						float right = lr_summit.y - horizontal_from_vertical(cv::Point2f(0.f, lr_velocity.y), globals.gravity, lr_summit.x - x) - 1;
						float left  = ul_summit.y - horizontal_from_vertical(cv::Point2f(0.f, ul_velocity.y), globals.gravity, ul_summit.x - x) + 1;
						right = __min(right, ascending_image.cols - 1); // If the platform is at the same level as the jump origin (inf otherwise).
						left  = __max(__min(left, ascending_image.cols - 1) /* if the jump is farther right than the image. */, platform_pixel.y);

						for (int y = left; y <= right; y++)
							ascending_image.at<uchar>(x, y) = 0;
					}

					for (int x = start_x; x < ascending_image.rows; x++) {
						float right = lr_summit.y + horizontal_from_vertical(cv::Point2f(0.f, lr_velocity.y), globals.gravity, lr_summit.x - x) - 1;
						float left = ul_summit.y + horizontal_from_vertical(cv::Point2f(0.f, ul_velocity.y), globals.gravity, ul_summit.x - x) + 1;
						right = __max(__min(right, ascending_image.cols - 1), 0); // If the jump is farther right than the image.
						left = __max(__min(left, ascending_image.cols - 1), 0); // If the jump is farther right than the image.

						for (int y = left; y <= right; y++)
							descending_image.at<uchar>(x, y) = 0;
					}
				}
			}

			if (process_descending) {
				cv::Point   ll_corner(collision.x + collision.h - 1, collision.y - 1);
				cv::Point2f ll_velocity = descending_velocity(platform_pixel, ll_corner, globals.velocity, globals.gravity);

				cv::Point   ur_corner(collision.x, collision.y + collision.w);
				cv::Point2f ur_velocity = descending_velocity(platform_pixel, ur_corner, globals.velocity, globals.gravity);

				for (int x = ur_corner.x; x < descending_image.rows; x++) {
					float right = ur_corner.y + horizontal_from_vertical(ur_velocity, globals.gravity, ur_corner.x - x) - 1;
					float left  = ll_corner.y + horizontal_from_vertical(ll_velocity, globals.gravity, __min(ll_corner.x - x, 0)) + 1;
					right = __min(right, descending_image.cols - 1); // If the jump is farther right than the image.
					left  = __min(__max(left, 0), descending_image.cols - 1); // If the jump is farther right than the image.

					for (int y = left; y <= right; y++) {
						descending_image.at<uchar>(x, y) = 0;
					}
				}
			}

			if (process_ascending) {
				cv::Point corner(collision.x + collision.h - 1, collision.y + collision.w - 1);
				float time = time_from_to_small(platform_pixel, corner, globals.velocity, globals.gravity);

				cv::Point   ll_corner(collision.x + collision.h - 1, collision.y);
					cv::Point2f hit(corner.x, __min(platform_pixel.y + globals.velocity.y * time, corner.y));
					cv::Point2f hit_velocity = ascending_velocity(platform_pixel, hit, globals.velocity, globals.gravity);


				//if (hit.y < collision.y) continue; // The jump hits the side of the platform, not underneath. 
				for (int x = corner.x; x < descending_image.rows; x++) {
					float left  = __max(ll_corner.y, platform_pixel.y);
					float right = hit.y + horizontal_from_vertical(cv::Point2f(0.f, hit_velocity.y), globals.gravity, hit.x - x);
					right = __min(right, descending_image.cols - 1); // if the jump is farther right than the image.

					for (int y = left; y <= right; y++)
					{
						modifyRectangleCorner(x, y, topLeftCornerRight, bottomRightCornerRight);
						descending_image.at<uchar>(x, y) = 255;
					}
						
					}
				}
			}
			/* Merge both ascending and descending parts into one image. */ {
				for (int x = topLeftCornerRight.x; x <= bottomRightCornerRight.x; x++)
					for (int y = topLeftCornerRight.y; y <= bottomRightCornerRight.y; y++)
						jump_right_image.at<uchar>(x, y) += ascending_image.at<uchar>(x, y) + descending_image.at<uchar>(x, y);
			}
		}
		cv::Point topLeftCornerLeft(dim_image.first,dim_image.second);
		cv::Point bottomRightCornerLeft(0, 0);
		cv::Mat jump_left_image(dim_image.first, dim_image.second, CV_8UC1, cv::Scalar(0));
		/* Calculates the left sided jump. */ {
			cv::Point summit(platform_pixel.x - globals.jump_height, platform_pixel.y - globals.jump_half_width);
			
			
			cv::Mat ascending_image(jump_left_image.rows, jump_left_image.cols, CV_8UC1, cv::Scalar(0));
			/* Paint the full ascending motion of the jump. */ {
				for (int x = __max(summit.x, 0) /* if jump is higher than the top of the image. */; x < platform_pixel.x; x++) {
					float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, -globals.velocity.y), globals.gravity, summit.x - x);
					for (int y = __max(summit.y - horizontal_delta, 0) /* if jump is too far left of the image. */; y <= platform_pixel.y; y++)
					{
						ascending_image.at<uchar>(x, y) = 255;
						modifyRectangleCorner(x, y, topLeftCornerLeft, bottomRightCornerLeft);
					}
				}
			}

			cv::Mat descending_image(jump_left_image.rows, jump_left_image.cols, CV_8UC1, cv::Scalar(0));
			/* Paint the full descending motion of the jump. */ {
				for (int x = __max(summit.x, 0) /* if jump is higher than the top of the image. */; x < descending_image.rows; x++) {
					float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, -globals.velocity.y), globals.gravity, summit.x - x);
					for (int y = __max(summit.y + horizontal_delta, 0) /* if jump is too far left of the image. */; y <= platform_pixel.y; y++)
					{
						descending_image.at<uchar>(x, y) = 255;
						modifyRectangleCorner(x, y, topLeftCornerLeft, bottomRightCornerLeft);
					}
				}
			}
			

			for (const collision_t& collision : collisions) {
				bool process_ascending = true;
				bool process_descending = true;
				/* Cut-offs for optimisation (platforms that will have no impact on reach). */ {
					if (collision.y > platform_pixel.y) continue;						  // The platform is to the right of the jump.
					if (collision.x + collision.h <= summit.x) continue;				  // The platform is above the jump height.
					if (collision.x > platform_pixel.x) process_ascending = false;		  // Ascending specific: the platform is below the starting point.
					if (collision.y + collision.w <= summit.y) process_ascending = false; // Ascending specific: the platform is farther left than the summit.

					float horizontal_delta = horizontal_from_vertical(cv::Point2f(0.f, -globals.velocity.y), globals.gravity, summit.x - descending_image.rows);
					if (collision.y + collision.w <= summit.y + horizontal_delta) process_descending = false; // Descending specific: the platform is farther left than the jump can reach.
				}

				if (process_ascending) {
					cv::Point   ll_corner(collision.x + collision.h - 1, collision.y - 1);
					cv::Point2f ll_velocity = ascending_velocity(platform_pixel, ll_corner, cv::Point2f(globals.velocity.x, -globals.velocity.y), globals.gravity);
					cv::Point2f ll_summit   = ascending_summit(ll_corner, ll_velocity, globals.gravity);

					cv::Point   ur_corner(__max(collision.x, summit.x), collision.y + collision.w);
					cv::Point2f ur_velocity = ascending_velocity(platform_pixel, ur_corner, cv::Point2f(globals.velocity.x, -globals.velocity.y), globals.gravity);
					cv::Point2f ur_summit   = ascending_summit(ur_corner, ur_velocity, globals.gravity);

					if (ur_velocity.y >= -globals.velocity.y) {
						int start_x = __max(ur_summit.x, ll_summit.x); // Get rid of floating-point errors.
						start_x = __max(start_x, 0); // if jump is higher than the top of the image.
						for (int x = start_x; x <= ll_corner.x; x++) {
							float right = ur_summit.y - horizontal_from_vertical(cv::Point2f(0.f, ur_velocity.y), globals.gravity, ur_summit.x - x) - 1;
							float left  = ll_summit.y - horizontal_from_vertical(cv::Point2f(0.f, ll_velocity.y), globals.gravity, ll_summit.x - x) + 1;
							right = __min(__max(right, 0) /* if the jump is farther left than the image. */, platform_pixel.y);
							left  = __max(left, 0); // If the platform is at the same level as the jump origin (inf otherwise).

							for (int y = left; y <= right; y++)
								ascending_image.at<uchar>(x, y) = 0;
						}

						for (int x = start_x; x < ascending_image.rows; x++) {
							float right = ur_corner.y - horizontal_from_vertical(cv::Point2f(0.f, ur_velocity.y), globals.gravity, ur_summit.x - x) - 1;
							float left = ll_corner.y - horizontal_from_vertical(cv::Point2f(0.f, ll_velocity.y), globals.gravity, ll_summit.x - x) + 1;
							right = __min(__max(right, 0), descending_image.cols); // If the jump is farther right than the image.
							left = __min(__max(left, 0), descending_image.cols - 1); // If the jump is farther right than the image.

							for (int y = left; y < right; y++)
								descending_image.at<uchar>(x, y) = 0;
						}
					}
				}

				if (process_descending) {
					cv::Point ul_corner(collision.x, collision.y - 1);
					cv::Point2f ul_velocity = descending_velocity(platform_pixel, ul_corner, cv::Point2f(globals.velocity.x, -globals.velocity.y), globals.gravity);

					cv::Point lr_corner(collision.x + collision.h - 1, collision.y + collision.w);
					cv::Point2f lr_velocity = descending_velocity(platform_pixel, lr_corner, cv::Point2f(globals.velocity.x, -globals.velocity.y), globals.gravity);

					for (int x = ul_corner.x; x < descending_image.rows; x++) {
						float right = lr_corner.y + horizontal_from_vertical(lr_velocity, globals.gravity, __min(lr_corner.x - x, 0)) - 1;
						float left = ul_corner.y + horizontal_from_vertical(ul_velocity, globals.gravity, ul_corner.x - x) + 1;
						right = __min(__max(right, 0), descending_image.cols); // If the jump is farther left than the image.
						left = __min(__max(left, 0), descending_image.cols); // If the jump is farther left than the image.

						for (int y = left; y < right; y++)
							descending_image.at<uchar>(x, y) = 0;
					}
				}
			}

			/* Merge both ascending and descending parts into one image. */ { 
				for (int x = topLeftCornerLeft.x; x <= bottomRightCornerLeft.x; x++)
					for (int y = topLeftCornerLeft.y; y <= bottomRightCornerLeft.y; y++)
						jump_left_image.at<uchar>(x, y) += ascending_image.at<uchar>(x, y) + descending_image.at<uchar>(x, y);
			}
		}

		int start_x = (topLeftCornerLeft.x < topLeftCornerRight.x) ? topLeftCornerLeft.x : topLeftCornerRight.x;
		int end_x = (bottomRightCornerLeft.x > bottomRightCornerRight.x) ? bottomRightCornerLeft.x : bottomRightCornerRight.x;

		int start_y = (topLeftCornerLeft.y < topLeftCornerRight.y) ? topLeftCornerLeft.y : topLeftCornerRight.y;
		int end_y = (bottomRightCornerLeft.y > bottomRightCornerRight.y) ? bottomRightCornerLeft.y : bottomRightCornerRight.y;
		
		for (int x = start_x; x <= end_x; x++)
			for (int y = start_y; y <= end_y; y++)
				reach_image.at<int>(x, y) += (jump_right_image.at<uchar>(x, y) || jump_left_image.at<uchar>(x, y));

		printf("%i\n", count++);
	} 


	/* Prints out the reach data image with values of EITHER 0 or 255 (aka no gradient). */ {
		cv::Mat reach_filled_image(dim_image.first, dim_image.second, CV_8UC1);
		for (int x = 0; x < reach_filled_image.rows; x++)
			for (int y = 0; y < reach_filled_image.cols; y++)
				reach_filled_image.at<uchar>(x, y) = 255 * (reach_image.at<int>(x, y) != 0);
		cv::imwrite(base_path + "\\reach_filled.png", reach_filled_image);
	}

	/* Create the visual of the result and save them inside a txt file.*/ {
		cv::Mat reach_normalized_image(dim_image.first, dim_image.second, CV_8UC1);
		int max = 0;
		for (int x = 0; x < reach_normalized_image.rows; x++)
			for (int y = 0; y < reach_normalized_image.cols; y++)
				max = __max(max, reach_image.at<int>(x, y));
		for (int x = 0; x < reach_normalized_image.rows; x++)
			for (int y = 0; y < reach_normalized_image.cols; y++)
				reach_normalized_image.at<uchar>(x, y) = 255 * reach_image.at<int>(x, y) / max;
		cv::imwrite(base_path + "\\reach_normalised.png", reach_normalized_image);

		cv::Mat danger_image = cv::imread(base_path + "\\danger_filled.png", cv::IMREAD_GRAYSCALE);
		cv::Mat collision_filled_image = cv::imread(base_path + "\\collision_filled.png", cv::IMREAD_GRAYSCALE);
		cv::Mat showcase_image(dim_image.first, dim_image.second, CV_8UC3);
		for (int x = 0; x < showcase_image.rows; x++)
			for (int y = 0; y < showcase_image.cols; y++) {
				showcase_image.at<cv::Vec3b>(x, y)[0] = collision_filled_image.at<uchar>(x, y);
				showcase_image.at<cv::Vec3b>(x, y)[1] = reach_normalized_image.at<uchar>(x, y); 
				showcase_image.at<cv::Vec3b>(x, y)[2] = danger_image.at<uchar>(x, y); 
			}
		cv::imwrite(base_path + "\\showcase.png", showcase_image);

		std::ofstream result_file(base_path + "\\results.txt", std::ios::trunc);
		result_file << "Filled Area Metric:	       " << metric_area_filled(reach_image, danger_image,0, reach_image.cols) << '\n';
		result_file << "Gradient Area Metric:      " << metric_area_gradient(reach_image, platform_pixels.size(), danger_image) << '\n';
		result_file << "Filled Perimeter Metric:   " << metric_perimeter_filled(reach_image, danger_image) << '\n';
		result_file << "Gradient Perimeter Metric: " << metric_perimeter_gradient(reach_image, platform_pixels.size(), danger_image) << '\n';
		result_file << "Execution time:  " << clock() - start_time;
		result_file.close();
		
		/*Create the result for a part*/
		{
			std::ofstream file(base_path + "\\graph.txt", std::ios::trunc);
			int window_width = 200;
			int step_y = 16;
			for (int end_y = window_width; end_y < reach_image.cols; end_y += step_y)
			{
				float metric = metric_area_filled(reach_image, danger_image, end_y - window_width, end_y);
				file << end_y << " | " << metric << '\n';
			}
			file.close();
		}
	}

	return 0;
}