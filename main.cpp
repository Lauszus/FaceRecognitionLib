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
#include <algorithm>

#include <Eigen/Dense> // http://eigen.tuxfamily.org

#include "Eigenfaces.h"
#include "Facebase.h"
#include "Fisherfaces.h"
#include "pgm.h"
#include "Tools.h"

Eigenfaces eigenfaces;
Fisherfaces fisherfaces;

using namespace std;
using namespace Eigen;

// Based on: http://jmcspot.com/Eigenface
// Images are from the AT&T Facedatabase: http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html

// See: http://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
static IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");

#if 1 // AT&T Facedatabase
static const char *facePath = "../orl_faces";
static const uint8_t N = 92, M = 112;
static const uint16_t n_pixels = M*N;
static const uint8_t n_person = 40;
static const uint8_t n_img_pr_person = 10;
#else // Yale Face Database
static const char *facePath = "../yalefaces";
static const uint16_t N = 320, M = 243;
static const uint32_t n_pixels = M*N;
static const uint8_t n_person = 15;
static const uint8_t n_img_pr_person = 11;
#endif

static const uint16_t n_images = n_img_pr_person*n_person;

static MatrixXi readPgmAsMatrix(const char *filename) {
    PGMImage imgRaw;
    getPGMfile(filename, &imgRaw);
    MatrixXi img(imgRaw.height, imgRaw.width);
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

static void trainFaces(MatrixXi &images) {
    cout << "Loading images" << endl;
    images = MatrixXi(n_pixels, n_images); // Pre-allocate storage for images
    VectorXi classes = VectorXi(n_images); // Create class vector
    cout << "images: " << images.rows() << " x " << images.cols() << endl << endl;

    // Load all images into the matrix
    for (int i = 0; i < n_person; i++) {
        for (int j = 0; j < n_img_pr_person; j++) {
            char filename[50];
            sprintf(filename, "%s/s%u/%u.pgm", facePath, i + 1, j + 1);
            MatrixXi img = readPgmAsMatrix(filename);
            const size_t index = i*n_img_pr_person + j;
            images.block<n_pixels, 1>(0, index) = Map<VectorXi>(img.data(), img.size()); // Flatten image
            classes(index) = i + 1; // Generate class number for each person
        }
    }

    eigenfaces.train(images);
    cout << "eigenfaces: " << eigenfaces.V.rows() << " x " << eigenfaces.V.cols() << endl;

    mkdir("eigenfaces", 0755);
    for (int i = 0; i < eigenfaces.numComponents; i++) { // Save Eigenfaces as PGM images
        Map<MatrixXf> eigenface(eigenfaces.V.block<n_pixels, 1>(0, i).data(), M, N); // Extract Eigenface
        //cout << "Eigenface: " << eigenface.rows() << " x " << eigenface.cols() << endl;
        char filename[50];
        sprintf(filename, "eigenfaces/eigenface%u.pgm", i);
        saveMatrixAsPgm(filename, eigenface); // Save Eigenface
    }

    cout << "Done training Eigenfaces" << endl << endl;

    fisherfaces.train(images, classes);
    cout << "fisherfaces: " << fisherfaces.V.rows() << " x " << fisherfaces.V.cols() << endl;

    mkdir("fisherfaces", 0755);
    for (int i = 0; i < fisherfaces.numComponents; i++) { // Save Fisherfaces as PGM images
        Map<MatrixXf> fisherface(fisherfaces.V.block<n_pixels, 1>(0, i).data(), M, N); // Extract Fisherface
        //cout << "Fisherface: " << fisherface.rows() << " x " << fisherface.cols() << endl;
        char filename[50];
        sprintf(filename, "fisherfaces/fisherface%u.pgm", i);
        saveMatrixAsPgm(filename, fisherface); // Save Fisherface
    }

    cout << "Done training Fisherfaces" << endl << endl;
}

void calculateMatches(MatrixXi &target, const MatrixXi &images, Facebase &facebase, const char *dirName) {
    cout << "target: " << target.rows() << " x " << target.cols() << endl;

    cout << "Reconstructing Faces" << endl;
    VectorXi target_flat = Map<VectorXi>(target.data(), target.size()); // Flatten image
    VectorXf W = facebase.project(target_flat); // Project onto subspace
    //cout << W.format(OctaveFmt) << endl;
    VectorXf face = facebase.reconstructFace(W);

    cout << "Calculate normalized Euclidean distance" << endl;
    float dist_face = facebase.euclideanDistFace(target_flat, face);
    cout << "Face distance: " << dist_face << endl;

    VectorXf dist = facebase.euclideanDist(W);
    //cout << "dist: " << dist.rows() << " x " << dist.cols() << endl;

    vector<size_t> idx = sortIndexes(dist);
    //for (auto i : idx) cout << "dist[" << i << "]: " << dist(i) << endl;

    char filename[50];
    mkdir(dirName, 0755);
    for (int i = 0; i < min(dist.size(), 9L); i++) { // Save first nine matches
        cout << "dist[" << idx[i] << "]: " << dist(idx[i]) << endl;
        sprintf(filename, "%s/match%u.pgm", dirName, i);
        MatrixXi img = images.block<n_pixels, 1>(0, idx[i]);
        //cout << "img: " << img.rows() << " x " << img.cols() << endl;
        saveMatrixAsPgm(filename, Map<MatrixXi>(img.data(), M, N).cast<float>()); // Save matched image
    }
    sprintf(filename, "%s/face.pgm", dirName);
    saveMatrixAsPgm(filename, Map<MatrixXf>(face.data(), M, N)); // Save face image
}

int main(void) {
    MatrixXi images;
    trainFaces(images);

    MatrixXi target = readPgmAsMatrix("../orl_faces/s3/8.pgm"); // Load a random image from the database
    //MatrixXi target = readPgmAsMatrix("img/bart.pgm"); // Used to test face distance
    //MatrixXi target = readPgmAsMatrix("../yalefaces/s1/4.pgm"); // Load image with light coming from the left side

    cout << "Calculating matches using Eigenfaces" << endl;
    calculateMatches(target, images, eigenfaces, "matches_eigenfaces"); // Calculate matches based on Eigenfaces

    cout << endl << "Calculating matches using Fisherfaces" << endl;
    calculateMatches(target, images, fisherfaces, "matches_fisherfaces"); // Calculate matches based on Fisherfaces

#if 0
    cout << endl << "Testing all faces against database" << endl;
    bool allMatched = true;
    for (int i = 0; i < n_person; i++) {
        for (int j = 0; j < n_img_pr_person; j++) {
            char filename[50];
            sprintf(filename, "%s/s%u/%u.pgm", facePath, i + 1, j + 1);
            target = readPgmAsMatrix(filename);
            VectorXf W = eigenfaces.project(Map<VectorXi>(target.data(), target.size())); // Flatten image and project onto Eigenfaces
            VectorXf dist = eigenfaces.euclideanDist(W);
            vector<size_t> idx = sortIndexes(dist);
            //cout << "dist[" << idx[0] << "]: " << dist(idx[0]) << endl;
            const size_t index = i*n_img_pr_person + j;
            if (idx[0] != index) {
                cout << "Wrong match: " << index << " should be: " << idx[0] << endl;
                allMatched = false;
            }
        }
    }
    if (allMatched)
        cout << "All faces matched!" << endl;
#endif

    return 0;
}
