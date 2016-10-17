#!/bin/bash -e

#mogrify -format png `find . -type f -name "*.pgm"` # Command used to convert all images

# Create 3x3 grid images
cd eigenfaces
convert \( eigenface0.pgm eigenface1.pgm eigenface2.pgm +append \) \
        \( eigenface3.pgm eigenface4.pgm eigenface5.pgm +append \) \
        \( eigenface6.pgm eigenface7.pgm eigenface8.pgm +append \) \
        -append eigenfaces.png

cd ../matches
convert \( match0.pgm match1.pgm match2.pgm +append \) \
        \( match3.pgm match4.pgm match5.pgm +append \) \
        \( match6.pgm match7.pgm match8.pgm +append \) \
        -append matches.png

cd ../

mv eigenfaces/eigenfaces.png .
mv matches/matches.png .
