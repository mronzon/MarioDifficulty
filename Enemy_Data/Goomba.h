#pragma once
#ifndef GOOMBA_H

#include "EnemyBase.h"

class Goomba : public Enemy_Base
{
public:
	Goomba();
	~Goomba();
	Goomba(int x, int y, int height, int width, std::string type = "goomba");
	void Move(std::map<std::pair<int, int>, int>& nb_pos_enemy, std::vector<std::pair<int, int>> list_pixel) override;
	void GenerateImage(std::map<std::pair<int, int>, int> nb_pos_enemy, cv::Mat Image, int height, int width) override;

};

Goomba::Goomba() {
}

Goomba::Goomba(int x, int y, int height, int width, std::string type = "goomba") : Enemy_Base(x, y, height, width, type) {
}

Goomba::~Goomba() {

}

#endif // !GOOMBA_H

