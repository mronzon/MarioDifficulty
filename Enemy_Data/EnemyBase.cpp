#include "EnemyBase.h"

template <typename T> void Enemy_Base::get_Json(std::string type, std::vector<T>& liste_ennemy, json j) {
	for (int i = 0; i < j["dynamic"]["enemies"].size(); i++) {
		if (j["dynamic"]["enemies"][i]["type"] == type) {
			liste_ennemy.push_back(T(j["dynamic"]["enemies"][i]["x"].as<int>(), j["dynamic"]["enemies"][i]["y"].as<int>(),
				j["dynamic"]["enemies"][i]["height"].as<int>(), j["dynamic"]["enemies"][i]["width"].as<int>(), j["dynamic"]["enemies"][i]["type"].as<std::string>()));
		}
	}
}