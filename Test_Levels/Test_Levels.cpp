
#include "../Collision_Data/general.h"
#include <string>
#include <iostream>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2)
        return -1;
    string base_path = argv[1];
    string level_path;
    string folder_path;
    size_t result;
    for(auto const entry : filesystem::directory_iterator(base_path))
    {
        folder_path = entry.path().string();
        level_path = folder_path.substr(base_path.size(), folder_path.size());
        if(level_path.find("Niveau_") == string::npos)
        {
            continue;
        }
        cout << "Creation du json pour " << level_path << endl;
        result = create_json(base_path, level_path);
        if(result < 0)
        {
            cout << "Probleme avec ce niveau" << endl;
        }
        
    }
    return 0;
}
