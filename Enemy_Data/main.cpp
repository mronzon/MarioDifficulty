#include "general.h"

int main(int argc, char* argv[]) {
    if (argc != 2) return 0;
	
    create_metric(argv[1], "\\Niveau_1_1", false);

    return 0;
}
