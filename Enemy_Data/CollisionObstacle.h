#pragma once
class CollisionObstacle
{
public:
	CollisionObstacle(int posX, int posY, int width, int height);
	~CollisionObstacle();

	int getPosX() {return posX;}

	int getPosY() {return posY;}

	int getWidth() {return width;}

	int getHeight() {return height;}

private:
	int posX;
	int posY;
	int width;
	int height;

protected:
};

