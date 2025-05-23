#ifndef _TILEMANAGER_H
#define _TILEMANAGER_H
#include <vector>
#include <string>
#include "PGMimage.h"

class Tile{
    private:
        int width, height;
        int x, y;
        unsigned char * pixels;

    public:
        Tile(int width, int height, int x_index, int y_index);
        Tile(int width, int height, const unsigned char * imageData, int imageWidth, int x_index, int y_index);
        Tile();
        ~Tile();
        
        int getHeight() const;
        int getWidth() const;
        int getX() const;
        int getY() const;
        void setPixels(int x, int y, unsigned char value);
        void setX(int newX);
        void setY(int newY);
        unsigned char* getPixelArray() const ;
        unsigned char getPixel(int x, int y) const;
        void swapTiles(Tile & tile);
        void clear();
};

class TileManager{
    private:
        int gridSize;
        int numMoves;

        PGMimage image;
        std::string inputImage;
        std::string baseOutputName;

        int imageWidth, imageHeight;
        int tileWidth, tileHeight, lastBlock;

        int newWidth, newHeight;
        std::vector<PGMimage> boardStates;
        int gridArea;
        int blankX, blankY;
        
        Tile ** tiles;
        // int blankX, blankY; //track the unused tile positions
    public:
        TileManager();
        TileManager(int grid_size, int moves, const std::string  & imageIn, const std::string & imageOut);
        ~TileManager();
        
        //load tiles from the buffer into matrix
        void loadTiles(int width, int height);
        void setLastBlock(int newVal);
        void swapTiles(int blankIndex, int tileIndex, unsigned char* &outputdata) ;

        void swapTiles(int x1, int y1, int x2, int y2);


        void saveBoard(std::string outputFile);

        bool moveTile(unsigned char* &outputdata);
        std::vector<int> move(int blankIndex);
        void outputSummaryImage(int numMove, int pad, int space, const std::string & ouputName);

};
#endif