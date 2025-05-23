#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "TileManager.h"
#include <algorithm>
#include <random> 

class PGMimage;

Tile::Tile(): width(0), height(0), x(0), y(0), pixels(nullptr){}

Tile::Tile(int width, int height, const unsigned char * data, int imageWidth, int x_index, int y_index): width(width), height(height), x(x_index), y(y_index){
    pixels = new unsigned char[width*height];
    
    for(int i = 0; i<height; ++i){
        for(int j = 0; j<width; ++j){
            int srcY = (y_index*height)+i;
            int srcX = (x_index*width)+j;
            setPixels(j, i, data[srcY*imageWidth+srcX]);
        }
    }
}

Tile::~Tile () {
    delete [] pixels;
}

unsigned char * Tile::getPixelArray() const { return pixels; }
int Tile::getHeight() const { return height; }
int Tile::getWidth() const {return width ;}
int Tile::getX() const {return x;}
int Tile::getY() const {return y;}
unsigned char Tile::getPixel(int x, int y) const {
    return pixels[y*width+x];
}

void Tile::setX(int newX) { x= newX;}
void Tile::setY(int newY) { y = newY;}
void Tile::setPixels(int x, int y, unsigned char value){
    pixels[y*width+x] = value;
}

void Tile::swapTiles(Tile & tile){
    std::swap(x, tile.x);
    std::swap(y, tile.y);
    std::swap(width, tile.width);
    std::swap(height, tile.height);
    std::swap(pixels, tile.pixels);
}

void Tile::clear(){
    if(pixels != nullptr){
        for(int i = 0; i < (width*height); i++){
            pixels[i] = 0;
        }
    }
}

//-----------------------------------------------------------------------------------//

TileManager::TileManager() : gridSize(0), tileHeight(0), tileWidth(0), newHeight(0), newWidth(0), tiles(nullptr) {}

TileManager::TileManager(int grid_size, int moves, const std::string  & imageIn, const std::string & imageOut) : gridSize(grid_size), numMoves(moves), baseOutputName(imageOut), inputImage(imageIn){
    image.read(inputImage);
    image.getDims(imageWidth, imageHeight);

    gridArea = gridSize*gridSize;
    // Initialize random seed
    //std::srand(static_cast<unsigned int>(std::time(nullptr)));

    tileWidth = imageWidth/gridSize;
    tileHeight = imageHeight/gridSize;
    lastBlock = gridArea-1;
    
    newWidth = tileWidth*gridSize;
    newHeight = tileHeight*gridSize;

    loadTiles(imageWidth, imageHeight);
    saveBoard(baseOutputName + "_0");

} 
    

TileManager::~TileManager(){
    if(tiles){
        for (int i = 0; i < gridSize*gridSize; ++i) {
            delete tiles[i];
        }
        delete [] tiles;
        tiles = nullptr;
    }

}

void TileManager::setLastBlock(int newVal){lastBlock = newVal;}

void TileManager::loadTiles(int width, int height) {

    const unsigned char *data = image.getBuffer();
    boardStates = {};
    tiles = new Tile * [gridArea];

    for (int r = 0; r < gridSize; ++r) {
        for (int c = 0; c < gridSize; ++c) {

            tiles[r*gridSize+c] = new Tile(tileWidth, tileHeight, data, imageWidth, c, r);
            if(r == gridSize - 1 && c == gridSize - 1){
                tiles[r*gridSize+c] -> clear();
                continue;
            }
        }
    }
}

void TileManager::saveBoard(std::string outputFile){
    //output data
    unsigned char* outputData = new unsigned char[newWidth * newHeight];
    
    // Initialize all pixels to 0 (black)
    std::memset(outputData, 0, newWidth * newHeight * sizeof(unsigned char));

    for(int r = 0; r < gridSize; ++r){
        for(int c = 0; c < gridSize; ++c){
            //get the tile at the current position
            Tile* tile = tiles[r * gridSize + c];
            
            //skip if this is a null tile (if blank tile)
            if (tile == nullptr) continue;
            
            //copy pixels from the tile to the output data
            for(int i = 0; i < tileHeight; ++i){
                for(int j = 0; j < tileWidth; ++j){
                    int x_index = c * tileWidth + j;
                    int y_index = r * tileHeight + i;
                    int outputIndex = (y_index * newWidth) + x_index;
                    int bufferIndex = (i * tileWidth) + j;

                    if (outputIndex < newWidth * newHeight && bufferIndex < tileWidth * tileHeight) {
                        outputData[outputIndex] = tile->getPixel(j, i);
                    }
                }
            }
        }
    }
    
    //save output image
    PGMimage outputImage;

    outputImage.setImageData(outputData, newWidth, newHeight);
    outputImage.write(outputFile + ".pgm");  //add .pgm extension if not present
    //boardStates.push_back(outputImage);
    //free memory
    delete[] outputData;
}

