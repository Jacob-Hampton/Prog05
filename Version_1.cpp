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
    //bool endPt = false;
    //  number of times reached if it is an end point
    //int numOfTimesReached = 0;
} Node;

typedef struct EndPoint{
    Node node;
    //  tells weather is an endpoint
    //bool endPt = false;
    //  number of times reached if it is an end point
    int numOfTimesReached = 0;
} EndPoint;
 
typedef struct Map
{
    //  num of rows
    int height;
    //  num of columns
    int width;
    //  2d vector of the map
    vector<vector<float>> grid;
    vector<EndPoint> endPoints;
} Map;
 



typedef struct Skier
{
    //  rout that the skier has taken
    vector<Node> route;
    //  row that the skier is currently on
    int r;
    //  column that the skier is currently on
    int c;
    //  width of the map
    int width;
    //  height of the map
    int height;
} Skier;
 
 
//----------------------------
//  Function prototypes
//----------------------------
bool goodNode(int,int,int);
bool findRoute(Skier skier, vector<vector<float>>);
void createEndPoint(int row, int col, vector<EndPoint> endPnts);
//----------------------------
//  Global variables
//----------------------------
 
//  This is the value that traceFlag will be if tracing is nessesary
#define TRACE 1
//  This is the value that traceFlag will be if tracing is unnecessary
#define TRACE_OFF 0
 
 
 
//  Our readers abd writers will do a bit of sleep time to simulate actual work
const int READER_READ_TIME  = 10000;
const int READER_PROCESSING_TIME = 200000;
const int WRITER_WRITE_TIME = 10000;
const int WRITER_PROCESSING_TIME = 200000;
 
//  Random generators:  For a uniform distribution
random_device randDev;
default_random_engine engine(randDev());
//uniform_int_distribution<unsigned int> randomArrayIndex(0, ARRAY_SIZE-1);
uniform_int_distribution<unsigned int> headsOrTails(0, 1);
uniform_int_distribution<unsigned int> threadType(0, 9);
uniform_int_distribution<int> randomVal(10, 100);
 
 
int main(int argc, const char* argv[])
{
    if(argc != 6){
        cout << "usage: ./Version_1 <path/filename.map path/outputfolder 1 1 1>" << endl;
        exit(1);
    }

    string mapFilePath = argv[1];
    string outputFolder = argv[2];
    int numOfSkiers = atoi(argv[3]);
    int numOfRuns = atoi(argv[4]);
    int traceFlag = atoi(argv[5]);
    Map map;
    ifstream mapFile;
    mapFile.open(mapFilePath);
    mapFile >> map.height >> map.width;
    
    for(int r=0; r< map.height ; r++){
        vector<float> temp;
        for(int c = 0; c< map.width; c++){
            float num;
            mapFile >> num;
            temp.push_back(num);
        }
        map.grid.push_back(temp);
    }
    

    for(int i = 0; i< 1; i++){
        bool ski = true;
        Skier elton;
        elton.r = 0;
        elton.c = 0;
        while (ski)
        {
            ski = findRoute(elton,map.grid);
            cout << elton.r << endl;
        }
        createEndPoint(elton.r,elton.c,map.endPoints);
        //cout << elton.r << endl;
    }

   // cout << map.endPoints.at(0).node.r<<endl;

    return 0;
}


bool findRoute(Skier skier, vector<vector<float>> grid){
    float lowestValue = grid[skier.r][skier.c];
    bool stop = false;
    bool newNode = false;
    // best row
    int bestRow = skier.r;
    //  best column
    int bestCol = skier.c;
    for (int r = 0; r < 3; r++)
    {
        //  potential row
        int pRow = skier.r + (r-1);
        if(!goodNode(pRow,skier.r, skier.height)){
            continue;
        }
        for(int c = 0; c<3; c++){
            //  potential column
            int pCol = skier.c + (c-1);
            if(!goodNode(pCol,skier.c, skier.width)){
                continue;
            }
            if(grid.at(r).at(c) <= lowestValue){
                lowestValue = grid.at(r).at(c);
                bestCol = c;
                bestRow = r;
                newNode = true;
            }
        }
    }
    if (newNode){
        Node node;
        node.c = bestCol;
        node.r = bestRow;
    }
    skier.r = bestRow;
    skier.c = bestCol;
    return stop;
}


bool goodNode(int potentialRorC, int skierRorC,int mapHorW){
    if(potentialRorC == skierRorC){
        return false;
    }else if((potentialRorC >= mapHorW) || (potentialRorC < 0)){
        return false;
    }else{
        return true;
    }
}


void createEndPoint(int row, int col, vector<EndPoint> endPnts){
    bool isNew =true;
    for(int i= 0;i< endPnts.size();i++){
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
 
 
 

