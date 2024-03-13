from numpy import ndarray
import skimage
from skimage.filters import gaussian, threshold_otsu
from skimage.color import rgb2gray
from skimage.measure import label, regionprops
import numpy as np
from typing import Callable
import matplotlib.pyplot as plt
import tempfile
from pathlib import Path

class Node:

    def __init__(
        self,
        name: str,
        f: Callable | None = None,
        childs: list["Node"] | None = None,
        *args,
        **kwargs,
    ):
        self.name = name
        self.childs = [] if childs is None else childs
        self.f = f
        self.args = args
        self.kwargs = kwargs

    def add_child(self, child: "Node"):
        self.childs.append(child)

    def __call__(self, array: np.ndarray | None = None):
        if self.f is not None:
            result = self.f(array, *self.args, **self.kwargs)
        else:
            result = array
        for child in self.childs:
            child(result)

    def __str__(self):
        s = f"{self.name}("

        isFirst = True
        for child in self.childs:
            if not isFirst:
                s += ", "
            s += str(child)
            isFirst = False

        s += ")"
        return s

class Display(Node):

    def __call__(self, array: ndarray | None = None):
        plt.ion()
        plt.figure(self.name)
        plt.imshow(array)
        plt.show()
        plt.pause(2)
        plt.close()
        plt.ioff()

class Centroid(Node):

    def __call__(self, array):
        props = regionprops(array)
        for child in self.childs:
            child(props[0].centroid)

class Store(Node):

    def __call__(self, array):
        path = self.kwargs.get("path", Path(tempfile.gettempdir()) / "out.txt")
        np.savetxt(path, array)

def binarize(array):
    return array > threshold_otsu(array)

image = skimage.data.coffee()

conveyor = Node("source")
convert = Node(
    "rgb2gray",
    rgb2gray,
    [
        Display("gray"),
    ],
)
blur = Node("gaussian", gaussian, [convert], sigma=1.1)
store = Store("output")
centroid = Centroid("centroid", childs=[store])
binarize = Node(
    "binarize",
    binarize,
    [Node("label", label, [Display("labeled"), centroid])],
)
convert.add_child(binarize)
conveyor.add_child(blur)
print(conveyor)
conveyor(image)

config = {
    "name": "Source",
    "childs": [
        {
            "name": "blur",
            "callable": "gaussian",
            "kwargs": {"sigma": 1.1},
            "childs": [
                {
                    "name": "rgb2gray",
                    "callable": "rgb2gray",
                    "childs": [
                        {"name": "gray", "callable": "display"},
                        {
                            "name": "binarize",
                            "callable": "binarize",
                        },
                    ],
                },
            ],
        },
    ],
}