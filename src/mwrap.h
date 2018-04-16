#include <string>
#include <vector>

void _thinFast(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name);

std::vector<unsigned char> _thinSlower(unsigned char *data,
                                int h,
                                int w,
                                const std::string & implementation_name);