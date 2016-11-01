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

void PCA::train(MatrixXf &images) {
    n_pixels = images.rows(); // Each images is represented as a column vector
    n_images = images.cols();

    mu = images.rowwise().mean(); // Calculate the mean along each row

    MatrixXf images_mu = images.colwise() - mu; // Subtract means from all columns before doing SVD

    cout << "Calculating the covariance matrix" << endl;
#if n_pixels < n_images
    MatrixXf cov_matrix = images_mu*images_mu.transpose();
    cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;

    cout << "Calculating the SVD" << endl;
    RedSVD::RedSVD<MatrixXf> svd(cov_matrix, K); // Calculate K largest singular values, using the JacobiSVD function with this size of covariance matrix is extremely slow, so beware!
    //cout << svd.singularValues().format(OctaveFmt) << endl;
    U = svd.matrixU();
#else // Method based on "Eigenfaces for recognition" by M. Turk and A. Pentland
    MatrixXf cov_matrix = images_mu.transpose()*images_mu;
    cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;

    cout << "Calculating the SVD" << endl;
    JacobiSVD<MatrixXf> svd(cov_matrix, ComputeThinV); // Calculate singular values

#if 1 // Calculate K based on cumulative energy instead of using hardcoded value - see: https://en.wikipedia.org/wiki/Principal_component_analysis#Compute_the_cumulative_energy_content_for_each_eigenvector
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
    cout << "Extracting " << K << " Eigenfaces. Containing " << cumulativeEnergyThreshold << " % of the energy" << endl;
#endif
    // MatrixXf D = S.block(0, 0, n_images, K); // Extract K largest values
    // D = D*D / n_pixels; // Calculate eigenvalues
    MatrixXf V = svd.matrixV().block(0, 0, n_images, K); // Extract K largest values

    cout << "V: " << V.rows() << " x " << V.cols() << " norm: " << V.norm() << endl;
    U = images_mu*V; // Calculate the actual Eigenvectors of the true covariance matrix
    U.colwise().normalize(); // Normalize Eigenvectors
#endif
    cout << "U: " << U.rows() << " x " << U.cols() << " norm: " << U.norm() << endl;

    cout << "Calculate weights for all images" << endl;
    W_all = project(images);
    cout << "W_all: " << W_all.rows() << " x " << W_all.cols() << endl;
    face_all = reconstructFace(W_all);
    //cout << "face_all: " << face_all.rows() << " x " << face_all.cols() << endl;
}

MatrixXf PCA::project(const MatrixXf &X) {
    return U.transpose()*(X.colwise() - mu); // Project X onto Eigenface subspace
}

MatrixXf PCA::reconstructFace(const MatrixXf &W) {
    return (U*W).colwise() + mu; // Reconstruct face
}

VectorXf PCA::euclideanDist(const VectorXf &W) {
    return ((W_all.colwise() - W)/n_pixels).colwise().norm()/sqrt(K); // Measure euclidean distance between weights
}