void TileManager::swapTiles(int blankIndex, int tileIndex, unsigned char* &outputdata) {
    // Ensure indices are valid
    if (blankIndex < 0 || blankIndex >= gridArea || tileIndex < 0 || tileIndex >= gridArea) {
        std::cerr << "Invalid swap indices: " << blankIndex << " <-> " << tileIndex << std::endl;
        return;
    }

    // Swap tile pointers
    std::swap(tiles[blankIndex], tiles[tileIndex]);

    // Update output image buffer to reflect the swap
    for (int i = 0; i < tileHeight; ++i) {
        for (int j = 0; j < tileWidth; ++j) {
            int blankX = (blankIndex % gridSize) * tileWidth + j;
            int blankY = (blankIndex / gridSize) * tileHeight + i;
            int tileX = (tileIndex % gridSize) * tileWidth + j;
            int tileY = (tileIndex / gridSize) * tileHeight + i;

            // Swap pixel values in the output buffer
            std::swap(outputdata[blankY * newWidth + blankX], outputdata[tileY * newWidth + tileX]);
        }
    }
}


bool TileManager::moveTile(unsigned char* &outputdata) {

    if (!outputdata) {
        outputdata = new unsigned char[newWidth * newHeight];
        std::memset(outputdata, 0, newWidth * newHeight * sizeof(unsigned char));
    }

    std::vector<int> validMoves = move(lastBlock);
    
    if (!validMoves.empty()) {
        std::random_device seed;        
        std::mt19937 generator(seed());
        std::shuffle(validMoves.begin(), validMoves.end(), generator);

        int selectedIndex = validMoves.front(); // Pick the first shuffled valid move
        swapTiles(lastBlock, selectedIndex, outputdata);
        setLastBlock(selectedIndex);
        return true;

    }
    return false;
}

std::vector<int> TileManager::move(int blankIndex){
    std::vector<int> validMoves = {};

    //heck up
    if (blankIndex - gridSize >= 0) {
        validMoves.push_back(blankIndex - gridSize);
    }
    //check down
    if (blankIndex + gridSize < gridArea) { 
        validMoves.push_back(blankIndex + gridSize);
    }
    //check left 
    if (blankIndex % gridSize != 0) {  
        validMoves.push_back(blankIndex - 1);
    }
    //check right
    if (blankIndex % gridSize != gridSize - 1) {  
        validMoves.push_back(blankIndex + 1);
    }

    return validMoves;
}

void TileManager::outputSummaryImage(int numMove, int pad, int space, const std::string &outputIName) {
    
    PGMimage summaryImage;
    if (boardStates.empty()) {
        std::cerr << "No images to compile!" << std::endl;
        return;
    }

    int col = ceil(sqrt(boardStates.size()));
    int row = ceil(static_cast<double>(boardStates.size()) / col);
    int totalHeight = (2 * pad) + (newHeight * row) + (space * (row - 1));
    int totalWidth = (2 * pad) + (newWidth * col) + (space * (col - 1));

    std::cout << "Compiling images... Grid: " << col << "x" << row << std::endl;

    // Allocate output buffer
    unsigned char *outputData = new unsigned char[totalWidth * totalHeight];
    std::memset(outputData, 255, totalWidth * totalHeight );  // White background

    // Copy images into buffer
    for (size_t imageIndex = 0; imageIndex < boardStates.size(); ++imageIndex) {
        if (!boardStates[imageIndex].getBuffer()) {
            std::cerr << "Error: Null buffer for image " << imageIndex << std::endl;
            delete[] outputData;
            return;
        }
        
        int r = imageIndex / col;
        int c = imageIndex % col;
        int y_index = pad + r * (newHeight + space);
        int x_index = pad + c * (newWidth + space);
        
        const unsigned char *imgBuffer = boardStates[imageIndex].getBuffer();
        if (!imgBuffer) {
            std::cerr << "Error: Image buffer is null for image " << imageIndex << std::endl;
            continue;
        }

        std::cout << "Placing image " << imageIndex << " at (" << x_index << ", " << y_index << ")" << std::endl;

        for (int row = 0; row < newHeight; ++row) {
            for (int col = 0; col < newWidth; ++col) {
                int outputIndex = (y_index + row) * totalWidth + (x_index + col);
                int bufferIndex = row * newWidth + col;
                
                if (outputIndex < totalWidth * totalHeight && bufferIndex < newWidth * newHeight) {
                    outputData[outputIndex] = imgBuffer[row * newWidth + col];
                } else {
                    std::cerr << "Skipping out-of-bounds pixel at output index " << outputIndex << std::endl;
                }
            }
        }
    }

    // Save output
    std::cout << "Writing summary image..." << std::endl;
    PGMimage outputImage;
    outputImage.setImageData(outputData, totalWidth, totalHeight);
    outputImage.write("compiled_" + outputIName + std::to_string(numMove) + ".pgm");

    delete[] outputData;
}
