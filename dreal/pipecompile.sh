#!/bin/bash

# pkg-config ibex --libs

# Removed option -Wall
g++ -O3 -march=native -flto -DNDEBUG -o piper --std=c++14 piper.cpp sexpr_parser.cpp -lboost_regex -pthread `pkg-config dreal --cflags --libs`


