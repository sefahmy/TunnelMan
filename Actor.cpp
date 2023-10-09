#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//************************************ ACTOR ***********************************

void Actor::moveTo(int x, int y) {
    if (x < 0) {x = 0;}
    if (x > 60) {x = 60;}
    if (y < 0) {y = 0;}
    if (y > 60)  {y = 60;}
    
    GraphObject::moveTo(x, y);
}

void Actor::die() {is_alive = false;}

bool Actor::isAlive() {return is_alive;}


//************************************ TUNNELMAN ***********************************


void TunnelMan::move(Direction d) {
    switch(d) { 
        case left:
            setDirection(left);
            if (!getWorld()->isThereABoulder(getX() - 1, getY())) {
                moveTo(getX() - 1, getY());
            }
                getWorld()->removeEarth(getX(), getY());
                getWorld()->playSound(SOUND_DIG);
            break;
        case right:
            setDirection(right);
            if (!getWorld()->isThereABoulder(getX() + 1, getY())) {
                moveTo(getX() + 1, getY());
            }
            getWorld()->removeEarth(getX(), getY());
            getWorld()->playSound(SOUND_DIG);
            break;
        case up:
            setDirection(up);
            if (!getWorld()->isThereABoulder(getX(), getY() + 1)) {
                moveTo(getX(), getY() + 1);
            }
            getWorld()->removeEarth(getX(), getY());
            getWorld()->playSound(SOUND_DIG);
            break;
        case down:
            setDirection(down);
            if (!getWorld()->isThereABoulder(getX(), getY() - 1)) {
                moveTo(getX(), getY() - 1);
            }
            getWorld()->removeEarth(getX(), getY());
            getWorld()->playSound(SOUND_DIG);
            break;
        default:
            return;
    }
}

void TunnelMan::annoy(int points) {
    loseHP(points);
    if (getHP() <= 0) {
        die();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void TunnelMan::doSomething() {
    if(!isAlive()) {return;}
    
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
                
                //MOVEMENT
            case KEY_PRESS_LEFT:
                move(left);
                break;
            case KEY_PRESS_RIGHT:
                move(right);
                break;
            case KEY_PRESS_UP:
                move(up);
                break;
            case KEY_PRESS_DOWN:
                move(down);
                break;
                
                //POWERUPS
            case KEY_PRESS_SPACE:
                if(water > 0) {
                    water--;
                    shoot();
                }
            case 'z':
            case 'Z':            //sonar
                if (sonar > 0)
                {
                    sonar--;
                    getWorld()->detectNearActors(getX(), getY(), 12);
                    getWorld()->playSound(SOUND_SONAR);
                }
                break;
            case KEY_PRESS_TAB:        //bribe with gold
                if (nuggets > 0) {
                    getWorld()->getActors().push_back(new Gold(getWorld(), getX(), getY(), true, true));
                    nuggets--;
                }
                break;
            default:
                break;
        }
    }
    return;
}

