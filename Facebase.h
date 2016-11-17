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

#ifndef __facebase_h__
#define __facebase_h__

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "PCA.h"

using namespace Eigen;

class Facebase {
public:
    virtual MatrixXf project(const MatrixXi &X) = 0;
    virtual MatrixXf reconstructFace(const MatrixXf &W) = 0;

    VectorXf euclideanDist(const VectorXf &W) {
        return ((W_all.colwise() - W)/n_pixels).colwise().norm()/sqrt(numComponents); // Measure euclidean distance between weights
    };

    MatrixXf V; // Eigenvector
    int32_t numComponents; // Number of components

protected:
    MatrixXf W_all, face_all; // Total weights and faces
    size_t n_pixels;
};

#endif
