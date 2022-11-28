#pragma once

typedef enum
{
	horizontal, vertical
} direction;


struct platform_t
{
	int x;
	int y;
	int height;
	int width;
};

struct moving_lift_t
{
	int x;
	int y;
	int width;
	int height;
	direction axe;
	int limit[2];
	int initial_velocity;
};

struct balanced_lift_t
{
	int x_first;
	int y_first;
	int x_second;
	int y_second;
	int width_first;
	int height_first;
	int width_second;
	int height_second;
	int total_length;
};

// TODO Finish the struct.
struct flimsy_lift
{
	
};