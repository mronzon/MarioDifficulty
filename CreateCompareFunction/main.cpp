#include "general.h"

int main(int argc, char* argv[])
{
    std::string const base_path = argv[1];

    for(auto const& entry : std::filesystem::directory_iterator(base_path))
    {
        std::string folder_path = entry.path().string();
        std::string level_path = folder_path.substr(base_path.size(), folder_path.size());
        if(level_path.find("Niveau_") == std::string::npos || level_path.find("Niveau_Custom") != std::string::npos)
        {
            continue;
        }

        create_graphs(base_path, level_path);
    }
    return 0;
}
