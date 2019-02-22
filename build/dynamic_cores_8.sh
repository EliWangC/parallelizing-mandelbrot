#!/bin/bash
#
#SBATCH --job-name=pa2_dynamic
#SBATCH --output=dyn_res_cores_8.txt
#SBATCH --ntasks=8
#SBATCH --nodes=1
#SBATCH --time=00:05:00
#
srun pa2_dynamic 2000 2000
#
mv dynamic.pgm ../bin