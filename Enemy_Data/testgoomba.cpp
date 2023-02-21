#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <fstream>
#include <vector>
#include <map>
#include <vector>
#include <utility>
#include <jsoncons/json.hpp>

#include "enemy.h"


using namespace jsoncons;

// On passe en argument le chemin vers le fichier JSON

int main(int argc, char* argv[]) {

	if (argc != 2) return 0;

	// On récupère le chemin vers le JSON 

	std::string base_path = argv[1];
	std::string level_path = "\\Niveau_1_1";
	std::string json_path = base_path + level_path + "\\level.json";
	std::string image_path = base_path + level_path + "\\level.png";
	cv::Mat level_image = cv::imread(image_path);

	std::cout << json_path << std::endl;

	// On parse le JSON et on récupère les valeurs du nombre de lignes/colonnes du niveau

	std::ifstream is(json_path);
	json j = json::parse(is);

	float ymax = j["levelCols"].as<float>();
	float xmax = j["levelRows"].as<float>();

	// On lit et on stocke la liste des ennemis depuis le JSON

	std::vector<enemy> list_enemy;

	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "goomba") {
			list_enemy.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(), 
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>()});
		}
	}

	// On lit et on stocke la liste des paires de pixel qui composent les collisions

	std::vector<std::pair<int, int>> list_pixel;

	for (int i = 0; i < j["static"]["pipes"].size(); i++) {
		int tmp_height = j["static"]["pipes"][i]["height"].as<int>();
		int tmp_width = j["static"]["pipes"][i]["width"].as<int>();
		int tmp_x = j["static"]["pipes"][i]["x"].as<int>();
		int tmp_y = j["static"]["pipes"][i]["y"].as<int>();
		for (int k = tmp_x; k < tmp_x + tmp_height; k++) {
			for (int l = tmp_y; l < tmp_y + tmp_width; l++) {
				std::pair<int, int> tmp_pair (k, l);
				list_pixel.push_back(tmp_pair);
			}
		}
	}

	for (int i = 0; i < j["static"]["positionCollision"].size(); i++) {
		int tmp_height = j["static"]["positionCollision"][i]["height"].as<int>();
		int tmp_width = j["static"]["positionCollision"][i]["width"].as<int>();
		int tmp_x = j["static"]["positionCollision"][i]["x"].as<int>();
		int tmp_y = j["static"]["positionCollision"][i]["y"].as<int>();
		for (int k = tmp_x; k < tmp_x + tmp_height; k++) {
			for (int l = tmp_y; l < tmp_y + tmp_width; l++) {
				std::pair<int, int> tmp_pair(k, l);
				list_pixel.push_back(tmp_pair);
			}
		}
	}

	// Maintenant qu'on a la position des goombas et des collisions, on peut gérer le déplacement des goombas

	std::vector<std::pair<int, int>> list_position_goomba;
	int nb_deplacement = 2500;

	for (int i = 0; i < nb_deplacement; i++) {
		for (enemy &ene : list_enemy) {
			list_position_goomba.push_back(std::pair<int, int> (ene.x, ene.y));
			// Si le goomba se déplace vers la gauche
			if (ene.isWalkingLeft) {
				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}
				else {
					ene.y -= 1;
				}
			}
			// S'il se déplace vers la droite 
			else {
				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}
				else {
					ene.y += 1;
				}
			}
		}
	}
	
	cv::Mat Image_Final(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
	for (const std::pair<int,int>& pos : list_position_goomba) {
		cv::Rect rect(pos.second, pos.first, 16, 16);
		cv::rectangle(Image_Final, rect, cv::Scalar(255, 255, 255), cv::FILLED);
	}
	cv::imwrite(base_path + level_path + "\\move_goomba.png", Image_Final);

	return 0;
}