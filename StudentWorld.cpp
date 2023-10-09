#include "StudentWorld.h"
#include <string>
#include <sstream>
#include <queue>
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir): GameWorld(assetDir) {
    m_tunnelman = nullptr;
    actors.clear();
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    m_barrels = ticksSince = protestersAlive = 0;
    firstTick = true;
    
    
    m_tunnelman = new TunnelMan(this);
    for (int i = 0; i <= 63; i++) { //might be wrong value
        for (int j = 0; j < 60; j++) {
            if (i <= 33 && i >= 30 && j > 3) {continue;};
            e[i][j] = new Earth(this, i, j);
        }
    }
    int B = min( (int)getLevel()/2 + 2, 9);
    int G = max((int)getLevel()/2, 2);
    int L = min( 2 + (int)getLevel(), 21);
    addBoGoBa(B, 'B');
    addBoGoBa(G, 'G');
    addBoGoBa(L, 'L');

    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::isValidMove(int x, int y) {
    for (auto a : actors) {        
        if (x + SPRITE_WIDTH - 1 > a->getX() && x < a->getX() + SPRITE_WIDTH - 1) {
            if (y + SPRITE_HEIGHT - 1 > a->getY() && y < a->getY() + SPRITE_HEIGHT - 1) {
                return false;
            }
        }
    }
        return true;
}

//page 18
int StudentWorld::move() {
    displayNums();
    
    for (Actor* a : actors) {
        if (a->isAlive()) {
            a->doSomething();
        }
        if (!m_tunnelman->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_barrels == 0) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    m_tunnelman->doSomething();
    
    addItems();
    addProtesters();
    
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end();) {
        if (!(*it)->isAlive()) {
            delete *it;
            it = actors.erase(it);
        }
        else it++;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 60; j++)
        {
            delete e[i][j];
        }
    }
    for (Actor* a : actors) {delete a;}
    
        actors.clear();
        delete m_tunnelman;
        m_tunnelman = nullptr;
}

bool StudentWorld::removeEarth(int x, int y) {
    bool clear = false;
        for (int i = x; i < x+4; i++)
            for (int j = y; j < y+4; j++)
            {
                if (e[i][j] != nullptr) {
                    delete e[i][j];
                    e[i][j] = nullptr;
                    clear = true;
                }
            }
        return clear;
}

bool StudentWorld::withinR(int x1, int y1, int x2, int y2, int r) {
    if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) <= r * r) {return true;}
    return false;
}

bool StudentWorld::actorsWithinR(int x, int y, int r) {
    vector<Actor*>::iterator it;
        it = actors.begin();
        while (it != actors.end()){
            if (withinR(x,y, (*it)->getX(),(*it)->getY(), r))
                return true;
            it++;
        }
        return false;
}

void StudentWorld::addBoGoBa(int n, char actor) {
    //boulder: B, Gold: G, Barrel: L
    int x, y;
    for (int i = 0; i < n; i++) {
        do {
            x = rand() % 60 + 1;
            
            if (actor == 'B') {y = rand() % 36 + 1 + 20;}
            
            else    {y = rand() % 56 + 1;}
            
        } while (actorsWithinR(x, y, 6) || (x > 26 && x < 34 && y>0));
        
        switch (actor) {
            case 'B':
                actors.push_back(new Boulder(this, x, y));
                break;
            case 'G':
                actors.push_back(new Gold(this, x, y, false, false));
                break;
            case 'L':
                actors.push_back(new Barrel(this, x, y));
                m_barrels++;
                break;
        }
    }
}

bool StudentWorld::isAboveEarth(int x, int y) {
    for (int i = x; i < x + 4; i++)
        {
            if (e[i][y] != nullptr)
                return true;
        }
        return false;
}

bool StudentWorld::isThereABoulder(int x, int y, int r) {
    vector<Actor*>::iterator it;
        for (it = actors.begin(); it != actors.end(); it++) {
            if ((*it)->getID() == TID_BOULDER && withinR(x, y, (*it)->getX(), (*it)->getY(), r))
                return true;
        }
        return false;
}

bool StudentWorld::isThereEarth(int x, int y) {
    for (int i = x; i < x + 4; i++) {
        
        for (int j = y; j <y + 4; j++) {
            
            if (e[i][j] != nullptr)
                return true;
        }
    }
    
    return false;
}

bool StudentWorld::playerWithinR(Actor* actor, int r) {
    return withinR(actor->getX(), actor->getY(), m_tunnelman->getX(), m_tunnelman->getY(), r);
}

void StudentWorld::minusBarrel() {
    m_barrels--;
}

Protester* StudentWorld::protesterWithinR(Actor *a, int r) {
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++) {
        if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
            if (withinR(a->getX(), a->getY(), (*it)->getX(), (*it)->getY(), r))
                return dynamic_cast<Protester*> (*it);
    }
    return nullptr;
}

string StudentWorld::displayNums() {
    stringstream s;

    s.fill('0');
    s << "Scr: ";
    s << setw(6) << getScore();

    s.fill(' ');
    s << " Lvl: ";
    s << setw(2) << getLevel();

    s << " Lives: ";
    s << setw(1) << getLives();

    s << "  Hlth: ";
    s << setw(3) << m_tunnelman->getHP() * 10;
    s << '%';

    s << "  Wtr: ";
    s << setw(2) << m_tunnelman->getWater();

    s << "  Gld: ";
    s << setw(2) << m_tunnelman->getNuggets();

    s << "  Sonar: ";
    s << setw(2) << m_tunnelman->getSonar();

    s << "  Oil Left: ";
    s << setw(2) << m_barrels;
    
    setGameStatText(s.str());

    return s.str();
    
}

