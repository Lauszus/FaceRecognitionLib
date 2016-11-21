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

#ifndef __eigenfaces_h__
#define __eigenfaces_h__

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "Facebase.h"
#include "PCA.h"

using namespace Eigen;

class Eigenfaces : public Facebase {
public:
    /**
     * Train Eigenfaces.
     * @param images Each images represented as a column vector.
     */
    void train(const MatrixXi &images);
};

#endif
