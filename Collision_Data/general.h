#pragma once
#ifndef GENERAL_COLLISION_H
#define GENERAL_COLLISION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <vector>
#include <map>

#include <jsoncons/json.hpp>

using namespace jsoncons;

int create_json(std::string const& base_path, std::string const& level_path);
#endif