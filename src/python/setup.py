# setup.py
"""
To build a voronoi.pyd or voronoi.so type following:

python setup.py build_ext --inplace

It should be noted that:
Following parameters are not actually required.
But they may be needed for gcc compliance if c++11 features are used in the future implementations.
...
extra_compile_args=["-std=c++11" ...],
extra_link_args=["-std=c++11" ...],
...
"""

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy

opencv_include_dir = 'D:/opencv/build/include'
#opencv_include_dir = '/usr/local/include' # use this one if needed
opencv_lib_dir = 'D:/opencv/build/x64/vc15/lib'
#opencv_lib_dir = '/usr/local/lib' # use this one if needed
opencv_libs = ['opencv_world341']
#opencv_libs = ['opencv_core', 'opencv_highgui', 'opencv_imgproc'] # use this one if needed

setup(
    name='voronoi',
    ext_modules=[
        Extension(
            "voronoi",
            ["voronoi_.pyx", "mwrap.cpp"],
            language="c++",
            extra_compile_args=["-std=c++11", "-O2", "-O3"],
            extra_link_args=["-std=c++11"],
            include_dirs=[numpy.get_include(), opencv_include_dir],
            libraries=opencv_libs,
            library_dirs=[opencv_lib_dir]
        )
    ],
    cmdclass = {'build_ext': build_ext}
)
