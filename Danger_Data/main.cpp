#include "general.h"

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;
	std::string const folder_path = argv[1];
	return create_danger(folder_path);
}