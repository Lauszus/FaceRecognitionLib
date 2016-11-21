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

#ifndef __pca_h__
#define __pca_h__

#include <Eigen/Dense> // http://eigen.tuxfamily.org

using namespace Eigen;

class PCA {
public:
    /**
     * Computes the Eigenvectors of the images using PCA.
     * @param  images        Each images is represented as a column vector.
     * @param  numComponents Number of singular values used. If this is set to -1, a cumulative energy threshold of 90 % is used.
     * @return               Returns the number of components used.
     */
    int32_t compute(const MatrixXi &images, int32_t numComponents = -1);

protected:
    MatrixXf U; // Eigenvectors
    VectorXf mu; // Mean along each row

private:
    const float cumulativeEnergyThreshold = .9f; // Determine the number of principal components required to model 90 % of data variance
};

#endif
