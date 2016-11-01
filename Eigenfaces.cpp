/* Copyright (C) 2016 Kristian Sloth Lauszus. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Sloth Lauszus
 Web      :  http://www.lauszus.com
 e-mail   :  lauszus@gmail.com
*/

#include <iostream>

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "Eigenfaces.h"

using namespace std;
using namespace Eigen;

void Eigenfaces::compute(MatrixXf &images) {
    PCA::compute(images);
#ifndef NDEBUG
    cout << "Calculate weights for all images" << endl;
#endif
    W_all = project(images);
#ifndef NDEBUG
    cout << "W_all: " << W_all.rows() << " x " << W_all.cols() << endl;
#endif
    face_all = reconstructFace(W_all);
    //cout << "face_all: " << face_all.rows() << " x " << face_all.cols() << endl;
}

VectorXf Eigenfaces::euclideanDist(const VectorXf &W) {
    return ((W_all.colwise() - W)/n_pixels).colwise().norm()/sqrt(K); // Measure euclidean distance between weights
}
