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
    float metric_static;

    if (!std::filesystem::is_directory(save_path) || !std::filesystem::exists(save_path)) { 
        std::filesystem::create_directory(save_path); 
    }
    

    for(int window_width = 16; window_width <= 320; window_width += 16)
    {
        std::ofstream graph_area_filed(save_path + "\\" + std::to_string(window_width) + " static.txt", std::ios::trunc);
        std::ofstream graph_total(save_path + "\\" + std::to_string(window_width) + " total.txt", std::ios::trunc);
        std::ifstream txt_metric_enemy;
        std::vector<float> valeur_metric_enemy;
        float metric_enemy;
        std::string tmp;
        int i = 0;
        txt_metric_enemy.open(save_path + "\\" + std::to_string(window_width) + " enemy.txt");
        if (!txt_metric_enemy) {
            std::cout << "On a pas ouvert le txt metric enemy" << std::endl;
            exit(1);   
        }
        else {
            std::cout << "On vient d'ouvrir " << std::to_string(window_width) + " enemy.txt" << std::endl;
        }
        while (txt_metric_enemy) {
            std::getline(txt_metric_enemy, tmp);
            metric_enemy = std::stof(tmp);
            valeur_metric_enemy.emplace_back(metric_enemy);
        }
        std::cout << "On a fini de lire le .txt enemy" << std::endl;
        points_array points;
        points_array points_finaux;
        for (int end_y = window_width; end_y < reach_filled.cols; end_y += step_y)
        {
            metric_static = metric_area_filled(reach_filled, danger_filled, end_y - window_width, end_y);
            graph_area_filed << end_y << " | " << metric_static << '\n';
            metric_enemy = valeur_metric_enemy[i];
            graph_total << end_y << " | " << metric_static + metric_enemy << '\n';           
            points.emplace_back(point(end_y, metric_static));
            points_finaux.emplace_back(point(end_y, metric_static + metric_enemy));
            i++;
        }
        std::cout << "On a fini de remplir le txt total" << std::endl;
        
        float Somme_Int = 0;
        float Somme_Int_total = 0;
        for (int j = 0; j < points.size() - 1; j++) {
            Somme_Int += (0.5f) * (points[j + 1].first - points[j].first) * (points[j + 1].second + points[j].second);
            Somme_Int_total += (0.5f) * (points_finaux[j + 1].first - points_finaux[j].first) * (points_finaux[j + 1].second + points_finaux[j].second);
        }

        std::cout << "On a fini de calculer les integrales" << std::endl;

        graph_area_filed << "Valeur de l'integrale : " << Somme_Int << std::endl;
        graph_total << "Valeur de l'integrale : " << Somme_Int_total << std::endl;

        graph_area_filed.close();
        txt_metric_enemy.close();
        graph_total.close();

        create_graph(points, save_path + "\\" + std::to_string(window_width) + " static.png");
        create_graph(points_finaux, save_path + "\\" + std::to_string(window_width) + " total.png");

        std::cout << "Graph creer pour une fenetre de taille " + std::to_string(window_width) << std::endl;
    }
}
