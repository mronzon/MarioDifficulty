#pragma once
#ifndef ENNEMY_H
#define ENNEMY_H

#include <string>

typedef enum {
	goomba, koopa, flying_koopa, bowser, lakitu, turtle_spike, hammer_bro, piranha_plant
} type_enemy;

struct enemy {
	int x;
	int y;
	type_enemy type;
};



#endif //ENNEMY_H

