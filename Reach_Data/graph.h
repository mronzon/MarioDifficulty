#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

typedef std::pair<int, float> point;
typedef std::vector<point> points_array;

std::string inline truncate_float(float val, int numDigits)
{
    std::string output = std::to_string(val).substr(0, numDigits+1);
    if (output.find('.') ==  std::string::npos ||
        output.back() == '.')
    {
        output.pop_back();
    }
    return output;
}

void inline create_graph(const points_array& points, std::string const path_to_save)
{
    cv::Mat graph_image(1000, 1000, CV_8UC1, cv::Scalar(0));
    // Creation of the axes. Width = 10.
    cv::Rect const vertical_axe(100, 50, 10, 860);
    cv::Rect const horizontal_axe(90, 890, 860, 10);
    cv::rectangle(graph_image, vertical_axe, cv::Scalar(255), cv::FILLED);
    cv::rectangle(graph_image, horizontal_axe, cv::Scalar(255), cv::FILLED);
    points_array::size_type const n = 10;
    points_array::size_type const step = 840 / n;
    float max_value = -1; // The maximum value of the points.
    int max_y = -1;
    for (point elt : points)
    {
        if (elt.second > max_value)
            max_value = elt.second;
        if(elt.first > max_y)
            max_y = elt.first;
    }
    float value_per_pixel_y = 900.f / max_y;
    float value_per_pixel_x = 850.f / max_value;
    float write_step =  max_value / 10;
    int step_x = max_y / 10;
    for (points_array::size_type i = 1; i <= n; i++)
    {
        cv::Rect const rect_horizontal(100 + i*step, 900, 5, 10); 
        cv::rectangle(graph_image, rect_horizontal, cv::Scalar(255), cv::FILLED);
        cv::putText(graph_image, std::to_string(i * step_x), cv::Point(80 + i*step, 925), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255), 1.5);
        
        cv::Rect const rect_vertical(90, 900 - i*step,10, 5); 
        cv::rectangle(graph_image, rect_vertical, cv::Scalar(255), cv::FILLED);
        cv::putText(graph_image, truncate_float(write_step*i, 4), cv::Point(40, 900 - i*step), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255), 1.5);
    }
    point point_avant = points.at(0);
    for (points_array::size_type i = 1; i < points.size(); i++)
    {
        point point_apres = points.at(i);
        float step_x_avant = value_per_pixel_x * point_avant.second;
        float step_y_avant = value_per_pixel_y * point_avant.first;
        float step_x_apres = value_per_pixel_x * point_apres.second;
        float step_y_apres = value_per_pixel_y * point_apres.first;
        cv::Point avant(50 + step_y_avant, 890 - step_x_avant);
        cv::Point apres(50 + step_y_apres, 890 - step_x_apres);
        cv::line(graph_image, avant, apres, cv::Scalar(255));
        point_avant = point_apres;
    }
    cv::imwrite(path_to_save, graph_image);

}

void inline find_max_points(const points_array& points, points_array& result)
{
    //Avancer dans la suite de points et regarder la valeur et celle d'apres. Si celle d'apres est plus grande on continue. Sinon on ajoute l'ancienne dans résult.
    // A voir ce que ça donne dans un premier temps apres on peut essayer de faire un gradient descent des familles


    

    for (points_array::size_type i = 10; i < points.size(); i++)
    {
        bool to_add = true;
        for(points_array::size_type j = i-10; j < __min(points.size(), i + 10); j++)
        {
            if(i == j)
            {
                continue;
            }
            if(points.at(j).second >= points.at(i).second)
            {
                to_add = false;
                break;
            }
        }
        if(to_add)
        {
            result.emplace_back(points.at(i));
        }
    }
}

#endif