#include "EnemyBase.h"

void Enemy_Base::get_Json(std::string type, std::vector<Enemy_Base>& liste_ennemy, json j) {
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == "hammer_bro") {
			liste_ennemy.push_back(Enemy_Base(j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>()));
		}
	}
}