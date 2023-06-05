#include "Goomba.h"

void Goomba::Move(std::map<std::pair<int, int>, int>& nb_pos_enemy, std::vector<std::pair<int, int>> list_pixel) {
	if (nb_pos_enemy.find(std::pair<int, int>(get_x(), get_y())) == nb_pos_enemy.end()) {
		nb_pos_enemy.insert({ std::pair<int, int>(get_x(), get_y()), 1});
	}
	else {
		nb_pos_enemy[std::pair<int, int>(get_x(), get_y())] += 1;
	}
	// Si le goomba se d�place vers la gauche
	if (get_isWalkingLeft()) {

		// On check d'abord si y a un tuyau qui l'emp�che d'avancer auquel cas on fait demi tour

		if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(get_x(), get_y() - 1)) != list_pixel.end()) {
			set_isWalkingLeft(!get_isWalkingLeft());
			set_y(get_y() + 1);
		}

		// On check maintenant si y a pas un trou sous lui auquel cas il tombe

		else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(get_x() + 17, get_y() + 16)) == list_pixel.end()) {
			set_x(get_x() + 5);
			set_y(get_y() - 1);
		}

		// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

		else {
			set_y(get_y() - 1);
		}

	}
	// S'il se d�place vers la droite 
	else {

		// On check d'abord si y a un tuyau qui l'emp�che d'avancer

		if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(get_x(), get_y() + 1)) != list_pixel.end()) {
			set_isWalkingLeft(!get_isWalkingLeft());
			set_y(get_y() - 1);
		}

		// On check maintenant si y a pas un trou sous lui

		else if (std::find(list_pixel.begin(), list_pixel.end(), std::pair<int, int>(get_x() + 17, get_y())) == list_pixel.end()) {
			set_x(get_x() + 5);
			set_y(get_y() + 1);
		}

		// Si aucun de ces cas ne se réalisent, alors ça signifie qu'on peut avancer tranquillement

		else {
			set_y(get_y() + 1);
		}

	}

}

void Goomba::GenerateImage(std::map<std::pair<int, int>, int> nb_pos_enemy, cv::Mat Image, int height, int width) {
	std::map<std::pair<int, int>, int>::iterator it_goomba;
	for (it_goomba = nb_pos_enemy.begin(); it_goomba != nb_pos_enemy.end(); it_goomba++) {
		cv::Rect rect(it_goomba->first.second, it_goomba->first.first, height, width);
		cv::rectangle(Image, rect,
			cv::Scalar(0, 29 + it_goomba->second * 2, 58 + it_goomba->second * 2),
			cv::FILLED); //BGR
	}
}
