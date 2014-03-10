
run: bin/hashes
	./bin/hashes

bin/hashes: src/hashes.cpp src/ordered_hash.h
	g++ src/hashes.cpp -o bin/hashes -O2 -Wall -Wfatal-errors -std=c++11

