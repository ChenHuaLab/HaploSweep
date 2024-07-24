#!/bin/bash

mkdir out_neutral
./HaploSweep calc -i testdata/neutral.vcf.gz -m testdata/neutral.map -t 8 -o1 out_neutral/result.out
./HaploSweep norm -dir out_neutral -t 1 -out result_neutral_norm.out

mkdir out_sweep
./HaploSweep calc -i testdata/sweep.vcf.gz -m testdata/sweep.map -t 8 -o1 out_sweep/result.out
./HaploSweep norm -dir out_sweep -t 1 -out result_sweep_norm.out
./HaploSweep calc -i testdata/sweep.vcf.gz -m testdata/sweep.map -region 10000000-10000000 -t 1 -o2 result_sweep.detail.out