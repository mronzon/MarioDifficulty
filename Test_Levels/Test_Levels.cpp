#include "../Collision_Data/general.h"
#include "../Danger_Data/general.h"
#include "../Reach_Data/general.h"
#include "../Reach_Data/graph.h"
#include "../Enemy_Data/general.h"
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
    if(argc != 10)
        return -1;
    string const base_path = argv[1];
    string level_path;
    string folder_path;
    bool create_all_jsons = false;
    bool create_dangers = false;
    bool create_reachs = false;
    bool create_enemies_metric = false;
    points_array tab;
    
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
        if(to_switch == "--enemies")
        {
            create_enemies_metric = convert_string(argv[i+1]);
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
            create_json(base_path, level_path);
        }
        if(create_dangers)
        {
            cout << "Creation du danger pour " << level_path << endl;
            create_danger(folder_path);
        }
        if(create_reachs)
        {
            cout << "Creation du reach de " << level_path << endl;
            std::string level_name = level_path.substr(8, level_path.size());
            int index = ((int) level_name[0] - 49) * 3 + (int) level_name[2] - 48;
            tab.emplace_back(point(index, create_reach(folder_path)));
        }
        if(create_enemies_metric)
        {
            cout << "Creation des metrics enemies de " << level_path << endl;
            create_metric(base_path, "\\Niveau_1_1", false);
        }
    }
    cout << "Creation FINI !!!!!"<< endl;
    if(create_reachs)
    {
        create_graph(tab, base_path + "\\all_metric.png");
    }
    return 0;
}
