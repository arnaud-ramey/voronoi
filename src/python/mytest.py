# -*- coding: utf-8 -*-
"""
Created on Sat Apr 14 00:24:57 2018

@author: aferust
"""

import numpy as np
import pylab as pl
from skimage.io import imread
from scipy import ndimage as ndi
import time
import voronoi

"""
There are two implementations named voronoi.thinImplicit() and voronoi.thinExplicit().
According to my tests, there was no serious speed difference between those.
However, I didn't compare memory usages of these functions. I guess thinExplicit
uses more?
At the end, I am a python lover: Explicit is better than implicit :)
"""

imrgb = imread('p1.jpg'); pl.figure(); pl.imshow(imrgb)

r = imrgb[:,:,0]
binary = (50 < r) & (r < 165)
im_filled = ndi.binary_fill_holes(binary).astype(np.uint8)

#im_filled = im_filled  * 255


start_time = time.time()
#voronoi.thinImplicit(im_filled, "guo_hall_fast") # returns None, modifies the source array
im_filled = voronoi.thinExplicit(im_filled, "guo_hall_fast") # do not touch source, returns a numpy array
print("--- %s seconds ---" % (time.time() - start_time))

pl.figure(); pl.imshow(im_filled)

pl.show()






