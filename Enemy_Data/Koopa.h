#pragma once
#ifndef KOOPA_H

#include "EnemyBase.h"

class Koopa : public Enemy_Base
{
public:
	Koopa();
	~Koopa();
	Koopa(int x, int y, int height, int width, std::string type = "koopa");
	void Move(std::map<std::pair<int, int>, int>& nb_pos_enemy, std::vector<std::pair<int, int>> list_pixel) override;
	void GenerateImage(std::map<std::pair<int, int>, int> nb_pos_enemy, cv::Mat Image, int height, int width) override;

};

Koopa::Koopa() {
}

Koopa::Koopa(int x, int y, int height, int width, std::string type = "koopa") : Enemy_Base(x, y, height, width, type) {
}

Koopa::~Koopa() {

}

#endif // !KOOPA_H