void TunnelMan::shoot() {
    switch (getDirection())
    {
    case left:
        if(!getWorld()->isThereEarth(getX()-4,getY())&& !getWorld()->isThereABoulder(getX()-4, getY()))
            getWorld()->getActors().push_back(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
        break;
    case right:
        if (!getWorld()->isThereEarth(getX() + 4, getY()) && !getWorld()->isThereABoulder(getX()+4, getY()))
            getWorld()->getActors().push_back(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
        break;
    case down:
        if (!getWorld()->isThereEarth(getX() , getY()-4) && !getWorld()->isThereABoulder(getX(), getY()-4))
            getWorld()->getActors().push_back(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
        break;
    case up:
        if (!getWorld()->isThereEarth(getX(), getY() + 4) && !getWorld()->isThereABoulder(getX(), getY()+4 ))
        getWorld()->getActors().push_back(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
        break;
    default:
        return;
    }
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

void TunnelMan::add(int ID) {
    if (ID == TID_GOLD) {nuggets++;}
    if (ID == TID_WATER_POOL) {water += 5;}
    if (ID == TID_SONAR) {sonar += 2;}
}

//************************************ BOULDER ***********************************

//TODO: Annoy MAN, check if needs to be in doSomething(), for all, the annoys are diff types, check why

void Boulder::annoyHuman() {
    if (getWorld()->playerWithinR(this, 3)) {
    getWorld()->getTM()->annoy(100);
    }
    Protester * p = getWorld()->protesterWithinR(this, 3);
    if (p != nullptr) {
        p->annoy(100);
    }
    
};

void Boulder::doSomething() {
    if (!isAlive()) return;
    if (isStable) {
        if (getWorld()->isAboveEarth(getX(), getY() - 1))
            return;
        else {isStable = false;}
    }
    if (m_ticks == 30) { //wait 30 ticks
        isFalling = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    m_ticks++;
    if (isFalling) {
        if (getWorld()->isAboveEarth(getX(), getY()-1)|| getWorld()->isThereABoulder(getX(), getY()-4, 0) || getY()==0) {
            die();
        }
        
        else {moveTo(getX(), getY() - 1);}
        
        annoyHuman();
    }
}

//************************************ Squirt ***********************************

//TODO: Annoy Protestors, check if needs to be in doSomething(), for all

bool Squirt::annoyProtesters() {
    Protester * p= getWorld()->protesterWithinR(this,3);
    if (p != nullptr) {
        p->annoy(2);
        return true;
    }
    return false;
}

void Squirt::doSomething() {
    if(!isAlive()) {return;}
    
    if (annoyProtesters() || distanceTravel == 4 ) {
        die();
        return;
    }
    
    switch(getDirection()) {
        case left:
            if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereABoulder(getX() - 1, getY())) {
                die();
                return;
                }
            else {moveTo(getX() - 1, getY());}
            break;
        case right:
            if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereABoulder(getX() + 1, getY())) {
                
                die();
                return;
                }
            else {moveTo(getX() + 1, getY());}
            break;
        case up:
            if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereABoulder(getX(), getY() + 1)) {
                
                die();
                return;
                }
            else {moveTo(getX(), getY() + 1);}
            break;
        case down:
            if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereABoulder(getX() , getY() - 1)) {
                
                die();
                return;
                }
            else {moveTo(getX(), getY() - 1);}
            break;
        default:
            return;
    }
    distanceTravel++;
}


//************************************ ITEM ***********************************

void Item::disappear(int ticks) {
    if (ticks == m_ticks) {die();}
    else {m_ticks++;}
}

//************************************ BARREL ***********************************

void Barrel::doSomething() {
    
    if (!isAlive()) {return;}
    
    if (!isVisible() && getWorld()->playerWithinR(this, 4)) {
        setVisible(true);
        return;
    }
    if (getWorld()->playerWithinR(this, 3)) {
        die();
        getWorld()->playSound(SOUND_FOUND_OIL);
        
        getWorld()->increaseScore(1000);
        
        getWorld()->minusBarrel();
        return;
    }
}

//************************************ Gold ***********************************

void Gold::doSomething() {
    if (!isAlive()) return;
        if (!isVisible() && getWorld()->playerWithinR(this, 4)) {
            setVisible(true);
            return;
        }
        if (!isDropped && getWorld()->playerWithinR(this, 3)) {
            die();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->getTM()->add(getID());
            getWorld()->increaseScore(10);
            return;
        }
        if (isDropped) {
            Protester* p = (getWorld()->protesterWithinR(this, 3));
            if (p != nullptr) {
                die();
                p->Bribed();
            };
            disappear(100);
        }
}

//************************************ POWERUP ***********************************

void PowerUp::doSomething(){
    if (!isAlive()) {return;}
    if (getWorld()->playerWithinR(this, 3)) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTM()->add(getID());
        getWorld()->increaseScore(75);
        return;
    }
    disappear(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

//************************************ PROTESTER ***********************************

Protester::Protester(StudentWorld* world, int imageID, int hp): Human(world, imageID, 60, 60, left, hp) {
    randomizeMove();
    
    ticksToWaitBetweenMoves = max(0, 3 - (int)getWorld()->getLevel() / 4);
};

void Protester::randomizeMove() {
    randomMove = rand() % 53 + 8;
}

void Protester::doSomething() {
    if (!isAlive()) {return;}
    
    if (ticksToWaitBetweenMoves > 0) {ticksToWaitBetweenMoves--; return;}
    
    else {
        ticksToWaitBetweenMoves = max(0, 3 - (int)getWorld()->getLevel() / 4);
        tickSinceLastTurn++;
        tickNoYell++;
    }
    
    //leave if annoyed
    
    if(leave == true) {
        if(getX() == 60 && getX() == 60) {
            die();
            getWorld()->minusProtester();return;
        }
        getWorld()->moveTowardsExit(this);
    }
    
    
    //yell!!!
    if (getWorld()->playerWithinR(this, 4) && facingPlayer()) {
        if (tickNoYell > 15) {
            getWorld()->getTM()->annoy(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            tickNoYell = 0;
            return;
        }
        return;
    }
    
    //hardcore
    
    if(getID()==TID_HARD_CORE_PROTESTER){
        int f = 16 + int(getWorld()->getLevel());
        
        Direction s = getWorld()->senseTM(this, f);
        
        if(s != none){
            moveInDirection(s);
            return;
        }
    }
    
    //get TM if near
    
    Direction d = directionToPlayer();
    if (d != none && straightPathToPlayer(d) && (!getWorld()->playerWithinR(this, 4))) {
        setDirection(d);
        moveInDirection(d);
        randomMove = 0;
        return;
    }
    
    //move
    randomMove--;
    if (randomMove <= 0) {
        Direction k = none;
        while (true) {
            k = randomDirection();
            if (getWorld()->canMoveInDirection(getX(),getY(), k)) {break;}
        }
        setDirection(k);
        randomizeMove();
    }
    
    else if (isAtIntersection() && tickSinceLastTurn > 200) {
        pickDirectionToTurn();
        tickSinceLastTurn = 0;
        randomizeMove();
    }

    moveInDirection(getDirection());

    if (!getWorld()->canMoveInDirection(getX(),getY(),getDirection())) {
        randomMove = 0;
    }
}

bool Protester::facingPlayer() {
    switch (getDirection()) {
        case left:
            return getWorld()->getTM()->getX() <= getX();
        case right:
            return getWorld()->getTM()->getX() >= getX();
        case up:
            return getWorld()->getTM()->getY() >= getY();
        case down:
            return getWorld()->getTM()->getY() <= getY();
        default:
            return false;
    }
}

GraphObject::Direction Protester::directionToPlayer()
{
    if (getY() == getWorld()->getTM()->getY() && getX() == getWorld()->getTM()->getX())
    {
        return getDirection();
    }
    if (getX() == getWorld()->getTM()->getX()) {
        if (getY() < getWorld()->getTM()->getY())
            return up;
        if (getY() > getWorld()->getTM()->getY())
            return down;
    }
    if (getY() == getWorld()->getTM()->getY()) {
        if (getX() > getWorld()->getTM()->getX())
            return left;
        if (getY() < getWorld()->getTM()->getY())
            return right;
    }
    
    return none;   // didnt see player in straight line
}

GraphObject::Direction Protester::randomDirection()
{
    int r = rand() % 4;
    switch (r) {
        case 0: return left;
        case 1: return right;
        case 2: return up;
        case 3: return down;
    }
    return none;
}

bool Protester::isAtIntersection()
{
    if (getDirection() == right || getDirection() == left) {
        return (getWorld()->canMoveInDirection(getX(),getY(), up) || getWorld()->canMoveInDirection(getX(),getY(), down));
    }
    else
        return (getWorld()->canMoveInDirection(getX(),getY(), right) || getWorld()->canMoveInDirection(getX(),getY(), left));
}

void Protester::pickDirectionToTurn()
{
    if (getDirection() == up || getDirection() == down) {
        if (!getWorld()->canMoveInDirection(getX(),getY(), left)) {setDirection(right);}
        
        else if (!getWorld()->canMoveInDirection(getX(),getY(), right)) {setDirection(left);}
        
        else {
            switch (rand() % 2) {
            case 0: setDirection(left);
            case 1: setDirection(right);
            }
        }
    }
    else {
        if (!getWorld()->canMoveInDirection(getX(),getY(), up)) setDirection(down);
        
        else if (!getWorld()->canMoveInDirection(getX(),getY(), down)) setDirection(up);
        
        else {
            switch (rand() % 2) {
            case 0: setDirection(up);
            case 1: setDirection(down);
            }
        }
    }

}

void Protester::getStunned() {
    ticksToWaitBetweenMoves = max(50, 100 - (int)getWorld()->getLevel()*10);
}

void Protester::annoy(int hp) {
    if (leave) {return;}
    
    loseHP(hp);
    getWorld()->playSound(SOUND_PLAYER_ANNOYED);
    getStunned();
    
    if (getHP() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        leave = true;
        ticksToWaitBetweenMoves = 0;
        if(hp == 100) {getWorld()->increaseScore(500);} // hit by boulder
        
        else if(getID()==TID_PROTESTER) {getWorld()->increaseScore(100);}
        
        else {getWorld()->increaseScore(250);}
    }
}

void Protester::Bribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    
    if(getID()==TID_PROTESTER) {
        
        getWorld()->increaseScore(25);
        
        leave = true;
    }
    else {
        getWorld()->increaseScore(50);
        
        getStunned();
    }
}

void Protester::moveInDirection(Direction direction)
{
    switch (direction) {
    case left:
        if (getDirection() == left) {
            if (getX() == 0) {setDirection(right);}
            moveTo(getX() - 1, getY());
        }
        else setDirection(left);
        break;
    case right:
        if (getDirection() == right) {
            if (getX() == 60) {setDirection(left);}
            moveTo(getX() + 1, getY());
        }
        else setDirection(right);
        break;
    case up:
        if (getDirection() == up) {
            if (getY() == 60) {setDirection(down);}
            moveTo(getX(), getY() + 1);
        }
        else setDirection(up);
        break;
    case down:
        if (getDirection() == down) {
            if (getY() == 0) {setDirection(up);}
            moveTo(getX(), getY() - 1);
        }
        else {setDirection(down);}
        break;
            
    default:
        return;
    }
}

bool Protester::straightPathToPlayer(Direction direction)
{
    int playerX = getWorld()->getTM()->getX();
    int playerY = getWorld()->getTM()->getX();
    switch (direction) {
        case left:
            for (int i = getX(); i >= playerX; i--) {
                if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereABoulder(i, getY()))
                    return false;
            }
            return true;
            break;
        case right:
            for (int i = getX(); i <= playerX; i++) {
                if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereABoulder(i, getY()))
                    return false;
            }
            return true;
            break;
        case up:
            for (int j = getY(); j <= playerY; j++) {
                if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereABoulder(getX(), j))
                    return false;
            }
            return true;
            break;
        case down:
            for (int j = getY(); j >= playerY; j--) {
                if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereABoulder(getX(), j))
                    return false;
            }
            return true;
            break;
        default:
            return false;
    }
}
