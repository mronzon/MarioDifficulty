#pragma once
#ifndef GENERAL_ENEMIES_H
#define GENERAL_ENEMIES_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <fstream>
#include <vector>
#include <map>
#include <vector>
#include <utility>
#include <jsoncons/json.hpp>
#include <filesystem>

#include "enemy.h"
#include "graph.h"
#include "metric.h"

void create_metric(std::string base_path, std::string level_path, bool create_images);

#endif