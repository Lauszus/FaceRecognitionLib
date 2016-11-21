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

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "Facebase.h"

using namespace Eigen;

MatrixXf Facebase::project(const MatrixXi &X) {
    return V.transpose()*(X.cast<float>().colwise() - mu); // Project X onto subspace
}

VectorXf Facebase::euclideanDist(const VectorXf &W) {
    return ((W_all.colwise() - W)/n_pixels).colwise().norm()/sqrt(numComponents); // Measure euclidean distance between weights
}

VectorXf Facebase::reconstructFace(const VectorXf &W) {
    return V*W;
}

float Facebase::euclideanDistFace(const VectorXi &X, const VectorXf &face) {
    return ((((X.cast<float>() - mu) - face)/n_pixels).colwise().norm()/sqrt(numComponents)).value(); // Measure euclidean distance between weights
}
