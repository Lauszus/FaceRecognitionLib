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
#include <numeric>
#include <vector>
#include <sys/stat.h>

#include <Eigen/Dense> // http://eigen.tuxfamily.org/index.php?title=Main_Page
#include <RedSVD/RedSVD-h> // https://github.com/ntessore/redsvd-h

#include "pgm.h"

using namespace std;
using namespace Eigen;

// Based on: http://jmcspot.com/Eigenface
// Images are from the AT&T Facedatabase: http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html

// See: http://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");

static const uint8_t M = 112, N = 92;
static const uint16_t n_pixels = M*N;
static const uint8_t n_person = 40;
static const uint8_t n_img_pr_person = 10;
static const uint16_t n_images = n_img_pr_person*n_person;

static const uint8_t K = 15; // Number of singular values

static MatrixXf readPgmAsMatrix(const char *filename) {
    PGMImage imgRaw;
    getPGMfile(filename, &imgRaw);
    MatrixXf img(imgRaw.height, imgRaw.width);
    for (int y = 0; y < imgRaw.height; y++) {
        for (int x = 0; x < imgRaw.width; x++) {
            img(y, x) = imgRaw.data[imgRaw.height - y - 1][x].red; // Map data to Eigen Matrix
        }
    }
    //cout << img.format(OctaveFmt) << endl;
    return img;
}

static void saveMatrixAsPgm(const char *filename, const MatrixXf &A) {
    const float min = A.minCoeff();
    const float max = A.maxCoeff();

    PGMImage img;
    img.height = A.rows();
    img.width = A.cols();
    img.maxVal = 255;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++)
            img.data[img.height - y - 1][x].red = img.maxVal * (A(y, x) - min) / (max - min); // Scale data to 0-255
    }
    savePGMfile(filename, &img);
}

static MatrixXf project(const MatrixXf &U, const MatrixXf &X, const VectorXf &mu) {
    return U.transpose()*(X.colwise() - mu); // Project X onto Eigenface subspace
}

static MatrixXf reconstructFace(const MatrixXf &U, const MatrixXf &W, const VectorXf &mu) {
    return (U*W).colwise() + mu; // Reconstruct face
}

static VectorXf euclideanDist(const MatrixXf &W_all, const VectorXf &W) {
    return ((W_all.colwise() - W)/n_pixels).colwise().norm()/sqrt(K);
}

static void trainEigenfaces(MatrixXf &images, VectorXf &mu, MatrixXf &U, MatrixXf &W_all) {
    cout << "Loading images" << endl;
    images = MatrixXf(n_pixels, n_images); // Pre-allocate storage for images
    cout << "images: " << images.rows() << " x " << images.cols() << endl;

    // Load all images into the matrix
    for (int i = 0; i < n_person; i++) {
        for (int j = 0; j < n_img_pr_person; j++) {
            char filename[50];
            sprintf(filename, "../orl_faces/s%u/%u.pgm", i + 1, j + 1);
            MatrixXf img = readPgmAsMatrix(filename);
            images.block<n_pixels, 1>(0, i*n_img_pr_person+j) = Map<VectorXf>(img.data(), img.size()); // Flatten image
        }
    }

    mu = images.rowwise().mean(); // Calculate the mean along each row

    MatrixXf images_mu = images.colwise() - mu; // Subtract means from all columns before doing SVD

    cout << "Calculating the covariance matrix" << endl;
#if n_pixels < n_images
    MatrixXf cov_matrix = images_mu*images_mu.transpose();
    cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;

    cout << "Calculating the SVD" << endl;
    RedSVD::RedSVD<MatrixXf> svd(cov_matrix, K); // Calculate K largest singular values
    //cout << svd.singularValues() << endl;
    U = svd.matrixU();
    cout << "U: " << U.rows() << " x " << U.cols() << " norm: " << U.norm() << endl;
#else // Method based on "Eigenfaces for recognition" by M. Turk and A. Pentland
    MatrixXf cov_matrix = images_mu.transpose()*images_mu;
    cout << "cov_matrix: " << cov_matrix.rows() << " x " << cov_matrix.cols() << endl;

    cout << "Calculating the SVD" << endl;
    //RedSVD::RedSVD<MatrixXf> svd(cov_matrix, K); // Calculate K largest singular values
    JacobiSVD<MatrixXf> svd(cov_matrix, ComputeThinV); // Calculate K largest singular values
    //VectorXf S = svd.singularValues().block<K, 1>(0,0); // Extract K largest singular values
    //cout << S.format(OctaveFmt) << endl;
    MatrixXf V = svd.matrixV().block<n_images, K>(0,0); // Extract K largest values

    cout << "V: " << V.rows() << " x " << V.cols() << " norm: " << V.norm() << endl;
    U = images_mu*V; // Calculate the actual Eigenvalues of the true coveriance matrix
    U.colwise().normalize(); // Normalize Eigenvectors
    cout << "U: " << U.rows() << " x " << U.cols() << " norm: " << U.norm() << endl;
#endif

    mkdir("eigenfaces", 0755);
    for (int i = 0; i < K; i++) { // Save Eigenfaces as PGM images
        Map<MatrixXf> Eigenface(U.block<n_pixels, 1>(0, i).data(), M, N); // Extract Eigenface
        //cout << "Eigenface: " << Eigenface.rows() << " x " << Eigenface.cols() << endl;
        char filename[50];
        sprintf(filename, "eigenfaces/eigenface%u.pgm", i);
        saveMatrixAsPgm(filename, Eigenface); // Save Eigenface
    }

    cout << "Calculate weights for all images" << endl;
    W_all = project(U, images, mu);
    cout << "W_all: " << W_all.rows() << " x " << W_all.cols() << endl;
    MatrixXf face_all = reconstructFace(U, W_all, mu);
    //cout << "face_all: " << face_all.rows() << " x " << face_all.cols() << endl;

    cout << "Done training" << endl;
}

