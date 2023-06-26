#ifndef GENERAL_GRAPH
#define GENERAL_GRAPH

#include <string>
#include <filesystem>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include "../Reach_Data/graph.h"

void create_graphs(const std::string& folder_path, const std::string& level_name);


#endif