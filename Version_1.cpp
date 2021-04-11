#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <cstring>      // for strerror()
#include <vector>
#include <thread>
#include <fstream>
#include <iterator>
#include <unistd.h>     // for usleep()
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;
 
//----------------------------
//  Custom data types
//----------------------------
typedef struct Node
{
    //  row which the Node exists
    int r;
    //  column that node exists
    int c;
    //  tells weather is an endpoint
    bool isEndPt = false;
} Node;

typedef struct EndPoint{
    Node node;
    //  number of times reached if it is an end point
    int numOfTimesReached = 0;
} EndPoint;

typedef struct Skier
{
    vector<vector<Node>> routes;

} Skier;

typedef struct Map
{
    //  num of rows
    int height;
    //  num of columns
    int width;
    //  2d vector of the map
    vector<vector<float>> grid;
    // Each endpoint
    vector<EndPoint> endPoints;
    //  Number of runs each skier should make
    int numOfRuns;
    // all the skiers
    vector<Skier> skiers;
} Map;
 

//----------------------------
//  Function prototypes
//----------------------------
bool goodNode(int pRow,int pCol, int height, int width, Node node);
Node findRoute(Node node,vector<vector<float>> grid,int height,int width);
void createEndPoint(int row, int col, vector<EndPoint> endPnts);
void execSkier(Map* map);
vector<Node> execRun(Map* map);
void writeToFile(string filePath,Map map);
//----------------------------
//  Global variables
//----------------------------
 
//  This is the value that traceFlag will be if tracing is nessesary
#define TRACE 1
//  This is the value that traceFlag will be if tracing is unnecessary
#define TRACE_OFF 0
 
 

 
 
int main(int argc, const char* argv[])
{
    if(argc != 6){
        cout << "usage: ./Version_1 <path/filename.map path/outputfolder 1 1 1>" << endl;
        exit(1);
    }
    string mapFilePath = argv[1];
    string outputFolder = argv[2];
    int numOfSkiers = atoi(argv[3]);
    const int numOfRuns = atoi(argv[4]);
    int traceFlag = atoi(argv[5]);
    Map map;
    ifstream mapFile;
    mapFile.open(mapFilePath);
    mapFile >> map.height >> map.width;
    map.numOfRuns = numOfRuns;
    for(int r=0; r< map.height ; r++){
        vector<float> temp;
        for(int c = 0; c< map.width; c++){
            float num;
            mapFile >> num;
            temp.push_back(num);
        }
        map.grid.push_back(temp);
    }

    vector<pid_t> pIds;
    for (int i=0; i< numOfSkiers; i++)
    {
        cout<< "New Skier +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        pid_t p = fork();
        if(p==0){
            execSkier(&map);
        }
    }

    int statusVal;
    pid_t termProcess;
    for (int i=0; i<numOfSkiers; i++)
    {
        termProcess = waitpid(-1, &statusVal, 0);
    }
    string basePath = mapFilePath.substr(mapFilePath.find_last_of("/\\")+1);
    string::size_type const p(basePath.find_last_of('.'));
    string fileName = basePath.substr(0,p);
    string outFileName = outputFolder.append(fileName.append(".out"));
    writeToFile(outFileName,map);
    return 0;
}

void writeToFile(string filePath,Map map){
    ofstream outFile;
    outFile.open(filePath);
    outFile << "Number of Runs: " << map.numOfRuns << "\tNumber of End Points: " << map.endPoints.size()<< endl;
    for(unsigned int i =0;i< map.endPoints.size(); i++){
        EndPoint endpt = map.endPoints.at(i);
        outFile << "End point at row: " << endpt.node.r << " and column: " << endpt.node.c << " was reached " << endpt.numOfTimesReached << " times." << endl;
    }
    outFile.close();
}


Node findRoute(Node node,vector<vector<float>> grid,int height,int width){
    float lowestValue = grid.at(node.r).at(node.c);
    bool newNode = false;
    int bestRow = node.r;
    int bestCol = node.c;
    for (int r = 0; r < 3; r++)
    {
        //  potential row
        int pRow = node.r + (r-1);
        for(int c = 0; c<3; c++){
            //  potential column
            int pCol = node.c + (c-1);
            if(!goodNode(pRow,pCol,height,width,node)){
                continue;
            }
 
            if(grid.at(pRow).at(pCol) <= lowestValue){
                lowestValue = grid.at(pRow).at(pCol);
                bestCol = pCol;
                bestRow = pRow;
                newNode = true;
            }
        }
    }
    if (newNode){
        Node newNode;
        newNode.c = bestCol;
        newNode.r = bestRow;
        return newNode;
    }else{
        node.isEndPt =true;
        return node;
    }
}


