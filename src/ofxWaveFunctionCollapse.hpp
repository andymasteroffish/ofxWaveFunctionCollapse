//
//  WaveFunctionCollapse.hpp
//
//  Created by Andrew Wallace on 11/22/19.
//

//Wave Function Collapse Algorithm created by Maxim Gumin
//https://twitter.com/ExUtumno
//https://github.com/mxgmn

#ifndef ofxWaveFunctionCollapse_hpp
#define ofxWaveFunctionCollapse_hpp

#include "ofMain.h"
#include "Tile.h"
#include "CheckPoint.h"
#include "PotentialTile.hpp"

class ofxWaveFunctionCollapse{
public:
    
    void setup();
    
    void resizeOutput(int newCols, int newRows);
    
    void reset();
    void resetOutput();
    void doFirstMove();
    
    void setSourceFromFile(string path, int cols, int rows);
    void setSourceFromString(string map, int cols, int rows);
    void setSourceFrom2DVector(vector< vector<int> > &map);
    void generateSourceTiles();
    void setNeightborInfo();
    
    void advance();
    vector<NeighborInfo> getTileChoicesWithFreq(int col, int row);
    void updateBoardFromMove(CheckPoint * point);
    void validateBoard();
    
    void revertToCheckPoint(CheckPoint * point);
    
    vector< vector<int> > get_int_map();
    void printOutput();
    
    vector<Tile> sourceTiles;
    
    int sourceCols, sourceRows;
    vector< vector<int> > sourceImage;  //RENAME THIS SOURCE MAP
    
    int outputCols, outputRows;
    vector< vector<PotentialTile> > outputImage;    //RENAME THIS OUTPUT MAP
    
    CheckPoint * rootMove;
    CheckPoint * curMove;
    
    bool needFirstMove;
    bool needToGetNeighborInfo;
    bool isDone;
    
    bool useFreq;
    float freqWeight;
    
};

#endif /* ofxWaveFunctionCollapse_hpp */
