#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class Actor: public GraphObject { //base
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth)
    : GraphObject(imageID, startX, startY, startDirection, size, depth), m_world(world) {is_alive = true; setVisible(true);}
            
    virtual void doSomething() = 0;
    
    ~Actor(){setVisible(false);}
        
    StudentWorld* getWorld() const {return m_world;};
    
    void die();
    
    bool isAlive();
    
    virtual void moveTo(int x, int y);
    
    
private:
    StudentWorld* m_world;
    bool is_alive;
};


class Earth: public Actor {
public:
    Earth(StudentWorld* world, int startX, int startY, int imageID = TID_EARTH, Direction d = right, float size = .25, unsigned int depth = 3): Actor(world, imageID, startX, startY, d, size, depth) {return;}; //always visible
    
    virtual void doSomething() {return;};
        
};

class Human: public Actor {
public:
    Human(StudentWorld* world, int imageID, int startX, int startY, Direction d, int hp) : Actor(world, imageID, startX, startY, d, 1.0, 0), hit_pts(hp) {return;};
    
    int getHP() {return hit_pts;};
    
    void loseHP(int points) {hit_pts -= points;};
    
    virtual void annoy(int points) = 0;
    
private:
    int hit_pts;
};

class TunnelMan: public Human {
public:
    TunnelMan(StudentWorld* world): Human(world, TID_PLAYER, 30, 60, right, 10), water(5), sonar(1), nuggets(0) {setVisible(true);}; //always visible
    
    void move(Direction d);
        
    virtual void doSomething();
        
    int getWater() {return water;};
    
    int getSonar() {return sonar;};
    
    int getNuggets() {return nuggets;};
    
    virtual void annoy(int points);
        
    void add(int ID);
    
    void shoot();
    
        
private:
    int hit_pts, water, sonar, nuggets;

};

class Boulder: public Actor {
public:
    Boulder(StudentWorld* world, int startX, int startY): Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1),isStable(true),m_ticks(0) {world->removeEarth(startX, startY);};
    
    virtual void doSomething();
    void annoyHuman();
private:
    bool isStable, isFalling;
    int m_ticks;
    
};


class Squirt : public Actor {
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction d): Actor(world, TID_WATER_SPURT, startX, startY, d, 1.0, 1), distanceTravel(0) {};
    
    virtual void doSomething();
    
    bool annoyProtesters(); //TODO: once protestors are implemented
private:
    int distanceTravel;
};


class Item :public Actor {
public:
    Item(StudentWorld* world, int imageID, int startX, int startY): Actor(world, imageID, startX, startY, right, 1.0, 2) {};
    
    virtual void doSomething() = 0;
    
    void disappear(int ticks);
    
private:
    int m_ticks;
};


class Barrel : public Item {
public:
    Barrel(StudentWorld* world, int startX, int startY): Item(world, TID_BARREL, startX, startY) {setVisible(false);};
    virtual void doSomething();
};


class Gold :public Item {
public:
    Gold(StudentWorld* world, int startX, int startY, bool visible,bool dropped): Item(world, TID_GOLD, startX, startY), isDropped(dropped) {setVisible(false);};
    virtual void doSomething();
private:
    bool isDropped;
};


class PowerUp : public Item {
public:
    PowerUp(StudentWorld* world, int imageID, int startX, int startY): Item(world, imageID, startX, startY) {};
    virtual void doSomething();
};


class Sonar :public PowerUp {
public:
    Sonar(StudentWorld* world, int startX, int startY): PowerUp(world, TID_SONAR, startX, startY) {};
};


class Water: public PowerUp {
public:
    Water(StudentWorld* world, int startX, int startY): PowerUp(world, TID_WATER_POOL, startX, startY) {};
};


//TODO: adjust later

class Protester : public Human{
public:
    Protester(StudentWorld* world, int imageID,int hp);
    virtual void doSomething();
    virtual void Bribed();
    void moveInDirection(Direction direction);
    void randomizeMove();
    virtual void annoy(int hp);
    void getStunned();
    bool facingPlayer();
    Direction directionToPlayer();
    bool straightPathToPlayer(Direction direction);
    Direction randomDirection();
    bool isAtIntersection();
    void pickDirectionToTurn();

private:
    bool leave;
    int randomMove;
    int tickSinceLastTurn;
    int tickNoYell;
    int ticksToWaitBetweenMoves;
};
class RegularProtester : public Protester {
public:
    RegularProtester(StudentWorld* world) : Protester(world, TID_PROTESTER, 5) {};
};

class HardcoreProtester : public Protester {
public:
    HardcoreProtester(StudentWorld* world) : Protester(world, TID_HARD_CORE_PROTESTER, 20) {};
    
};

#endif // ACTOR_H_
