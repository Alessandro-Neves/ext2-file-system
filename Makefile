all:
	g++ ./code/main.cpp -o fs.exe

run:
	./fs.exe

types:
	g++ ./tests/types.cpp -o types.exe && ./types.exe && rm -rf types.exe