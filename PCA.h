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

#ifndef _pca_h_
#define _pca_h_

#include <numeric>
#include <vector>

#include <Eigen/Dense> // http://eigen.tuxfamily.org

using namespace std;
using namespace Eigen;

class PCA {
public:
    PCA() {};

    /**
     * Computes the Eigenvectors of the images using PCA.
     * @param images        Each images is represented as a column vector.
     * @param numComponents Number of singular values used. If this is set to -1, a cumulative energy threshold of 90 % is used.
     */
    void compute(MatrixXf &images, int16_t numComponents = -1);
    MatrixXf project(const MatrixXf &X);
    MatrixXf reconstructFace(const MatrixXf &W);

    MatrixXf U; // Eigenvectors
    VectorXf mu; // Mean along each row
    int16_t K; // Number of singular values used, this might be calculated based on cumulative energy threshold

protected:
    const float cumulativeEnergyThreshold = .9f; // Determine the number of principal components required to model 90 % of data variance
    uint16_t n_pixels, n_images;
};

#endif
