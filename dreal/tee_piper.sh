#!/bin/bash

# tee dreal_in.log | /homes/ac4014/dreal_temp/piper | tee dreal_out.log

id=$(cat /data/deltaopt/smt-optimization/dreal/piperid)

echo $(( (id + 1) % 3 + 1 )) > /data/deltaopt/smt-optimization/dreal/piperid

tee dreal_in_$id.log | /data/deltaopt/smt-optimization/dreal/piper > >(tee dreal_out_$id.log) 2>dreal_err_$id.log
