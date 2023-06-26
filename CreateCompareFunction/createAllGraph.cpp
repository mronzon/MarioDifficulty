#include "general.h"
#include <map>

float inline metric_area_filled(const cv::Mat& reach_filled, const cv::Mat& danger_filled, int start_y, int end_y) {
    int reach_count = 0;
    int effective_count = 0;

    for (int x = 0; x < reach_filled.rows; x++)
        for (int y = start_y; y < end_y; y++) {
            reach_count += (reach_filled.at<uchar>(x, y) != 0);
            effective_count += (reach_filled.at<uchar>(x, y) && danger_filled.at<uchar>(x, y));
        }

    return static_cast<float>(effective_count) / static_cast<float>(reach_count);
}

float inline metric_enemy(const cv::Mat& reach_filled, int start_y, int end_y, std::map<std::pair<int, int>, int>& enemy_map, float surface) {
    int reach_count = 0;
    int effective_count = 0;

    for (int x = 0; x < reach_filled.rows; x++)
        for (int y = start_y; y < end_y; y++) {
            reach_count += (reach_filled.at<uchar>(x, y) != 0);
            // si le pixel est dans reach + qu'il y a des pixels dessus
            if (reach_filled.at<uchar>(x, y) && enemy_map.find(std::pair<int, int>(x, y)) != enemy_map.end()) {
                effective_count += enemy_map[std::pair<int, int>(x, y)] * surface;
            }
        }

    return static_cast<float>(effective_count) / static_cast<float>(reach_count);
}

void create_graphs(const std::string& folder_path, const std::string& level_name)
{
    const std::string save_path = folder_path + "\\Graph" + level_name;
    const cv::Mat danger_filled = cv::imread(folder_path + level_name +"\\danger_filled.png", cv::IMREAD_GRAYSCALE);
    const cv::Mat reach_filled = cv::imread(folder_path + level_name + "\\reach_filled.png", cv::IMREAD_GRAYSCALE);
    const int step_y = 16;
    float metric;

    if (!std::filesystem::is_directory(save_path) || !std::filesystem::exists(save_path)) { 
        std::filesystem::create_directory(save_path); 
    }
    
    for(int window_width = 16; window_width <= 320; window_width += 16)
    {
        std::ofstream graph_area_filed(save_path + "\\" + std::to_string(window_width) + ".txt", std::ios::trunc);
        points_array points;
        for (int end_y = window_width; end_y < reach_filled.cols; end_y += step_y)
        {
            metric = metric_area_filled(reach_filled, danger_filled, end_y - window_width, end_y);
            graph_area_filed << end_y << " | " << metric << '\n';
            points.emplace_back(point(end_y, metric));
        }
        
        float Somme_Int = 0;
        for (int i = 0; i < points.size() - 1; i++) {
            /*std::cout << "b : " << points[i + 1].first << std::endl;
            std::cout << "a : " << points[i].first << std::endl;
            std::cout << "f(b) : " << points[i + 1].second << std::endl;
            std::cout << "f(a) : " << points[i].second << std::endl;*/
            Somme_Int += (0.5f) * (points[i + 1].first - points[i].first) * (points[i + 1].second + points[i].second);
            //std::cout << "Somme Int : " << Somme_Int << std::endl;
        }

        graph_area_filed << "Valeur de l'integrale : " << Somme_Int << std::endl;

        graph_area_filed.close();

        std::cout << "Integrale pour une fenetre de taille " + std::to_string(window_width) + " : " << Somme_Int << std::endl;


        create_graph(points, save_path + "\\" + std::to_string(window_width) + ".png");

        std::cout << "Graph creer pour une fenetre de taille " + std::to_string(window_width) << std::endl;
    }
}
