//
//  WaveFunctionCollapse.cpp
//
//  Created by Andrew Wallace on 11/22/19.
//

//Wave Function Collapse Algorithm created by Maxim Gumin
//https://twitter.com/ExUtumno
//https://github.com/mxgmn

#include "ofxWaveFunctionCollapse.hpp"



//--------------------------------------------------------------
void ofxWaveFunctionCollapse::setup(){
    rootMove = new CheckPoint(NULL);
    
    useFreq = true;
    freqWeight = 0.6f;
}


//--------------------------------------------------------------
void ofxWaveFunctionCollapse::resizeOutput(int newCols, int newRows){
    outputCols = MAX(newCols,2);
    outputRows = MAX(newRows,2);
    
    for (int i=0; i<outputImage.size(); i++){
        outputImage[i].clear();
    }
    outputImage.clear();
    
    outputImage.resize(newCols);
    for (int i=0; i<outputImage.size(); i++){
        outputImage[i].resize(newRows);
    }
    
    
    resetOutput();
    needToGetNeighborInfo = true;
    needFirstMove = true;
    
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::reset(){
    setNeightborInfo();
    resetOutput();
    //doFirstMove();
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::resetOutput(){
    for (int x=0; x<outputCols; x++){
        for (int y=0; y<outputRows; y++){
            outputImage[x][y].reset(sourceTiles.size(), x, y);
        }
    }
    isDone = false;
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::doFirstMove(){
    cout<<"do first move"<<endl;
    needFirstMove = false;
    //start us off
    rootMove->prune();
    curMove = new CheckPoint(rootMove);
    curMove->move(ofRandom(outputCols), ofRandom(outputRows), ofRandom(sourceTiles.size())) ;
    updateBoardFromMove(curMove);
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::setSourceFromFile(string path, int cols, int rows){
    cout<<"loading "<<path<<endl;
    ofBuffer buffer = ofBufferFromFile(path);
    string file_text = "";
    if(buffer.size()) {

        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {

            string line = *it;
            cout<<line<<endl;
            file_text += line+"\n";
        }
    }

    setSourceFromString(file_text, cols, rows);
}
//--------------------------------------------------------------
void ofxWaveFunctionCollapse::setSourceFromString(string map, int cols, int rows){
    
    //resize the source image
    sourceCols = cols;
    sourceRows = rows;
    
    sourceImage.resize(cols);
    for (int i=0; i< cols; i++){
        sourceImage[i].resize(rows);
    }
    
    //go through and set everything
    int curX = 0;
    int curY = 0;
    cout<<map<<endl;
    for (int i=0; i<map.length(); i++){
        if (map[i] == ','){
            curX++;
        }
        else if (map[i] == '\n'){
            curX=0;
            curY++;
        }
        else {
            string numString = "x";
            numString[0] = map[i];
            
            if (map[i+1] != ','){
                numString += map[i+1];
                i++;
            }
            
            int idNum = ofToInt(numString);
            //cout<<curX<<","<<curY<<" id: "<<idNum<<endl;
            if (curX < sourceCols){    //my source image was fucked up so I'm getting rid of the last column
                //cout<<"do it"<<endl;
                sourceImage[curX][curY] = idNum;
            }
        }
    }
    
    //testing
    cout<<"source map:"<<endl;
    for (int y=0; y<sourceRows; y++){
        for (int x=0; x<sourceCols; x++){
            //cout<<x<<","<<y<<endl;
            cout<<sourceImage[x][y];
        }
        cout<<endl;
    }
    
    //grab source tiles from this map
    generateSourceTiles();
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::setSourceFrom2DVector(vector< vector<int> > &map){
    //resize source map
    sourceCols = map.size();
    sourceRows = map[0].size();
    
    sourceImage.resize(sourceCols);
    for (int i=0; i< sourceCols; i++){
        sourceImage[i].resize(sourceRows);
    }
    
    
    cout<<"this baby is "<<sourceCols<<" x "<<sourceRows<<endl;
    
    //go through and set everything
    int curX = 0;
    int curY = 0;
    
    for (int x=0; x<sourceCols; x++){
        for (int y=0; y<sourceRows; y++){
            sourceImage[x][y] = map[x][y];
        }
    }
    
    
    //testing
    cout<<"source map:"<<endl;
    for (int y=0; y<sourceRows; y++){
        for (int x=0; x<sourceCols; x++){
            //cout<<x<<","<<y<<endl;
            cout<<sourceImage[x][y];
        }
        cout<<endl;
    }
    
    //grab source tiles from this map
    generateSourceTiles();
    
}

//--------------------------------------------------------------
//right now, this creates a source tile for every int from 0 to the max ID. This is kind of clunky
void ofxWaveFunctionCollapse::generateSourceTiles(){
    //clear out what's there
    for (int i=0; i<sourceTiles.size(); i++){
        sourceTiles[i].resetNeighborInfo();
    }
    sourceTiles.clear();
    
    //get the max id
    int max_val = 0;
    for (int x=0; x<sourceCols; x++){
        for (int y=0; y<sourceRows; y++){
            
            if (sourceImage[x][y] > max_val){
                max_val = sourceImage[x][y];
            }
        }
    }
    
    for (int i=0; i<=max_val; i++){
        Tile tile;
        tile.idNum = i;
        sourceTiles.push_back(tile);
    }
    
//    //find unique tile IDs
//    for (int x=0; x<sourceCols; x++){
//        for (int y=0; y<sourceRows; y++){
//
//            int this_id = sourceImage[x][y];
//
//            bool is_new = true;
//            for (int i=0; i<sourceTiles.size(); i++){
//                if (sourceTiles[i].idNum == this_id){
//                    is_new = false;
//                    break;
//                }
//            }
//
//            if (is_new){
//                Tile tile;
//                tile.idNum = this_id;
//                sourceTiles.push_back(tile);
//            }
//
//        }
//    }
    
    //tetsing
    cout<<"unique tiles:"<<endl;
    for (int i=0; i<sourceTiles.size(); i++){
        cout<<i<<":"<<sourceTiles[i].idNum<<endl;
    }
    
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::setNeightborInfo(){
    needToGetNeighborInfo = false;
    
    for (int i=0; i<sourceTiles.size(); i++){
        sourceTiles[i].resetNeighborInfo();
    }
    
    for (int x=0; x<sourceCols; x++){
        for (int y=0; y<sourceRows; y++){
            int id = sourceImage[x][y];
            
            //cout<<"setting neighbors for "<<x<<","<<y<<endl;
            
            //check to the north
            if (y>0){
                sourceTiles[id].noteNeighbor(0, sourceImage[x][y-1]);
            }
            
            //check to the east
            if (x<sourceCols-1){
                sourceTiles[id].noteNeighbor(1, sourceImage[x+1][y]);
            }
            
            //check to the south
            if (y<sourceRows-1){
                sourceTiles[id].noteNeighbor(2, sourceImage[x][y+1]);
            }
            
            //check to the west
            if (x>0){
                sourceTiles[id].noteNeighbor(3, sourceImage[x-1][y]);
            }
        }
    }
    
    
    //testing info
    string labels[4] = {"North", "East", "South", "West" };
    for (int i=0; i<sourceTiles.size(); i++){
        cout<<"tile "<<i<<endl;
        for (int dir=0; dir<4; dir++){
            cout<<" "<<labels[dir]<<endl;
            for (int k=0; k<sourceTiles[i].neighbors[dir].size(); k++){
                cout<<"  "<<sourceTiles[i].neighbors[dir][k].idNum<<":"<<sourceTiles[i].neighbors[dir][k].freq<<endl;
            }
        }
    }
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::advance(){
    if (isDone){
        return;
    }
    
    if (needToGetNeighborInfo){
        cout<<"need that neighbor info"<<endl;
        setNeightborInfo();
    }
    if (needFirstMove){
        cout<<"do that first move"<<endl;
        doFirstMove();
        return;
    }
    
    //cout<<"advance"<<endl;
    CheckPoint * oldMove = curMove;
    curMove = new CheckPoint(oldMove);
    
    //cout<<"move "<<curMove->getDepth()<<endl;
    
    //make a list of the active potential tiles with the fewest posibilits
    int lowVal = sourceTiles.size()+1;
    for (int x=0; x<outputCols; x++){
        for (int y=0; y<outputRows; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() < lowVal){
                lowVal = outputImage[x][y].potentialIDs.size();
            }
        }
    }
    
    vector<PotentialTile> choices;
    for (int x=0; x<outputCols; x++){
        for (int y=0; y<outputRows; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() == lowVal){
                choices.push_back(outputImage[x][y]);
            }
        }
    }
    
    if (choices.size() == 0){
        cout<<"all done!"<<endl;
        isDone = true;
        return;
    }
    
    //select one at random
    int thisChoice = (int) ofRandom(choices.size());
    
    //select a possibility at random
    int thisTile = -1;
    if (!useFreq){
        thisTile = (int) ofRandom(choices[thisChoice].potentialIDs.size());
    }
    //get the frequency of each possibility for each direction
    else{
        vector<NeighborInfo> tileChoices = getTileChoicesWithFreq( choices[thisChoice].x, choices[thisChoice].y );
        float totalFreq = 0;
        for (int i=0; i<tileChoices.size(); i++){
            totalFreq += tileChoices[i].freq;
        }
        float roll = ofRandom(totalFreq);
        
        for (int i=0; i<tileChoices.size(); i++){
            roll -= tileChoices[i].freq;
            if (roll <= 0){
                thisTile = i;
                break;
            }
        }
    }
    
    //make a move
    curMove->move( choices[thisChoice].x, choices[thisChoice].y, choices[thisChoice].potentialIDs[thisTile]);
    //cout<<"setting "<<curMove->thisMove.col<<","<<curMove->thisMove.row<<" to "<<curMove->thisMove.idNum<<endl;
    
    updateBoardFromMove(curMove);
}

//--------------------------------------------------------------
vector<NeighborInfo> ofxWaveFunctionCollapse::getTileChoicesWithFreq(int col, int row){
    vector<NeighborInfo> tileChoices;
    for (int i=0; i<outputImage[col][row].potentialIDs.size(); i++){
        NeighborInfo info;
        info.idNum = outputImage[col][row].potentialIDs[i];
        info.freq = 0;
        tileChoices.push_back(info);
    }
    
    //check the tile to our north
    if (row > 0){
        if (outputImage[col][row-1].state == STATE_SET){
            int thisID = outputImage[col][row-1].setID;
            sourceTiles[thisID].addNeighborFreq(2, tileChoices);
        }
    }
    
    //check the tile to our east
    if (col < outputCols-1){
        if (outputImage[col+1][row].state == STATE_SET){
            int thisID = outputImage[col+1][row].setID;
            sourceTiles[thisID].addNeighborFreq(3, tileChoices);
        }
    }
    
    //check the tile to our south
    if (row < outputRows-1){
        if (outputImage[col][row+1].state == STATE_SET){
            int thisID = outputImage[col][row+1].setID;
            sourceTiles[thisID].addNeighborFreq(0, tileChoices);
        }
    }
    
    //check the tile to our west
    if (col > 0){
        if (outputImage[col-1][row].state == STATE_SET){
            int thisID = outputImage[col-1][row].setID;
            sourceTiles[thisID].addNeighborFreq(1, tileChoices);
        }
    }
    
    //multiply them by the weight and then give them all at least 1 frequency
    for (int i=0; i<tileChoices.size(); i++){
        tileChoices[i].freq *= freqWeight;
        tileChoices[i].freq += 1;
    }
    
    //testing
    //    cout<<"choices "<<tileChoices.size()<<endl;
    //    for (int i=0; i<tileChoices.size(); i++){
    //        cout<<" tile "<<tileChoices[i].idNum<<" "<<tileChoices[i].freq<<endl;
    //    }
    
    return tileChoices;
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::updateBoardFromMove(CheckPoint * point){
    if (isDone){
        return;
    }
    //cout<<"update board"<<endl;
    //cout<<"depth "<<point->getDepth()<<endl;
    MoveInfo move = point->thisMove;
    if (move.col == -1){
        cout<<"empty move"<<endl;
        return;
    }
    
    //set the given tiles
    outputImage[move.col][move.row].set(move.idNum);
    
    //rule out anything we need to because it previously lead to dead ends
    for (int i=0; i<point->badMoves.size(); i++){
        MoveInfo badMove = point->badMoves[i];
        outputImage[badMove.col][badMove.row].ruleOutID(badMove.idNum);
        cout<<"hey brah don't do "<<badMove.col<<","<<badMove.row<<": "<<badMove.idNum<<endl;
    }
    
    //go through the neighbors and update them
    
    //north
    if (move.row > 0){
        outputImage[move.col][move.row-1].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 0);
    }
    
    //east
    if (move.col < outputCols-1){
        outputImage[move.col+1][move.row].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 1);
    }
    
    //south
    if (move.row < outputRows-1){
        outputImage[move.col][move.row+1].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 2);
    }
    
    //west
    if (move.col > 0){
        outputImage[move.col-1][move.row].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 3);
    }
    
    //validate
    validateBoard();
}

//--------------------------------------------------------------
//if any potential tiles have no options, this move is dirt!
void ofxWaveFunctionCollapse::validateBoard(){
    bool boardIsValid = true;
    for (int x=0; x<outputCols; x++){
        for (int y=0; y<outputRows; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() == 0){
                cout<<"invalid board on "<<curMove->getDepth()<<endl;
                cout<<x<<","<<y<<" is no good!"<<endl;
                boardIsValid = false;
            }
        }
    }
    
    if (!boardIsValid){
        //autoPlay = false;
        curMove->prevPoint->ruleOutMove(curMove->thisMove);
        revertToCheckPoint(curMove->prevPoint);
    }
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::revertToCheckPoint(CheckPoint * point){
    cout<<"REVERT to "<<point->getDepth()<<endl;
    resetOutput();
    
    if (point->getDepth() == 0){
        needFirstMove = true;
    }
    
    curMove = rootMove;
    int steps =0;
    while(curMove != point){
        steps++;
        //cout<<"step: "<<steps<<endl;
        updateBoardFromMove(curMove);
        curMove = curMove->nextPoint;
    }
    
    //cout<<"we found the move"<<endl;
    
    updateBoardFromMove(curMove);
    
    curMove->prune();
    
}


//--------------------------------------------------------------
vector<vector<int>> ofxWaveFunctionCollapse::get_int_map(){
    vector< vector<int> > map;
    map.resize(outputCols);
    for (int i=0; i< outputCols; i++){
        map[i].resize(outputRows);
    }
    
    for (int x=0; x<outputCols; x++){
        for (int y=0; y<outputRows; y++){
            map[x][y] = outputImage[x][y].setID;
        }
    }
    
    return map;
}

//--------------------------------------------------------------
void ofxWaveFunctionCollapse::printOutput(){
    cout<<"map: "<<endl;
    for (int y=0; y<outputRows; y++){
        for (int x=0; x<outputCols; x++){
            //cout<<x<<","<<y<<endl;
            cout<<outputImage[x][y].print();
        }
        cout<<endl;
    }
}
