all:
	g++ ./code/main.cpp ./code/file-operations.cpp ./code/superblock-operations.cpp ./code/blocks-group-operations.cpp ./code/inode-operations.cpp ./code/directory-operations.cpp ./code/util-operations.cpp ./code/ext2-file-manager.cpp  -o fs.exe
	clear
	./fs.exe

compile:
	g++ ./code/main.cpp ./code/file-operations.cpp ./code/superblock-operations.cpp ./code/blocks-group-operations.cpp ./code/inode-operations.cpp ./code/directory-operations.cpp ./code/util-operations.cpp  ./code/ext2-file-manager.cpp -o fs.exe

run:
	./fs.exe

types:
	g++ ./tests/types.cpp -o types.exe && ./types.exe && rm -rf types.exe

image-restore:
	rm -rf ./ext2.img
	cp ./image/ext2.img ./

clear:
	rm -rf *.o
	rm -rf *.exe