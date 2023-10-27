#include <iostream>

#include "general.h"

int main(int argc, char* argv[]) {

	
    if (argc == 2) create_metric(argv[1], "\\Niveau_4_1", true);
	
	if (argc == 3)
	{
		std::string base_path = argv[1];
		for(auto const& entry : std::filesystem::directory_iterator(base_path))
		{
			std::string folder_path = entry.path().string();
			std::string level_path = folder_path.substr(base_path.size(), folder_path.size());
			if(level_path.find("Niveau_") == std::string::npos)
			{
				continue;
			}
			std::cout << "Creation de la metrique ennemie pour le niveau " << level_path << std::endl;
			create_metric(base_path, level_path, false);
		}
	}
    

    
    return 0;
}
