#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "voronoi.h"
#include <string>
#include <vector>
#include <iostream>

VoronoiThinner thinner;

std::vector<unsigned char> _thin(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name){
    cv::Mat img(h, w, CV_8UC1, data);
    
    thinner.thin(img, implementation_name, false);
    
    cv::Mat skel(h, w, CV_8UC1, thinner.get_skeleton().data);
    std::vector<unsigned char> vtor;
    
    vtor.assign(skel.data, skel.data + h*w);
    
    return vtor;
}