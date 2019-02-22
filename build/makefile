all: pa2_seq pa2_static pa2_dynamic

pa2_seq: ../src/mandel_seq.cpp ../src/PIMFuncs.cpp
	mpiCC -Wall -lpmi -o pa2_seq ../src/mandel_seq.cpp ../src/PIMFuncs.cpp

pa2_static: ../src/mandel_static.cpp ../src/PIMFuncs.cpp
	mpiCC -Wall -lpmi -o pa2_static ../src/mandel_static.cpp ../src/PIMFuncs.cpp

pa2_dynamic: ../src/mandel_dynamic.cpp ../src/PIMFuncs.cpp
	mpiCC -Wall -lpmi -o pa2_dynamic ../src/mandel_dynamic.cpp ../src/PIMFuncs.cpp

clean:
	-@if rm *.out 2>/dev/null || true; then echo "Cleaned .out files"; else echo "No .out files to clean"; fi