#!/bin/bash -e

#mogrify -format png `find . -type f -name "*.pgm"` # Command used to convert all images

# Create 3x3 grid images
cd eigenfaces
convert \( eigenface0.pgm eigenface1.pgm eigenface2.pgm +append \) \
        \( eigenface3.pgm eigenface4.pgm eigenface5.pgm +append \) \
        \( eigenface6.pgm eigenface7.pgm eigenface8.pgm +append \) \
        -append eigenfaces.png

cd ../matches_eigenfaces
convert \( match0.pgm match1.pgm match2.pgm +append \) \
        \( match3.pgm match4.pgm match5.pgm +append \) \
        \( match6.pgm match7.pgm match8.pgm +append \) \
        -append matches_eigenfaces.png

cd ../fisherfaces
convert \( fisherface0.pgm fisherface1.pgm fisherface2.pgm +append \) \
        \( fisherface3.pgm fisherface4.pgm fisherface5.pgm +append \) \
        \( fisherface6.pgm fisherface7.pgm fisherface8.pgm +append \) \
        -append fisherfaces.png

cd ../matches_fisherfaces
convert \( match0.pgm match1.pgm match2.pgm +append \) \
        \( match3.pgm match4.pgm match5.pgm +append \) \
        \( match6.pgm match7.pgm match8.pgm +append \) \
        -append matches_fisherfaces.png

cd ../

mv eigenfaces/eigenfaces.png img/eigenfaces.png
mv matches_eigenfaces/matches_eigenfaces.png img/matches_eigenfaces.png
mv fisherfaces/fisherfaces.png img/fisherfaces.png
mv matches_fisherfaces/matches_fisherfaces.png img/matches_fisherfaces.png
