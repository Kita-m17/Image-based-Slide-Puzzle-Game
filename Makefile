driver: PGMimage.o TileManager.o main.o
	g++ PGMimage.o TileManager.o main.o -o driver -std=c++20

PGMimage.o: PGMimage.cpp
	g++ -c PGMimage.cpp -o PGMimage.o -std=c++20

TileManager.o: TileManager.cpp
	g++ -c TileManager.cpp -o TileManager.o -std=c++20

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++20

clean:
	rm -r *.o driver
