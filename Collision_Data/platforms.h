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
};

struct flimsy_lift_t
{
	int x;
	int y;
	int width;
	int height;
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


std::vector<collision_t> inline merge_lift(std::vector<collision_t>& lifts)
{
	// Sort unique collisions for rows (top -> bot), then cols (left -> right).
	std::sort(lifts.begin(), lifts.end(), comp_collision);

	std::vector<collision_t> tmp;
	bool to_add = true;
	for (const collision_t& collision : lifts)
	{
		to_add = true;
		for (const collision_t& col : tmp)
		{
			if (collision.x > col.x && collision.x < col.x + col.height)
			{
				if (collision.y > col.y && collision.y < col.y + col.width)
				{
					to_add = false;
					break;
				}
			}
		}
		if (to_add) tmp.push_back(collision);
	}
	lifts = tmp;
	// Merge horizontaly connected collisions.
	std::vector<collision_t> collision_merged;
	collision_t result;
	for (int cur = 0; cur < lifts.size();) {
		result = lifts[cur++];
		while (cur < lifts.size() && lifts[cur].x == result.x && lifts[cur].height == result.height && lifts[cur].y <= result.y + result.width) {
			result.width += lifts[cur].width - ((result.y + result.width) - lifts[cur].y);
			cur++;
		}
		collision_merged.push_back(result);
	}

	return collision_merged;
}

bool inline comp_lift(const collision_t& a, const collision_t& b) {
	return (a.y < b.y || (a.y == b.y && a.x < b.x));
}


// The structures for the Magic Bean
struct magic_bean_t
{
	int x;
	int y;
	int width;
	int height;
	int x_exit;
	int y_exit;
};

