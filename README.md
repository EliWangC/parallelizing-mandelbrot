# Parallelizing Mandelbrot

>This was a project that I worked on during my undergraduate course.
It implements the sequential and parallel versions of the Mandelbrot
fractal. Of course this is not a novel project idea but I did come up with my own hybrid version of static and dynamic load balancing in the parallel implementation that produced some interesting results as shown in the report.

# Course Instructions

Author: Raymond Pistoresi

Updated: 20 March 2017

To execute the programming assignment successfully the following must exist:

    1. The cloned repo https://github.com/rpistoresi/cs415_Pistoresi.git into the h1 user dir.

    2. cd into cs415_Pistoresi/PA2/build and execute make command.

    3. Jobs can be submitted using sbatch ./<script>, using sequential_1K.sh, static_cores_16.sh, etc.

    4. The output can be shown in terminal by the command: cat <output file>, or simple open the output file.

Notes:
The script files also configure the cluster according to the appropriate hardware desired.
This is also where the image size can be set as command line arguments. Default image size 
is set to 2000x2000 in parallel scripts and sequential scripts are named respective to their image
size. There is also an output file designated for each variation and is listed as --output="a_res_filename.txt"
without quotes in the batch files. Additionally, after the job finishes, a .pgm file is created
and moved into the bin directory. Please note that for each script variation only one unique image file
name is created: sequential.pgm, static.pgm, or dynamic.pgm. Also, run times for each scenario and test are 
recorded with graphs in file Run_Results.xlxs in the bin folder. For more details on this programming 
assignment and its results refer to the Report.pdf file found in this projects root directory.