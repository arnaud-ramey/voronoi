#include <string>
#include <valarray>

void _thinImplicit(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name);

std::valarray<unsigned char> _thinExplicit(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name);