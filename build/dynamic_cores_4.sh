#!/bin/bash
#
#SBATCH --job-name=pa2_dynamic
#SBATCH --output=dyn_res_cores_4.txt
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --time=00:05:00
#
srun pa2_dynamic 2000 2000
#
mv dynamic.pgm ../bin