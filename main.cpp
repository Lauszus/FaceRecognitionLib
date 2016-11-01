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
#include <sys/stat.h>

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "PCA.h"
#include "pgm.h"

PCA pca;

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

static void trainEigenfaces(MatrixXf &images) {
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

    pca.train(images);

    mkdir("eigenfaces", 0755);
    for (int i = 0; i < pca.K; i++) { // Save Eigenfaces as PGM images
        Map<MatrixXf> Eigenface(pca.U.block<n_pixels, 1>(0, i).data(), M, N); // Extract Eigenface
        //cout << "Eigenface: " << Eigenface.rows() << " x " << Eigenface.cols() << endl;
        char filename[50];
        sprintf(filename, "eigenfaces/eigenface%u.pgm", i);
        saveMatrixAsPgm(filename, Eigenface); // Save Eigenface
    }

    cout << "Done training" << endl;
}

int main(void) {
    MatrixXf images;
    trainEigenfaces(images);

    MatrixXf target = readPgmAsMatrix("../orl_faces/s3/8.pgm"); // Load a random image from the database

    cout << "Reconstructing Faces" << endl;
    VectorXf W = pca.project(Map<VectorXf>(target.data(), target.size())); // Flatten image and project onto Eigenfaces
    VectorXf face = pca.reconstructFace(W);
    //cout << W.format(OctaveFmt) << endl;

    cout << "Calculate normalized Euclidean distance" << endl;
    VectorXf dist = pca.euclideanDist(W);
    //cout << "dist: " << dist.rows() << " x " << dist.cols() << endl;

    vector<size_t> idx = pca.sortIndexes(dist);
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
            W = pca.project(Map<VectorXf>(target.data(), target.size())); // Flatten image and project onto Eigenfaces
            dist = pca.euclideanDist(W);
            idx = pca.sortIndexes(dist);
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
