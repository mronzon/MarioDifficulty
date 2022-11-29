#pragma once

struct position_t
{
	int x;
	int y;
};

struct moving_lift_t
{
	int x;
	int y;
	int width;
	int height;
	int axe; // 0 for vertical and 1 for horizontal
	int first_limit;
	int second_limit;
	int initial_velocity; // -1 for bottom/left and 1 for up/right
};

struct balanced_lift_t
{
	int x_first;
	int y_first;
	int width_first;
	int height_first;
	int x_second;
	int y_second;
	int width_second;
	int height_second;
	int total_length;
	int max_x;
};

// TODO Finish the struct.
struct flimsy_lift
{
	
};