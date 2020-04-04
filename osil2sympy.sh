#!/bin/bash

filename=$(dirname $1)/$(basename $1 .osil)

./osil2smt.py $1
./smt2sympy.py $filename.smt2

rm $filename.smt2
