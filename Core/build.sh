read -p "gen stack code? " yn
case $yn in
	[Yy]* )
		g++ stack_gen.cpp -o gen_stack_code
		L=$(ls *.cpp)
		./gen_stack_code $L ;;
esac
make
