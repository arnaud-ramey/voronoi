#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../voronoi.h"
#include <string>
#include <valarray>

VoronoiThinner thinner;

void _thinImplicit(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name){
    cv::Mat img(h, w, CV_8UC1, data);
    
    thinner.thin(img, implementation_name, false);
    
    memcpy(data, thinner.get_skeleton().data, h*w * sizeof *thinner.get_skeleton().data); 
}

std::valarray<unsigned char> _thinExplicit(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name){
    cv::Mat img(h, w, CV_8UC1, data);
    
    thinner.thin(img, implementation_name, false);
    
    std::valarray<unsigned char> varray (thinner.get_skeleton().data, h*w);
    
    return varray;
}