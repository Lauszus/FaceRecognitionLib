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
#include <RedSVD/RedSVD-h> // https://github.com/ntessore/redsvd-h

#include "PCA.h"

using namespace std;
using namespace Eigen;

// See: http://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
static IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");

int32_t PCA::compute(const MatrixXi &images, int32_t numComponents /*= -1*/) {
#ifndef NDEBUG
    cout << "Computing PCA" << endl;
#endif // NDEBUG

    const size_t n_pixels = images.rows();
    const size_t n_images = images.cols();

    int32_t K = numComponents;

    mu = images.cast<float>().rowwise().mean(); // Calculate the mean along each row

    MatrixXf images_mu = images.cast<float>().colwise() - mu; // Subtract means from all columns before doing SVD

#ifndef NDEBUG
    cout << "Calculating the covariance matrix" << endl;
#endif // NDEBUG
    if (n_pixels < n_images) {
        if (K == -1) {
#ifndef NDEBUG
            cout << "Please specify the number of singular values used" << endl;
#endif // NDEBUG
            assert(K != -1);
        }
        MatrixXf cov_matrix = images_mu*images_mu.transpose();
#ifndef NDEBUG
        cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;
#endif // NDEBUG

        cout << "Calculating the SVD" << endl;
        RedSVD::RedSVD<MatrixXf> svd(cov_matrix, K); // Calculate K largest singular values, using the JacobiSVD function with this size of covariance matrix is extremely slow, so beware!
        //cout << svd.singularValues().format(OctaveFmt) << endl;
        U = svd.matrixU();
    } else { // Method based on "Eigenfaces for recognition" by M. Turk and A. Pentland
        MatrixXf cov_matrix = images_mu.transpose()*images_mu;
#ifndef NDEBUG
        cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;

        cout << "Calculating the SVD" << endl;
#endif // NDEBUG
        //JacobiSVD<MatrixXf> svd(cov_matrix, ComputeThinV); // Calculate singular values
        BDCSVD<MatrixXf> svd(cov_matrix, ComputeThinV); // Calculate singular values

        if (K == -1) { // Calculate K based on cumulative energy instead of using hardcoded value - see: https://en.wikipedia.org/wiki/Principal_component_analysis#Compute_the_cumulative_energy_content_for_each_eigenvector
            VectorXf S = svd.singularValues(); // Get singular values
            //cout << S.format(OctaveFmt) << endl;
            VectorXf cumulativeEnergy(S.size());
            cumulativeEnergy(0) = S(0);
            for (int i = 1; i < S.size(); i++)
                cumulativeEnergy(i) = cumulativeEnergy(i - 1) + S(i); // Calculate the cumulative sum of the singular values
            //cout << cumulativeEnergy.format(OctaveFmt) << endl;
            K = 1; // Make sure that we have at least two Eigenfaces - note that we add one to this value below
            for (; K < cumulativeEnergy.size(); K++) {
                float energy = cumulativeEnergy(K) / cumulativeEnergy(cumulativeEnergy.size() - 1); // Convert cumulative energy into percentage
                //cout << energy << endl;
                if (energy >= cumulativeEnergyThreshold) {
                    K++; // Since indices start at 0 we need to add one to the K value
                    break;
                }
            }
#ifndef NDEBUG
            cout << "Extracting " << K << " Eigenfaces. Containing " << cumulativeEnergyThreshold << " % of the energy" << endl;
#endif // NDEBUG
            if (K > (int32_t)(n_images - 1)) { // Make sure that K is never equal to n_images
                K = n_images - 1; // K can never be larger than n_images - 1
#ifndef NDEBUG
                cout << "K was limited to: " << K << endl;
#endif // NDEBUG
            }
        }
#ifndef NDEBUG
        else
            cout << "Using " << K << " Eigenfaces" << endl;
#endif // NDEBUG

        // MatrixXf D = S.block(0, 0, n_images, K); // Extract K largest values
        // D = D*D / n_pixels; // Calculate eigenvalues
        MatrixXf V = svd.matrixV().block(0, 0, n_images, K); // Extract K largest values

#ifndef NDEBUG
        cout << "V: " << V.rows() << " x " << V.cols() << " norm: " << V.norm() << endl;
#endif // NDEBUG
        U = images_mu*V; // Calculate the actual Eigenvectors of the true covariance matrix
        U.colwise().normalize(); // Normalize Eigenvectors
    }
#ifndef NDEBUG
    cout << "U: " << U.rows() << " x " << U.cols() << " norm: " << U.norm() << endl;
#endif // NDEBUG

    return K;
}
