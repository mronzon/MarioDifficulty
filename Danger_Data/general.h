#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <jsoncons/json.hpp>

using namespace jsoncons;

#include <cstdio>
#include <vector>
#include <filesystem>
#include <fstream>

#ifndef NDEBUG
#define dbg_printf(format, ...) printf(format, __VA_ARGS__)
#define dbg_imwrite(path, img) cv::imwrite((path), (img)) 
#else
#define dbg_printf(format, ...) (void(0))
#define dbg_imwrite(path, img) (void(0))
#endif // !NDEBUG

bool inline get_dimension(std::tuple<int, int>& dim, std::string json_path)
{
    std::ifstream file(json_path.c_str());

    if (!file.is_open()) {
        dbg_printf("File \"%s\" could not be oppened for reading.\n", json_path.c_str());
        return false;
    }
    json json_content = json::parse(file);
    std::get<0>(dim) = json_content["levelRows"].as<int>();
    std::get<1>(dim) = json_content["levelCols"].as<int>();
    return true;
}

int create_danger(std::string const& folder_path);

#endif // !GENERAL_HPP
