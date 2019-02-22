#!/bin/bash
#
#SBATCH --job-name=pa2_static
#SBATCH --output=stat_res_cores_32.txt
#SBATCH --ntasks=32
#SBATCH --nodes=4
#SBATCH --time=00:05:00
#
srun pa2_static 2000 2000
#
mv static.pgm ../bin