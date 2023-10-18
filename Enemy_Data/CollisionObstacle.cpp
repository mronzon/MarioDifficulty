#include "CollisionObstacle.h"

CollisionObstacle::CollisionObstacle(int posX, int posY, int width, int height)
{
	this->posX = posX;
	this->posY = posY;
	this->width = width;
	this->height = height;
}

CollisionObstacle::~CollisionObstacle()
{
}