bool goodNode(int pRow,int pCol, int height, int width, Node node){
    if((pRow == node.r) && (pCol && node.c)){
        return false;
    }else if((pRow >= height) || (pCol >= width)){
        return false;
    }else if((pRow < 0) || (pCol < 0)){
        return false;
    }else{
        return true;
    }
}


void createEndPoint(int row, int col, vector<EndPoint> endPnts){
    bool isNew =true;
    for(unsigned int i= 0;i< endPnts.size();i++){
        if((endPnts.at(i).node.c == col) || (endPnts.at(i).node.r == row)){
            endPnts.at(i).numOfTimesReached++;
            isNew = false;
        }
    }
    if(isNew){
        EndPoint newEndpt;
        newEndpt.node.c = col;
        newEndpt.node.r = row;
        newEndpt.numOfTimesReached++;
        endPnts.push_back(newEndpt);
    }
}

void execSkier(Map* map){
    srand(time(NULL)+getpid());
    Skier skier;
    for(int i =0; i< map->numOfRuns;i++){
        cout << "new run**********************************************"<<endl;
        skier.routes.push_back(execRun(map));
    }
    map->skiers.push_back(skier);
    exit(0);
}

vector<Node> execRun(Map* map){
    vector<Node> route;
    Node start;
    start.r = rand()% (map->height -1);
    start.c = 1 + rand()% (map->width -1);
    route.push_back(start);
    bool ski =true;
    while(ski){
        Node node = findRoute(route.at(route.size()-1),map->grid,map->height,map->width);
        ski = !node.isEndPt;
        if(!node.isEndPt){
            route.push_back(node);
        }else{
            createEndPoint(node.r,node.c,map->endPoints);
        }
        cout << "AFter Find Route =============================" << endl;
        cout << "r: " << node.r << ", c: " << node.c << endl;
        cout << "val: " << map->grid.at(node.r).at(node.c)<< endl;
    }
    return route;
}






































   /*
   //  I allocate my data data (all zero values)
   Pair* data = new Pair[ARRAY_SIZE];
//   vector<ThreadData> tdatas;
   for (unsigned int i=0; i<ARRAY_SIZE; i++)
   {
       data[i].isInitialized = false;
 
   }
   int wCounter = 0;
   int rCounter = 0;
   //  I create my readers and my writers
   for (int k=0; k<NUM_THREADS; k++)
   {
       ThreadData tdata;
       tdata.p = data;
       if (threadType(engine) < WRITER_PROP)
       {
           // create a writer
           tdata.isReader =false;
           tdata.index = wCounter;
           wCounter++;
       }
       else
       {
           //  create a reader
           tdata.isReader = true;
           tdata.index = rCounter;
           rCounter++;
       }
//      tdatas.push_back(tdata);
   }
   
   vector <thread> threads;
   for (int k=0; k<NUM_THREADS; k++)
   {
 
       if(tdatas[k].isReader){
//          threads.push_back(thread(readerThreadFunc,&tdatas[k]));
       }else{
//          threads.push_back(thread(writerThreadFunc,&tdatas[k]));
       }
   }
   //  right now my main thread has nothing to do
  
   while (true);

   return 0;
}
 
 
//  This is my reader thread function.
void readerThreadFunc(ThreadData *tData)
{
   bool keepGoing = true;
   cout << "Reader:" << tData->index << endl; 
   while (keepGoing)
   {
       //  pick a range of indices to work on
       unsigned int index1 = randomArrayIndex(engine),
                    index2 = randomArrayIndex(engine);
       //  I want index1 ≤ index2
       if (index1 > index2)
       {
           unsigned int temp = index1;
           index1 = index2;
           index2 = temp;
       }
      
       //  Read all elements between index1 and index2
       int sumX = 0, sumY = 0;
       for (unsigned int i=index1; i<=index2; i++)
       {
//          int x = data[i].x;
           usleep(READER_READ_TIME);
//          int y = data[i].y;
           //
//          sumX += x;
//          sumY += y; 
       }  
      
       //  do some more work
       usleep(READER_PROCESSING_TIME);    
   }  
   return;
}
 
//  This is my writer thread function.
void writerThreadFunc(ThreadData *tdata)
{
   bool keepGoing = true;
   cout << "Writer:" << tData->index << endl;
   while (keepGoing)
   {
      
       //  compute some values [10, 100] to store there
       usleep(WRITER_PROCESSING_TIME);    
       int x = randomVal(engine);
       int y = randomVal(engine);
      
       //  then write them in the data at chosen location
       //  pick a random index to work on
       int index = randomArrayIndex(engine);
//      data[index].x = x;
       usleep(WRITER_WRITE_TIME);
//      data[index].y = y;
       usleep(WRITER_WRITE_TIME);
   }  
  
   return;
}
 */
 
 
 

