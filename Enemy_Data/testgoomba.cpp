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

	// On définit les constantes physiques
	std::pair<int,int> velocity_goomba(30, 10);
	int gravity = 5;

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

	//std::vector<std::pair<int, int>> list_position_goomba;
	int nb_deplacement = 2500;
	std::map<std::pair<int, int>, int> nb_pos;

	for (int i = 0; i < nb_deplacement; i++) {

		cv::Mat Image_Final(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		
		for (enemy &ene : list_enemy) {
			if (nb_pos.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos.end()) {
				nb_pos.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			// Si le goomba se déplace vers la gauche
			if (ene.isWalkingLeft) {

				// On check d'abord si y a un tuyau qui l'empêche d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1; 
				}
				else {
					ene.y -= 1;
				}

				// On check maintenant si y a pas un trou sous lui 

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 17, ene.y + 16)) != list_pixel.end()) {
					ene.y -= 1;
				}
				else {
					ene.x += 5;
					ene.y -= 1;
				}

			}
			// S'il se déplace vers la droite 
			else {

				// On check d'abord si y a un tuyau qui l'empêche d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}
				else {
					ene.y += 1;
				}

				// On check maintenant si y a pas un trou sous lui

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 17, ene.y)) != list_pixel.end()) {
					ene.y += 1;
				}
				else {
					ene.x += 5;
					ene.y += 1;
				}

			}
		}

		std::map<std::pair<int, int>, int>::iterator it;
		for (it = nb_pos.begin(); it != nb_pos.end(); it++) {
			cv::Rect rect(it->first.second, it->first.first, 16, 16);
			cv::rectangle(Image_Final, rect,
				cv::Scalar(0, 29+it->second*2, 58 + it->second * 5),
				cv::FILLED); //BGR

		}
		std::string test = std::to_string(i);
		std::string filename = "\\move_goomba_" + test + ".png";
		cv::imwrite(base_path + level_path + "\\Images_move_goomba" + filename, Image_Final);
	}

	return 0;
}