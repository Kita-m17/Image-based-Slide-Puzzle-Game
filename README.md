This program generates a slide puzzle game from a PGM image. The image is divided into a grid, with one tile left empth to allow for movement.

Compiling
To compile the program, use the following command:
make
This will build the driver executable

Usage
Run the program with the following command-line arguments:
./driver -s <grid_size> -o <output_base_name> -n <numMoves> inputImageName

Where:
-s <grid_size>: Specifies the grid size (must be an odd integer ≥ 3).
-o <output_base_name>: Sets the base name for output PGM images.
-n <numMoves>: Number of tile moves to generate (must be ≥ 1).

inputImageName: The input PGM image file.
Eg:
./driver -s 3 -o puzzle -n 5 input/mona512.pgm

Code Structure
TileManager.h/.cpp: Contails noth the Tile class and TileManager classes. Manages the tile grid, performs moves, and saves board states.
PGMimage.cpp: Handles reading/writing PGM images.
driver.cpp: The main entry point of the program.
Makefile: Automates compilation.

Implementation Notes
Moves are valid only if adjacent tiles slide into the empty space.
Uses std::random_device and std::shuffle to randomize moves.

Output images follow the format <output_base_name>_N.pgm.
