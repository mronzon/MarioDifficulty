#include "collisions.h"

void propagate(const std::vector<collision_t>& collision_merged, cv::Mat& danger_gradient_temp, const cv::Point& point) {
    if (point.x < 0 || point.y < 0 || point.y >= danger_gradient_temp.cols) return;
    for (auto collision: collision_merged)
    {
        if(is_inside(collision, point.x, point.y))
        {
            return;
        }
    }
    if (danger_gradient_temp.at<uchar>(point.x, point.y) != 0) return;

    danger_gradient_temp.at<uchar>(point.x, point.y) = 255;
    propagate(collision_merged, danger_gradient_temp, cv::Point(point.x - 1, point.y - 1));
    propagate(collision_merged, danger_gradient_temp, cv::Point(point.x - 1, point.y));
    propagate(collision_merged, danger_gradient_temp, cv::Point(point.x - 1, point.y + 1));

}

int main(int argc, char* argv[]) {
    if (argc != 3) return 0;
    std::string const base_path = argv[1];
    std::string const folder_path = base_path + "\\" + argv[2];
    std::tuple<int, int> dim;

    //Get the dimensions of the image.
    if(!get_dimension(dim, folder_path + "\\level.json"))
    {
        return -2;
    }
    cv::Mat danger_filled(std::get<0>(dim), std::get<1>(dim), CV_8UC1, cv::Scalar(0));
    
    // Get collisions from the json file.
    std::vector<collision_t> collisions_merged;
    if (!read_collision(collisions_merged, folder_path + "\\level.json")) { // If the json file is not here !
        return -2;
    }

    
    for (int y = 0; y < danger_filled.cols; y++) {
        propagate(collisions_merged, danger_filled, cv::Point(danger_filled.rows - 1, y));
        printf("progress...%i%%\n", 100 * (y + 1) / danger_filled.cols);
    }
    cv::imwrite(folder_path + "\\danger_filled.png", danger_filled);

    return 0;
}