// Based on: http://stackoverflow.com/a/12399290/2175837
template<typename VectorType>
vector<size_t> sortIndexes(const VectorType &v) {
    // Initialize original index locations
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    // Sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(),
        [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

    return idx;
}

int main(void) {
    MatrixXf images, U, W_all;
    VectorXf mu;
    trainEigenfaces(images, mu, U, W_all);
    //cout << W_all.format(OctaveFmt) << endl;

    MatrixXf target = readPgmAsMatrix("../orl_faces/s3/8.pgm"); // Load a random image from the database

    cout << "Reconstructing Faces" << endl;
    VectorXf W = project(U, Map<VectorXf>(target.data(), target.size()), mu); // Flatten image and project onto Eigenfaces
    VectorXf face = reconstructFace(U, W, mu);
    //cout << W.format(OctaveFmt) << endl;

    cout << "Calculate normalized Euclidean distance" << endl;
    VectorXf dist = euclideanDist(W_all, W);
    //cout << "dist: " << dist.rows() << " x " << dist.cols() << endl;

    vector<size_t> idx = sortIndexes(dist);
    //for (auto i : idx) cout << "dist[" << i << "]: " << dist(i) << endl;

    mkdir("matches", 0755);
    for (int i = 0; i < 9; i++) { // Save first nine matches
        cout << "dist[" << idx[i] << "]: " << dist(idx[i]) << endl;
        char filename[50];
        sprintf(filename, "matches/match%u.pgm", i);
        MatrixXf img = images.block<n_pixels, 1>(0, idx[i]);
        //cout << "img: " << img.rows() << " x " << img.cols() << endl;
        saveMatrixAsPgm(filename, Map<MatrixXf>(img.data(), M, N)); // Save matched image
    }
    saveMatrixAsPgm("matches/target.pgm", target); // Save target image as well
    saveMatrixAsPgm("matches/face.pgm", Map<MatrixXf>(face.data(), M, N)); // Save face image

    cout << endl << "Testing all faces against database" << endl;
    bool allMatched = true;
    for (int i = 0; i < n_person; i++) {
        for (int j = 0; j < n_img_pr_person; j++) {
            char filename[50];
            sprintf(filename, "../orl_faces/s%u/%u.pgm", i + 1, j + 1);
            target = readPgmAsMatrix(filename);
            W = project(U, Map<VectorXf>(target.data(), target.size()), mu); // Flatten image and project onto Eigenfaces
            dist = euclideanDist(W_all, W);
            idx = sortIndexes(dist);
            //cout << "dist[" << idx[0] << "]: " << dist(idx[0]) << endl;
            if ((int)idx[0] != i*n_img_pr_person+j) {
                cout << "Wrong match: " << i*n_img_pr_person+j << " should be: " << idx[0] << endl;
                allMatched = false;
            }
        }
    }
    if (allMatched)
        cout << "All faces matched!" << endl;

    return 0;
}
