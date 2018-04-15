# setup.py
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy

setup(
    name='voronoi',
    ext_modules=[
        # the .pyx file may not have the same name as the .cpp file
        Extension(
            "voronoi",
            ["voronoi_.pyx", "mwrap.cpp"],
            language="c++",
            extra_compile_args=["-std=c++11"],
            extra_link_args=["-std=c++11"],
            include_dirs=[numpy.get_include(), 'D:/opencv/build/include'],
            libraries=["opencv_world341"],
            library_dirs=['D:/opencv/build/x64/vc15/lib']
        )
    ],
    cmdclass = {'build_ext': build_ext}
)
