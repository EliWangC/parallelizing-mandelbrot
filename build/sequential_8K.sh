#!/bin/bash
#
#SBATCH --job-name=pa2_seq
#SBATCH --output=seq_res_8K.txt
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --time=00:05:00
#
srun pa2_seq 8000 8000
#
mv sequential.pgm ../bin