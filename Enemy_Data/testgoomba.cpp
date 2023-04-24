#include "general.h"


using namespace jsoncons;

// On passe en argument le chemin vers le fichier JSON

void create_metric(std::string base_path, std::string level_path, bool create_images) {
	// On r�cup�re le chemin vers le JSON 
	std::string json_path = base_path + level_path + "\\level.json";
	std::string image_path = base_path + level_path + "\\level.png";
	std::string metric_path = base_path + level_path + "\\Metric";
	if (!std::filesystem::is_directory(base_path + level_path + "\\Metric") || !std::filesystem::exists(base_path + level_path + "\\Metric")) { // Check if Metric folder exists
		std::filesystem::create_directory(base_path + level_path + "\\Metric"); // create Metric folder
	}
	if (!std::filesystem::is_directory(base_path + level_path + "\\Metric") || !std::filesystem::exists(base_path + level_path + "\\Metric")) { // Check if Metric folder exists
		std::filesystem::create_directory(base_path + level_path + "\\Metric"); // create Metric folder
	}

	cv::Mat level_image = cv::imread(image_path);
	cv::Mat reach_filled_image = cv::imread(base_path + level_path + "\\reach_filled.png", cv::IMREAD_GRAYSCALE);

	// On parse le JSON et on r�cup�re les valeurs du nombre de lignes/colonnes du niveau

	std::ifstream is(json_path);
	json j = json::parse(is);

	float ymax = j["levelCols"].as<float>();
	float xmax = j["levelRows"].as<float>();

	// On d�finit les constantes physiques
	std::pair<int,int> velocity_goomba(30, 10);
	int gravity = 5;

	// On lit et on stocke la liste des ennemis depuis le JSON

	std::vector<enemy> list_enemy_goomba;
	std::vector<enemy> list_enemy_koopa;
	std::vector<enemy> list_enemy_flying_koopa;
	std::vector<enemy> list_enemy_piranha_plant;
	std::vector<enemy> list_enemy_hammer_bro;
	std::vector<enemy> list_enemy_bowser;
	std::vector<enemy> list_enemy_flying_fish;
	std::vector<enemy> list_enemy_lakitu;
	std::vector<enemy> list_enemy_turtle_spike;
	std::vector<enemy> list_enemy_turtle;

	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "goomba") {
			list_enemy_goomba.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(), 
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>()});
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "koopa") {
			list_enemy_koopa.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "flying_koopa") {
			list_enemy_flying_koopa.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "piranha_plant") {
			list_enemy_piranha_plant.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "hammer_bro") {
			list_enemy_hammer_bro.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "bowser") {
			list_enemy_bowser.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "flying_fish") {
			list_enemy_flying_fish.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "lakitu") {
			list_enemy_lakitu.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "turtle_spike") {
			list_enemy_turtle_spike.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
		}
	}
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "turtle") {
			list_enemy_turtle.push_back(enemy{ j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>() });
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

	// Maintenant qu'on a la position des goombas et des collisions, on peut g�rer le d�placement des goombas

	//std::vector<std::pair<int, int>> list_position_goomba;
	int nb_deplacement = 250;
	std::map<std::pair<int, int>, int> nb_pos_goomba;
	std::map<std::pair<int, int>, int> nb_pos_koopa;
	std::map<std::pair<int, int>, int> nb_pos_flying_koopa;
	std::map<std::pair<int, int>, int> nb_pos_piranha_plant;
	std::map<std::pair<int, int>, int> nb_pos_hammer_bro;
	std::map<std::pair<int, int>, int> nb_pos_bowser;
	std::map<std::pair<int, int>, int> nb_pos_flying_fish;
	std::map<std::pair<int, int>, int> nb_pos_lakitu;
	std::map<std::pair<int, int>, int> nb_pos_turtle_spike;
	std::map<std::pair<int, int>, int> nb_pos_turtle; 

	int nb_frames_wait_piranha_plant = 60;
	int nb_frames_deplacement_piranha_plant = 0;

	std::cout << "Je suis bien la" << std::endl;
	for (int i = 0; i < nb_deplacement; i++) {
		std::cout << "J'arrive dans la boucle" << std::endl;
		bool didPiranhaPlantMove = false;
		bool didPiranhaPlantWait = false;

		cv::Mat Image_Final(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
		
		// On traite d'abord les goombas (Marron -> Orange sur l'image) (Koopa -> Vert / Plante piranha -> Rouge / Fr�re Marteau -> Bleu / Lakitu -> Blanc / Bowser -> Blanc / Poisson volant -> / Turtle Spike -> Gris)
		std::cout << "Goomba !" << std::endl;
		std::cout << list_enemy_goomba.size() << std::endl;
		list_enemy_goomba.push_back(enemy{0, 0, 0, 0, "", false});
		std::cout << list_enemy_goomba.size() << std::endl;
		for (enemy &ene : list_enemy_goomba) {
			if (nb_pos_goomba.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos_goomba.end()) {
				nb_pos_goomba.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos_goomba[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			// Si le goomba se d�place vers la gauche
			if (ene.isWalkingLeft) {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer auquel cas on fait demi tour

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1; 
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 17, ene.y + 16)) == list_pixel.end()) {
					ene.x += 5;
					ene.y -= 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y -= 1;
				}

			}
			// S'il se d�place vers la droite 
			else {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// On check maintenant si y a pas un trou sous lui

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 17, ene.y)) == list_pixel.end()) {
					ene.x += 5;
					ene.y += 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y += 1;
				}

			}
		}
		std::cout << "Koopa !" << std::endl;
		for (enemy& ene : list_enemy_koopa) {
			if (nb_pos_koopa.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos_koopa.end()) {
				nb_pos_koopa.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos_koopa[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			// Si le koopa se d�place vers la gauche
			if (ene.isWalkingLeft) {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y)) == list_pixel.end()) {
					ene.x += 5;
					ene.y -= 1;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas il faudra faire demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y-1)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}
				
				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y -= 1;
				}

			}
			// S'il se d�place vers la droite 
			else {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 16)) == list_pixel.end()) {
					ene.y += 1;
					ene.x += 5;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas on fait demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 17)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y += 1;
				}

			}
		}
		std::cout << "Turtle spike !" << std::endl;
		for (enemy& ene : list_enemy_turtle_spike) {
			if (nb_pos_turtle_spike.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos_turtle_spike.end()) {
				nb_pos_turtle_spike.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos_turtle_spike[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			// Si le koopa se d�place vers la gauche
			if (ene.isWalkingLeft) {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y)) == list_pixel.end()) {
					ene.x += 5;
					ene.y -= 1;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas il faudra faire demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y - 1)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y -= 1;
				}

			}
			// S'il se d�place vers la droite 
			else {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 16)) == list_pixel.end()) {
					ene.y += 1;
					ene.x += 5;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas on fait demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 17)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y += 1;
				}

			}
		}
		std::cout << "Turtle !" << std::endl;
		for (enemy& ene : list_enemy_turtle) {
			if (nb_pos_turtle.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos_turtle.end()) {
				nb_pos_turtle.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos_turtle[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			// Si le koopa se d�place vers la gauche
			if (ene.isWalkingLeft) {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y - 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y)) == list_pixel.end()) {
					ene.x += 5;
					ene.y -= 1;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas il faudra faire demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y - 1)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y += 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y -= 1;
				}

			}
			// S'il se d�place vers la droite 
			else {

				// On check d'abord si y a un tuyau qui l'emp�che d'avancer

				if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x, ene.y + 1)) != list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// On check maintenant si y a pas un trou sous lui auquel cas il tombe (cas où l'ennemi est au dessus du vide au début)

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 16)) == list_pixel.end()) {
					ene.y += 1;
					ene.x += 5;
				}

				// On check si le prochain pas va l'emmener au dessus du vide, auquel cas on fait demi-tour

				else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(ene.x + 24, ene.y + 17)) == list_pixel.end()) {
					ene.isWalkingLeft = !ene.isWalkingLeft;
					ene.y -= 1;
				}

				// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

				else {
					ene.y += 1;
				}

			}
		}
		std::cout << "Piranha !" << std::endl;
		for (enemy& ene : list_enemy_piranha_plant) {
			if (nb_pos_piranha_plant.find(std::pair<int, int>(ene.x, ene.y)) == nb_pos_piranha_plant.end()) {
				nb_pos_piranha_plant.insert({ std::pair<int, int>(ene.x, ene.y), 1 });
			}
			else {
				nb_pos_piranha_plant[std::pair<int, int>(ene.x, ene.y)] += 1;
			}
			if (nb_frames_wait_piranha_plant != 0) {
				didPiranhaPlantWait = true;
			}
			else if (nb_frames_deplacement_piranha_plant != ene.height && ene.isWalkingLeft) {
				ene.x += 1;	
				didPiranhaPlantMove = true;
			}
			else if (nb_frames_deplacement_piranha_plant != ene.height && !ene.isWalkingLeft) {
				ene.x -= 1;		
				didPiranhaPlantMove = true;
			}
			else {
				ene.isWalkingLeft = !ene.isWalkingLeft;			
			}			
		}
		if (didPiranhaPlantWait) {
			nb_frames_wait_piranha_plant -= 1;
		}		
		else if (didPiranhaPlantMove) {
			nb_frames_deplacement_piranha_plant += 1;
		}
		else {
			nb_frames_wait_piranha_plant = 60;
			nb_frames_deplacement_piranha_plant = 0;
		}
		
		std::cout << " Avant creation images !" << std::endl;
		if (create_images) {
			std::map<std::pair<int, int>, int>::iterator it_goomba;
			for (it_goomba = nb_pos_goomba.begin(); it_goomba != nb_pos_goomba.end(); it_goomba++) {
				cv::Rect rect(it_goomba->first.second, it_goomba->first.first, 16, 16);
				cv::rectangle(Image_Final, rect,
					cv::Scalar(0, 29 + it_goomba->second * 2, 58 + it_goomba->second * 2),
					cv::FILLED); //BGR
			}

			std::map<std::pair<int, int>, int>::iterator it_turtle_spike;
			for (it_turtle_spike = nb_pos_turtle_spike.begin(); it_turtle_spike != nb_pos_turtle_spike.end(); it_turtle_spike++) {
				cv::Rect rect(it_turtle_spike->first.second, it_turtle_spike->first.first, 15, 16);
				cv::rectangle(Image_Final, rect,
					cv::Scalar(0, 0, 40 + it_turtle_spike->second * 2),
					cv::FILLED); //BGR
			}

			std::map<std::pair<int, int>, int>::iterator it_turtle;
			for (it_turtle = nb_pos_turtle.begin(); it_turtle != nb_pos_turtle.end(); it_turtle++) {
				cv::Rect rect(it_turtle->first.second, it_turtle->first.first, 15, 16);
				cv::rectangle(Image_Final, rect,
					cv::Scalar(20 + it_turtle->second * 2, 20 + it_turtle->second * 2, 20 + it_turtle->second * 2),
					cv::FILLED); //BGR
			}


			std::map<std::pair<int, int>, int>::iterator it_koopa;
			for (it_koopa = nb_pos_koopa.begin(); it_koopa != nb_pos_koopa.end(); it_koopa++) {
				cv::Rect rect(it_koopa->first.second, it_koopa->first.first, 23, 16);
				cv::rectangle(Image_Final, rect,
					cv::Scalar(0, 20 + it_koopa->second * 3, 0),
					cv::FILLED); //BGR
			}

			std::map<std::pair<int, int>, int>::iterator it_piranha_plant;
			for (it_piranha_plant = nb_pos_piranha_plant.begin(); it_piranha_plant != nb_pos_piranha_plant.end(); it_piranha_plant++) {
				cv::Rect rect(it_piranha_plant->first.second, it_piranha_plant->first.first, 22, 16);
				cv::rectangle(Image_Final, rect,
					cv::Scalar(20 + it_piranha_plant->second , 0, 20 + it_piranha_plant->second),
					cv::FILLED); //BGR
			}

			std::string num_fichier = std::to_string(i);
			std::string filename = "\\move_enemies" + num_fichier + ".png";
			cv::imwrite(base_path + level_path + "\\Images_move_enemies" + filename, Image_Final);
		}
	}

	cv::Mat Image_Final_Temp(level_image.rows, level_image.cols, level_image.type(), cv::Scalar(0, 0, 0));
	std::cout << " je suis bien ici !" << std::endl;
	std::map<std::pair<int, int>, int>::iterator it_goomba;
	for (it_goomba = nb_pos_goomba.begin(); it_goomba != nb_pos_goomba.end(); it_goomba++) {
		cv::Rect rect(it_goomba->first.second, it_goomba->first.first, 16, 16);
		cv::rectangle(Image_Final_Temp, rect,
			cv::Scalar(0, 29 + it_goomba->second * 2, 58 + it_goomba->second * 2),
			cv::FILLED); //BGR
	}

	std::map<std::pair<int, int>, int>::iterator it_turtle_spike;
	for (it_turtle_spike = nb_pos_turtle_spike.begin(); it_turtle_spike != nb_pos_turtle_spike.end(); it_turtle_spike++) {
		cv::Rect rect(it_turtle_spike->first.second, it_turtle_spike->first.first, 15, 16);
		cv::rectangle(Image_Final_Temp, rect,
			cv::Scalar(0, 0, 40 + it_turtle_spike->second * 2),
			cv::FILLED); //BGR
	}

	std::map<std::pair<int, int>, int>::iterator it_turtle;
	for (it_turtle = nb_pos_turtle.begin(); it_turtle != nb_pos_turtle.end(); it_turtle++) {
		cv::Rect rect(it_turtle->first.second, it_turtle->first.first, 15, 16);
		cv::rectangle(Image_Final_Temp, rect,
			cv::Scalar(20 + it_turtle->second * 2, 20 + it_turtle->second * 2, 20 + it_turtle->second * 2),
			cv::FILLED); //BGR
	}


	std::map<std::pair<int, int>, int>::iterator it_koopa;
	for (it_koopa = nb_pos_koopa.begin(); it_koopa != nb_pos_koopa.end(); it_koopa++) {
		cv::Rect rect(it_koopa->first.second, it_koopa->first.first, 23, 16);
		cv::rectangle(Image_Final_Temp, rect,
			cv::Scalar(0, 20 + it_koopa->second * 3, 0),
			cv::FILLED); //BGR
	}

	std::map<std::pair<int, int>, int>::iterator it_piranha_plant;
	for (it_piranha_plant = nb_pos_piranha_plant.begin(); it_piranha_plant != nb_pos_piranha_plant.end(); it_piranha_plant++) {
		cv::Rect rect(it_piranha_plant->first.second, it_piranha_plant->first.first, 22, 16);
		cv::rectangle(Image_Final_Temp, rect,
			cv::Scalar(20 + it_piranha_plant->second * 2, 0, 20 + it_piranha_plant->second * 2),
			cv::FILLED); //BGR
	}

	std::string num_fichier = std::to_string(0);
	std::string filename = "\\move_enemies" + num_fichier + ".png";
	cv::imwrite(base_path + level_path + "\\Images_move_enemies" + filename, Image_Final_Temp);

	// Calcul m�trique

	int window_width = 200;
	int step_y = 16;
	float metric = 0.f;
	float metric_global = 0.0f;
	points_array points_goomba;
	points_array points_koopa;
	points_array points_turtle_spike;
	points_array points_turtle;
	points_array points_piranha_plant;
	points_array points_globaux;
	

	for (int end_y = window_width; end_y < reach_filled_image.cols; end_y += step_y)
	{
		metric_global = 0;
		metric = metric_area_filled(reach_filled_image, end_y - window_width, end_y, nb_pos_goomba, 16);
		metric_global += metric;
		points_goomba.emplace_back(point(end_y, metric));

		metric = metric_area_filled(reach_filled_image, end_y - window_width, end_y, nb_pos_koopa, 23);
		metric_global += metric;
		points_koopa.emplace_back(point(end_y, metric));

		metric = metric_area_filled(reach_filled_image, end_y - window_width, end_y, nb_pos_turtle_spike, 15);
		metric_global += metric;
		points_turtle_spike.emplace_back(point(end_y, metric));

		metric = metric_area_filled(reach_filled_image, end_y - window_width, end_y, nb_pos_turtle, 15);
		metric_global += metric;
		points_turtle.emplace_back(point(end_y, metric));

		metric = metric_area_filled(reach_filled_image, end_y - window_width, end_y, nb_pos_piranha_plant, 22);
		metric_global += metric;
		points_piranha_plant.emplace_back(point(end_y, metric));

		points_globaux.emplace_back(point(end_y, metric_global));
	}
	create_graph(points_goomba, metric_path + "\\metric_goomba.png");
	create_graph(points_koopa, metric_path + "\\metric_koopa.png");
	create_graph(points_turtle_spike, metric_path + "\\metric_turtle_spike.png");
	create_graph(points_turtle, metric_path + "\\metric_turtle.png");
	create_graph(points_piranha_plant, metric_path + "\\metric_piranha_plant.png");
	std::vector<points_array> tab;
	std::vector<cv::Scalar> colors;
	tab.emplace_back(points_goomba);
	colors.emplace_back(cv::Scalar(0,75,150));
	tab.emplace_back(points_koopa);
	colors.emplace_back(cv::Scalar(34,139,34));
	tab.emplace_back(points_turtle_spike);
	colors.emplace_back(cv::Scalar(0, 0, 255));
	tab.emplace_back(points_turtle);
	colors.emplace_back(cv::Scalar(100, 100, 100));
	tab.emplace_back(points_piranha_plant);
	colors.emplace_back(cv::Scalar(255, 0, 255));
	tab.emplace_back(points_globaux);
	colors.emplace_back(cv::Scalar(255,255,255));
	create_graph(tab, colors, metric_path + "\\metric_enemy_global.png");
}