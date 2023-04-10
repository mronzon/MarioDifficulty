
#include "../Collision_Data/general.h"
#include "../Danger_Data/general.h"
#include <string>
#include <iostream>
#include <filesystem>

using namespace std;

bool convert_string(string const& str)
{
    if(str == "false")
    {
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    if(argc != 8)
        return -1;
    string const base_path = argv[1];
    string level_path;
    string folder_path;
    size_t result;
    bool create_all_jsons = false;
    bool create_dangers = false;
    bool create_reachs = false;
    for (int i = 2; i < argc - 1; i++)
    {
        string to_switch = argv[i];
        if(to_switch == "--json")
        {
            create_all_jsons = convert_string(argv[i+1]);
            i++;
        }
        if(to_switch == "--danger")
        {
            create_dangers = convert_string(argv[i+1]);
            i++;
        }
        if(to_switch == "--reach")
        {
            create_reachs = convert_string(argv[i+1]);
            i++;
        }
    }
    
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
        if(create_dangers)
        {
            cout << "Creation du danger pour " << level_path << endl;
            create_danger(folder_path);
        }
        
        
        
        
    }
    return 0;
}
