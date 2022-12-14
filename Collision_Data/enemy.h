#pragma once
#ifndef ENNEMY_H
#define ENNEMY_H

#include <string>
#include <map>

typedef enum {
	goomba, koopa, flying_koopa, bowser, lakitu, turtle_spike, hammer_bro, piranha_plant, flying_fish
} type_enemy;

struct enemy {
	int x;
	int y;
	int height;
	int width;
	std::string type;
};

static std::map<type_enemy, std::string> mapToConvertString = { {goomba,"goomba"},{koopa,"koopa"},{flying_koopa,"flying_koopa"},{bowser,"bowser"},{lakitu,"lakitu"},{turtle_spike,"turtle_spike"},{hammer_bro,"hammer_bro"},{piranha_plant,"piranha_plant"}, {flying_fish,"flying_fish"} };

std::vector<cv::Mat> inline find_textures_enemies(const std::string& path, std::map<int,type_enemy>* map)
{
	int i = 0;
	std::vector<cv::Mat> textures;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
		cv::Mat texture = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
		textures.push_back(texture);
		if (entry.path().filename().string().find("goomba") != std::string::npos) {
			map->insert({ i, goomba });
		}
		else if (entry.path().filename().string().find("hammer") != std::string::npos) {
			map->insert({ i, hammer_bro });
		}
		else if (entry.path().filename().string().find("volant") != std::string::npos) {
			map->insert({ i, flying_koopa });
		}
		else if (entry.path().filename().string().find("koopa") != std::string::npos) {
			map->insert({ i ,koopa });
		}
		else if (entry.path().filename().string().find("flying") != std::string::npos) {
			map->insert({ i ,flying_fish });
		}
		else if (entry.path().filename().string().find("lakitu") != std::string::npos) {
			map->insert({ i ,lakitu });
		}
		else if (entry.path().filename().string().find("piranha_plant") != std::string::npos) {
			map->insert({ i , piranha_plant });
		}
		else if (entry.path().filename().string().find("turtle") != std::string::npos) {
			map->insert({ i , turtle_spike });
		}
		i++;
	}
	return textures;
}

std::string convertToString(type_enemy tEnemy) {
	return mapToConvertString[tEnemy];
}


#endif //ENNEMY_H

