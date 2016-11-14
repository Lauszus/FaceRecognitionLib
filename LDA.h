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

#ifndef __lda_h__
#define __lda_h__

#include <Eigen/Dense> // http://eigen.tuxfamily.org

using namespace Eigen;

class LDA {
public:
    /**
     * Computes the Eigenvectors of X using LDA.
     * @param  X             Data matrix.
     * @param  classes       Class vector. Must start at 1 and increment by one.
     * @param  numComponents Number of components used for the analysis.
     * @return               Returns the number of components used.
     */
    int32_t compute(const MatrixXf &X, const VectorXi &classes, int32_t numComponents);

protected:
    MatrixXf U; // Eigenvectors
};

#endif
