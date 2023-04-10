
#include "../Collision_Data/general.h"
#include "../Danger_Data/general.h"
#include <string>
#include <iostream>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 6)
        return -1;
    string const base_path = argv[1];
    string level_path;
    string folder_path;
    size_t result;
    bool const create_all_jsons = false;
    for(auto const& entry : filesystem::directory_iterator(base_path))
    {
        folder_path = entry.path().string();
        level_path = folder_path.substr(base_path.size(), folder_path.size());
        if(level_path.find("Niveau_") == string::npos)
        {
            continue;
        }
        
        if(create_all_jsons)
        {
            cout << "Creation du json pour " << level_path << endl;
            result = create_json(base_path, level_path);
            if(result < 0)
            {
                cout << "Probleme avec ce niveau" << endl;
            }
        }
        cout << "Creation du danger pour " << level_path << endl;
        create_danger(folder_path);
        
        
        
    }
    return 0;
}
