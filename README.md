# Eigenfaces
#### Developed by Kristian Lauszus, 2016

The code is released under the GNU General Public License.
_________

This project demonstrats how to calculate Eigenfaces efficiently as described in the [paper](http://www.face-rec.org/algorithms/PCA/jcn.pdf) by M. Turk and A. Pentland.

The code was tested using images from the [AT&T Facedatabase](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html).

The [Eigenfaces.m](Eigenfaces.m) Matlab script was used for development and was then ported to C++.

In order to build the C++ code you need to install Eigen3:

Mac:

```bash
brew install eigen
```

Ubuntu:

```bash
sudo apt-get install libeigen3-dev
```

The [RedSVD](https://github.com/ntessore/redsvd-h) library is included as a submodule.

If you want to convert the PGM images, I would recommend using [ImageMagick](http://www.imagemagick.org). This can then be done easily by running the [convert_pgm.sh](convert_pgm) script.

For more information send me an email at <lauszus@gmail.com>.
