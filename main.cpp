#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "TileManager.h"
#include "PGMimage.h"
#include <algorithm>
#include <random> 
int main(int argc, char *argv[]) {
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " -s <grid_size> -o <output_base_name> -n <numMoves> inputImageName" << std::endl;
        return 1;
    }

    // Get command-line parameters
    int gridSize = 0, numMoves = 0;
    std::string outputBaseName, inputImage;

    for (int i = 1; i < argc - 1; i++) {
        std::string arg = argv[i];
        if (arg == "-s") {
            gridSize = std::stoi(argv[++i]);
            if (gridSize % 2 == 0 || gridSize < 3) {
                std::cerr << "Error: Grid size must be an odd integer ≥ 3." << std::endl;
                return 1;
            }
        } else if (arg == "-o") {
            outputBaseName = argv[++i];
        } else if (arg == "-n") {
            numMoves = std::stoi(argv[++i]);
            if (numMoves < 1) {
                std::cerr << "Error: Number of moves must be ≥ 1." << std::endl;
                return 1;
            }
        }
    }
    inputImage = argv[argc - 1];

    // Load the input image
    PGMimage image;
    image.read(inputImage);
    
    int imageWidth, imageHeight;
    image.getDims(imageWidth, imageHeight);

    // Initialize TileManager
    TileManager manager(gridSize, numMoves, inputImage, outputBaseName);
    // Save initial board state
    manager.saveBoard(outputBaseName + "_0.pgm");

    // // Move the tile `numMoves` times
    for (int i = 1; i <= numMoves; ++i) {
        unsigned char* outputData = nullptr;
        if (manager.moveTile(outputData)) { // Move tile and update board
            manager.saveBoard(outputBaseName + "_" + std::to_string(i));
        } else {
            std::cerr << "Error: No valid move available at step " << i << std::endl;
            break;
        }
        
    }
    //manager.compileAll(numMoves, 2 , 2, outputBaseName);


    return 0;
}
