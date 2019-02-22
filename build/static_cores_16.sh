#!/bin/bash
#
#SBATCH --job-name=pa2_static
#SBATCH --output=stat_res_cores_16.txt
#SBATCH --ntasks=16
#SBATCH --nodes=2
#SBATCH --time=00:05:00
#
srun pa2_static 2000 2000
#
mv static.pgm ../bin