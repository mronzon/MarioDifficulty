#include "general.h"

int main(int argc, char* argv[])
{
    std::string const base_path = argv[1];
    points_array tab_nishikado;
    tab_nishikado.push_back(point(0, 0));
    int value = 1;

    for(auto const& entry : std::filesystem::directory_iterator(base_path))
    {
        std::string folder_path = entry.path().string();
        std::string level_path = folder_path.substr(base_path.size(), folder_path.size());

        const std::string path_recup_int = base_path + "\\Graph" + level_path;
        std::ifstream txt_recup_integrale;
        std::string tmp;
        float value_int = 0;

        if(level_path.find("Niveau_") == std::string::npos || level_path.find("Niveau_Custom") != std::string::npos)
        {
            continue;
        }

        //create_graphs(base_path, level_path);

        std::cout << path_recup_int << std::endl;

        txt_recup_integrale.open(path_recup_int + "\\128 total.txt");
        if (!txt_recup_integrale) {
            std::cout << "On a pas reussi a ouvrir le .txt pour calculer Nishikado";
            exit(128);
        }
        while (txt_recup_integrale) {
            std::getline(txt_recup_integrale, tmp);
            if (tmp.find("Valeur") != std::string::npos) {
                tmp.replace(0, 24, "");
                value_int = std::stof(tmp);
                tab_nishikado.push_back(point(value, value_int));
            }
        }
        txt_recup_integrale.close();
        value++;
        
    }
    create_graph(tab_nishikado, base_path + "\\Nishikado 128.png");
    return 0;
}
