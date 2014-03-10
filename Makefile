
run: compile
	./bin/hashes

compile: src/hashes.cpp src/ordered_hash.h
	g++ -I /home/cheater/workspace/libs/boost_1_55_0/ src/hashes.cpp -o bin/hashes -Wall -Wfatal-errors -std=c++11

clear:
	rm -rf bin/*

