
run: compile
	./bin/hashes

compile: src/hashes.cpp src/ordered_hash.h
	g++ -I /home/cheater/workspace/libs/boost_1_55_0/ src/hashes.cpp -o bin/hashes -O2 -Wall -Wfatal-errors -std=c++11

clear:
	rm -rf bin/*

test:
	g++ tests/test.cpp -o bin/test -O2 -Wall -std=c++11

test-titanic:
	g++ tests/titanic/titanic-ordered-hash.cpp -o bin/titanic-ordered-hash -O2 -Wall -std=c++11
