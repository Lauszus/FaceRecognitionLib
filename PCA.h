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

#ifndef _pca_h_
#define _pca_h_

#include <numeric>
#include <vector>

#include <Eigen/Dense> // http://eigen.tuxfamily.org

using namespace std;
using namespace Eigen;

class PCA {
public:
    PCA(){};

    void train(MatrixXf &images);
    MatrixXf project(const MatrixXf &X);
    MatrixXf reconstructFace(const MatrixXf &W);
    VectorXf euclideanDist(const VectorXf &W);

    template<typename VectorType>
    vector<size_t> sortIndexes(const VectorType &v) {
        // Based on: http://stackoverflow.com/a/12399290/2175837
        // Initialize original index locations
        vector<size_t> idx(v.size());
        iota(idx.begin(), idx.end(), 0);

        // Sort indexes based on comparing values in v
        sort(idx.begin(), idx.end(),
            [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

        return idx;
    }

    MatrixXf U; // Eigenfaces
    W_all, face_all; // Total weights and faces
    VectorXf mu; // Mean along each row

    const float cumulativeEnergyThreshold = .9f; // Determine the number of principal components required to model 90 % of data variance
    uint16_t K = 15; // Number of singular values used, this might change based on cumulative energy threshold

private:
    uint16_t n_pixels, n_images;
};

#endif
