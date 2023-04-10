
/*
	Inputs: a level image and a set of collision textures.
	Output: a text file with all collision hits.
*/

#include "general.h"

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;
	
	return create_json(argv[1], "\\Niveau_1_1");
}
