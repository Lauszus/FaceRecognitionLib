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

// Inspired by: https://github.com/opencv/opencv/blob/master/modules/core/src/lda.cpp,
// https://github.com/bytefish/facerec/blob/master/py/facerec/feature.py,
// and https://github.com/bytefish/facerec/blob/master/m/models/lda.m

#include <iostream>
#include <algorithm>

#include <Eigen/Dense> // http://eigen.tuxfamily.org
#include <Eigen/Eigenvalues>

#include "LDA.h"
#include "Tools.h"

using namespace std;
using namespace Eigen;

// See: http://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
static IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");

int32_t LDA::compute(const MatrixXf &X, const VectorXi &classes, int32_t numComponents) {
#ifndef NDEBUG
    cout << "Computing LDA" << endl;
    cout << "X: " << X.rows() << " x " << X.cols() << endl;
    cout << "classes: " << classes.rows() << " x " << classes.cols() << endl;
#endif // NDEBUG

    const size_t N = X.cols();
    assert((size_t)classes.size() == N); // Make sure that there is a class for each column
    const size_t dim = X.rows();
    const int c = classes.maxCoeff(); // Calculate the number of classes, assuming that labels start at 1 and are incremented by 1

#ifndef NDEBUG
    cout << "N: " << N << endl;
    cout << "dim: " << dim << endl;
    cout << "c: " << c << endl;
#endif // NDEBUG

    numComponents = min(c - 1, numComponents); // Make sure number of components is valid
#ifndef NDEBUG
    cout << "numComponents: " << numComponents << endl;
#endif // NDEBUG

    if (dim == 0 || numComponents == 0) {
        U = MatrixXf::Zero(dim, 1); // Set all element to zero
        return 0; // Make sure dimension is valid
    }

    VectorXf mu = X.rowwise().mean(); // Calculate the mean along each row

    vector<size_t> idx = sortIndexes(classes); // Get indices corresponding to the different classes sorted by the class number
    //for (auto i : idx) cout << "classes(" << i << "): " << classes(i) << endl;
    auto iterator = idx.begin(); // Get iterator

    MatrixXf Sw = MatrixXf::Zero(dim, dim);
    MatrixXf Sb = MatrixXf::Zero(dim, dim);
    // cout << X.format(OctaveFmt) << endl;
    for (int i = 0; i < c; i++) {
        int class_count = count(classes.data(), classes.data() + classes.size(), i + 1); // Get the number of times that specific class occurs
        // cout << "class_count: " << class_count << endl;

        MatrixXf X_class(dim, class_count);
        for (int j = 0; j < class_count; j++) {
            // cout << "classes(" << iterator[j] << "): " << classes(iterator[j]) << endl;
            X_class.block(0, j, dim, 1) = X.block(0, iterator[j], dim, 1); // Copy data from original image, each image at a time based on the sorted indices
        }
        advance(iterator, class_count); // Advance iterator by the class count
        // cout << X_class.format(OctaveFmt) << endl;

        VectorXf mu_class = X_class.rowwise().mean(); // Calculate the mean along each row
        X_class = X_class.colwise() - mu_class; // Subtract means from all columns, thus centering the data
        VectorXf mu_vector = mu_class - mu;

        Sw += X_class*X_class.transpose(); // Calculate within-class scatter
        Sb += N*mu_vector*mu_vector.transpose(); // Calculate between-class scatter
    }

    EigenSolver<MatrixXf> es(Sw.inverse()*Sb); // Solves eigenvalues and eigenvectors of the matrix
    VectorXf D = es.eigenvalues().real();
    idx = sortIndexes(D); // Get indices sorted by increasing Eigenvalues
    // for (auto i : idx) cout << "D(" << i << "): " << D(i) << endl;
    MatrixXf V = es.eigenvectors().real();
    U = MatrixXf(dim, numComponents); // Allocate memory
    for (int32_t i = 0; i < numComponents; i++) // Sort Eigenvectors according to Eigenvalues in decreasing order
        U.col(i) = V.col(idx[(idx.size() - 1) - i]); // Reverse indices, as we want them to be sorted by decreasing Eigenvalues
    // cout << U.block(0, 0, 10, 10).format(OctaveFmt) << endl;

    return numComponents;
}
