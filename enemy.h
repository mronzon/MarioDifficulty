#pragma once
#ifndef ENNEMY_H
#define ENNEMY_H

#include <string>
#include <map>

typedef enum {
	goomba, koopa, flying_koopa, bowser, lakitu, turtle_spike, hammer_bro, piranha_plant
} type_enemy;

struct enemy {
	int x;
	int y;
	int height;
	int width;
	type_enemy type;
};

std::vector<cv::Mat> inline find_textures_enemies(const std::string& path, std::map<cv::Mat,type_enemy>* map)
{
	std::cout << "On commence find_textures_enemies" << std::endl;
	std::vector<cv::Mat> textures;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
		std::cout << "On commence la boucle de find_textures_enemies" << std::endl;
		if (entry.path().extension().string() != ".png") continue;
		cv::Mat texture = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
		textures.push_back(texture);
		std::cout << "On a fini d'isoler la texture" << std::endl;
		if (entry.path().filename().string().find("goomba") != std::string::npos) {
			map->insert({ texture, goomba });
		}
		else if (entry.path().filename().string().find("hammer") != std::string::npos) {
			map->insert({ texture,hammer_bro });
		}
		else if (entry.path().filename().string().find("volant") != std::string::npos) {
			map->insert({ texture, flying_koopa });
		}
		else if (entry.path().filename().string().find("koopa") != std::string::npos) {
			map->insert({ texture,koopa });
		}
		std::cout << "On a inséré dans la map normalement" << std::endl;
	}
	return textures;
}


#endif //ENNEMY_H

