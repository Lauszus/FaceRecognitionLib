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

class Facebase : public PCA {
public:
    /**
     * Project X onto subspace.
     * @param  X Input image.
     * @return   Returns the weight matrix.
     */
    MatrixXf project(const MatrixXi &X);

    /**
     * Calculate distance between weight and the weights for the current method used.
     * @param  W Weights calculated by projecting images onto subspace.
     * @return   Return a vector containing all distances.
     */
    VectorXf euclideanDist(const VectorXf &W);

    /**
     * Reconstruct a face from a weight.
     * @param  W Weight calculated by projecting image onto subspace.
     * @return   Returns the face vector.
     */
    VectorXf reconstructFace(const VectorXf &W);

    /**
     * Calculate the distance to the face subspace.
     * @param  X    Input image.
     * @param  face Face vector.
     * @return      Returns the distance to the face subspace.
     */
    float euclideanDistFace(const VectorXi &X, const VectorXf &face);

    MatrixXf V; // Eigenvector
    int32_t numComponents; // Number of components

protected:
    MatrixXf W_all; // Total weights
    size_t n_pixels;
};

#endif
