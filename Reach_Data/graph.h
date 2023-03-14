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
    
    for (point const elt : points)
    {
        float step_x = value_per_pixel_x * elt.second;
        float step_y = value_per_pixel_y * elt.first;
        cv::Rect rect(50 + step_y, 890 - step_x , 5, 5);
        cv::rectangle(graph_image, rect, cv::Scalar(255), cv::FILLED);
    }
    cv::imwrite(path_to_save, graph_image);

}

#endif