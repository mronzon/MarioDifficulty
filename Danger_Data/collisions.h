#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "general.h"


#include <string>

#ifndef NDEBUG
#define dbg_write_img(rows, cols, collisions, file_path)\
write_collision_img((rows), (cols), (collisions), (file_path))
#else
#define dbg_write_img(rows, cols, collisions, file_path) (void(0))
#endif // !NDEBUG

struct collision_t {
    int32_t x, y, h, w;
};


bool inline read_collision(
    std::vector<collision_t>& collisions,
    const std::string& file_path)
{
    std::ifstream file(file_path.c_str());

    if (!file.is_open()) {
        dbg_printf("File \"%s\" could not be oppened for reading.\n", file_path.c_str());
        return false;
    }
    json json_content = json::parse(file);
    int n = json_content["static"]["positionCollision"].size();
    collisions.resize(n * sizeof(collision_t));
    for(int i = 0; i < n ; i++)
    {
#pragma warning(suppress : 4996)
        collisions.push_back(collision_t{json_content["static"]["positionCollision"][i]["x"].as_int(), json_content["static"]["positionCollision"][i]["y"].as_int(), json_content["static"]["positionCollision"][i]["height"].as_int(), json_content["static"]["positionCollision"][i]["width"].as_int()});
    }
	
    return true;
}

bool inline collision_compare(const collision_t& a, const collision_t& b) {
    return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

void inline write_collision_img(
    int rows,
    int cols,
    const std::vector<collision_t>& collisions,
    const std::string& file_path)
{
    cv::Mat result(rows, cols, CV_8UC1, cv::Scalar(0));
    for (const collision_t& collision : collisions) {
        cv::Rect rect(collision.y, collision.x, collision.w, collision.h);
        cv::rectangle(result, rect, cv::Scalar(255));
    }
    cv::imwrite(file_path, result);
}

bool inline is_inside(const collision_t& collision, const int& x, const int& y)
{
    return((collision.x <= x && x <= collision.x + collision.h) && (collision.y <= y && y <= collision.y + collision.w));
}

#endif // !COLLISIONS_HPP
