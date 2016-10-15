#!/bin/bash

mogrify -format png `find . -type f -name "*.pgm"`
