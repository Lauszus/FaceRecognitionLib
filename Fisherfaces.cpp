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

#include "Fisherfaces.h"

using namespace std;
using namespace Eigen;

void Fisherfaces::train(const MatrixXi &images, const VectorXi &classes) {
    this->n_pixels = images.rows();
    size_t n_images = classes.size(); // Get number of images
    assert(classes.minCoeff() == 1); // Class labels should start at 1 and increment by 1
    int c = classes.maxCoeff(); // Calculate the number of classes

    PCA::compute(images, n_images - c);
    this->V = PCA::U; // Store PCA eigenvector temporarily, as it used for the projection
    MatrixXf W_pca = project(images); // Project images onto subspace
    this->numComponents = LDA::compute(W_pca, classes, c - 1); // Copy number of components from LDA

#ifndef NDEBUG
    cout << "Calculate weights for all images" << endl;
#endif
    this->V = PCA::U*LDA::U; // Calculate Fisherfaces
    this->W_all = project(images); // Calculate weights
#ifndef NDEBUG
    cout << "W_all: " << W_all.rows() << " x " << W_all.cols() << endl;
#endif
}