void StudentWorld::moveTowardsExit(Protester *p) {
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            maze[i][j]=0;
        }
    }
    
    //maze solver!!!
    int a = p->getX();
    int b = p->getY();
        queue<Grid> q;
    q.push(Grid(60,60));
    maze[60][60]=1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;

        //left
        if(canMoveInDirection(x,y, GraphObject::left)&& maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            maze[x-1][y] =maze[x][y]+1;
        }
        //right
        if(canMoveInDirection(x,y, GraphObject::right)&& maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            maze[x+1][y] = maze[x][y]+1;
        }
        //up
        if(canMoveInDirection(x,y, GraphObject::up)&& maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            maze[x][y+1] = maze[x][y]+1;
        }
        // down
        if(canMoveInDirection(x,y, GraphObject::down)&& maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            maze[x][y-1] = maze[x][y]+1;
        }
    }
    if(canMoveInDirection(a,b, GraphObject::left)&& maze[a-1][b]< maze[a][b])
        p->moveInDirection(GraphObject::left);
    if(canMoveInDirection(a,b, GraphObject::right)&& maze[a+1][b]< maze[a][b])
        p->moveInDirection(GraphObject::right);
    if(canMoveInDirection(a,b, GraphObject::up)&& maze[a][b+1]< maze[a][b])
        p->moveInDirection(GraphObject::up);
    if(canMoveInDirection(a,b, GraphObject::down)&& maze[a][b-1]< maze[a][b])
        p->moveInDirection(GraphObject::down);
    return ;
}

bool StudentWorld::canMoveInDirection(int x, int y, GraphObject::Direction d) {
    switch (d) {
            
        case GraphObject::left:
            return (x !=0 &&!isThereEarth(x - 1, y) && !isThereABoulder(x, y));
        case GraphObject::right:
            return (x !=60 && !isThereEarth(x + 1, y) && !isThereABoulder(x + 1, y));
        case GraphObject::up:
            return (y !=60 &&!isThereEarth(x, y + 1) && !isThereABoulder(x, y + 1));
        case GraphObject::down:
            return (y !=0 && !isThereEarth(x, y - 1) && !isThereABoulder(x, y - 1));
        default:
            return false;
        }
}

GraphObject::Direction StudentWorld::senseTM(Protester * p, int f){
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            maze[i][j]=0;
        }
    }
    int a = p->getX();
    int b = p->getY();
    queue<Grid> q;
    q.push(Grid(getTM()->getX(),getTM()->getY()));
    maze[getTM()->getX()][getTM()->getY()]=1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;
        
        //left
        if(canMoveInDirection(x,y, GraphObject::left)&& maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            maze[x-1][y] = maze[x][y]+1;
        }
        //right
        if(canMoveInDirection(x,y, GraphObject::right)&& maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            maze[x+1][y] =maze[x][y]+1;
        }
        //up
        if(canMoveInDirection(x,y, GraphObject::up)&& maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            maze[x][y+1] = maze[x][y]+1;
        }
        // down
        if(canMoveInDirection(x,y, GraphObject::down)&& maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            maze[x][y-1] = maze[x][y]+1;
        }
    }
    if( maze[a][b] <= f+1){
        if(canMoveInDirection(a,b, GraphObject::left)&& maze[a-1][b]<maze[a][b])
            return GraphObject::left;
        if(canMoveInDirection(a,b, GraphObject::right)&& maze[a+1][b]< maze[a][b])
            return GraphObject::right;
        if(canMoveInDirection(a,b, GraphObject::up)&& maze[a][b+1]< maze[a][b])
            return GraphObject::up;
        if(canMoveInDirection(a,b, GraphObject::down)&& maze[a][b-1]< maze[a][b])
            return GraphObject::down;
    }
    return GraphObject::none;
}

void StudentWorld::addItems()
{
    int x, y;
    int G = (int)getLevel() + 300;
    if (int(rand() % G) + 1 == 1) {
        if (int(rand() % 5) + 1 == 1) {
            actors.push_back(new Sonar(this, 0, 60));
        }
        else {
            do {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isThereEarth(x, y));
            actors.push_back(new Water(this, x, y));
        }
    }
}
void StudentWorld::addProtesters()
{
    int T = max(25, 200 - (int)getLevel());
    int P = fmin(15, 2 + getLevel() * 1.5);    int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
    if (firstTick || (ticksSince > T && protestersAlive < P)) {
        if (rand() % 100 + 1 < probabilityOfHardcore)
            actors.push_back(new HardcoreProtester(this));
        else actors.push_back(new RegularProtester(this));
        ticksSince = 0;
        protestersAlive++;
        firstTick = false;
    }
    ticksSince++;
}

void StudentWorld::detectNearActors(int x, int y, int r)
{
    int a, b;
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++)
    {
        if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
        {
            a = (*it)->getX(); b = (*it)->getY();
            if ((x - a)*(x - a) + (y - b)*(y - b) <= r * r)
                (*it)->setVisible(true);
        }
    }
}
