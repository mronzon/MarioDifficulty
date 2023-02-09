#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <jsoncons/json.hpp>

using namespace jsoncons;

#include <cstdio>
#include <vector>

#ifndef NDEBUG
#define dbg_printf(format, ...) printf(format, __VA_ARGS__)
#define dbg_imwrite(path, img) cv::imwrite((path), (img)) 
#else
#define dbg_printf(format, ...) (void(0))
#define dbg_imwrite(path, img) (void(0))
#endif // !NDEBUG

#endif // !GENERAL_HPP
