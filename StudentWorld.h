#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class TunnelMan;
class Earth;
class Actor;
class Protester;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();


    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool removeEarth(int x, int y);
    
    bool isAboveEarth(int x, int y);
    
    bool isThereEarth(int x, int y);
    
    bool isThereABoulder(int x, int y, int r = 3);

    bool isValidMove(int x, int y);
    
    bool withinR(int x1, int y1, int x2, int y2, int r);
    
    bool actorsWithinR(int x, int y, int r);
    
    bool playerWithinR(Actor* actor, int r);
    
    void minusBarrel();
    
    void addBoGoBa(int n, char actor);
    
    string displayNums();
    
    Protester* protesterWithinR(Actor* a,  int r);
    
    TunnelMan* getTM() {return m_tunnelman;};
        
    bool canMoveInDirection(int x, int y, GraphObject::Direction d);
    
    void moveTowardsExit(Protester * p);
    
    GraphObject::Direction senseTM(Protester* p, int f);
        
    void addItems();
    
    void addProtesters();
    
    void minusProtester() {protestersAlive--;};
    
    vector<Actor*> getActors() {return actors;};
    
    void detectNearActors(int x, int y, int r);

private:
    TunnelMan* m_tunnelman;
    vector<Actor*> actors;
    Earth* e[70][70];
    int maze[70][70];
    int m_barrels, ticksSince, protestersAlive;
    bool firstTick;
    
    struct Grid {
        int x;
        int y;
        Grid(int a, int b): x(a),y(b){}
    };
};

#endif // STUDENTWORLD_H_
