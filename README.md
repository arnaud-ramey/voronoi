# voronoi

[![Build Status](https://travis-ci.org/arnaud-ramey/voronoi.svg)](https://travis-ci.org/arnaud-ramey/voronoi)

The morphological skeleton of an image is the set of its non-zero pixels which are equidistant to its boundaries.
More info on [Wikipedia](http://en.wikipedia.org/wiki/Topological_skeleton).

Thinning an image consits in reducing its non-zero pixels to their morphological skeleton.
More info on [Wikipedia](http://en.wikipedia.org/wiki/Thinning_(morphology)).

VoronoiSkeleton is a C++ class
made for the fast computing of Voronoi diagrams of monochrome images.
It contains different implementations of thinning algorithms:

  * Zhang - Suen explained in 'A fast parallel algorithm for thinning digital
  patterns' by T.Y. Zhang and C.Y. Suen and based on
  [this implentation](http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/)

  * Guo - Hall explained in 'Parallel thinning with two sub-iteration
  algorithms' by Zicheng Guo and Richard Hall and based on
  [this implentation](http://opencv-code.com/quick-tips/implementation-of-guo-hall-thinning-algorithm/)

  * a morphological one, based on the ```erode()``` and ```dilate()``` operators.
  Coming from previous work by [Félix Abecassis](http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/).

A special care has been given to optimize the 2 first ones.
Instead of
re-examining the whole image at each iteration, only the pixels of the
current contour are considered. This leads to a speedup by almost 100 times
on experimental tests.

Licence
=======

LGPL v3, check file ```LICENCE```.

Dependencies
============

You need the following libraries before compiling :

  * cmake  ( ```$ sudo apt-get install cmake``` ),
  * OpenCV ( ```$ sudo apt-get install libopencv-dev``` )

How to build the program
========================

The project is based on a ```CMakeLists```.
It is easy to build the program on a Unix computer.
Go in the source folder and type:
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

For Windows users, some instructions are available on OpenCV website:
http://opencv.willowgarage.com/wiki/Getting_started .

How to use the program
======================

To display the help,
from the main folder, run the generated executable '```build/test_voronoi``` ' with no arguments.
It will display the help of the program.

Related projects
================

[mnist-digits-as-stroke-sequences](https://github.com/edwin-de-jong/mnist-digits-as-stroke-sequences/)
Code project to transform the well
known MNIST handwritten digit images to sequences of pen strokes, in
order to obtain a data set for sequence learning.

Authors
=======

Arnaud Ramey <arnaud.a.ramey@gmail.com>
  -- Robotics Lab, University Carlos III of Madrid
