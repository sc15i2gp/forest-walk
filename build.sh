g++ stack_gen.cpp -o gen_stack_code
L=$(ls *.cpp)
./gen_stack_code $L
make
