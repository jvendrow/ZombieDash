#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:GraphObject(imageID, startX, startY, dir, depth, size), m_isAlive(true), world(game_world)
{};

StudentWorld* Actor::getWorld() const {
    return world;
}

MovingThing::MovingThing(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world),  isParalyzed(false)
{};


bool MovingThing::checkAndReplaceParalysis() {
    isParalyzed = !isParalyzed;
    return !isParalyzed;
}
/*
bool MovingThing::intersects(int x, int y) {
    for(list<Actor*>::iterator p = others.begin(); p != others.end(); p++) {
        if(abs((*p)->getX() - x) < SPRITE_WIDTH && abs((*p)->getY() - y) < SPRITE_HEIGHT) {
            return true;
        }
    }
    return false;
}

void MovingThing::addOther(Actor* a) {
    others.push_back(a);
}
*/
Person::Person(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:MovingThing(imageID, startX, startY, dir, depth, size, game_world), isInfected(false), infection_count(0)
{};

void Person::infect()
{
    isInfected = true;
    if(infection_count == 0){
        infection_count ++;
    }
}

int Person::getInfectionDuration()
{
    return infection_count;
}

void Person::increaseCount() {
    infection_count++;
}

void Person::clearInfection() {
    isInfected = false;
    infection_count = 0;
}
Penelope::Penelope(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Person(imageID, startX, startY, dir, depth, size, game_world), landmineGoodies(0), gasCanGoodies(0), vaccineGoodies(0)
{};

void Penelope::doSomething() {
    if(getInfectionDuration() > 0) {
         increaseCount();
    }
    if(getInfectionDuration() > 500) {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        int newx;
        int newy;
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                newx = getX() - 4;
                newy = getY();
                setDirection(180);
               // if(!getWorld()->blocks(newx, newy, this)) {
                    moveTo(newx, newy);
                //}
                
                break;
            case KEY_PRESS_RIGHT:
                newx = getX() + 4;
                newy = getY();
                setDirection(0);
                if(!getWorld()->blocks(newx, newy, this)) {
                    moveTo(newx, newy);
                }
                break;
            case KEY_PRESS_UP:
                newx = getX();
                newy = getY() + 4;
                setDirection(90);
                if(!getWorld()->blocks(newx, newy, this)) {
                    moveTo(newx, newy);
                }
                break;
            case KEY_PRESS_DOWN:
                newx = getX();
                newy = getY() - 4;
                setDirection(270);
                if(!getWorld()->blocks(newx, newy, this)) {
                    moveTo(newx, newy);
                }
                break;
            case KEY_PRESS_SPACE:
                if(gasCanGoodies > 0) {
                    getWorld()->sendFlames(getDirection());
                    gasCanGoodies --;
                }
            case KEY_PRESS_ENTER:
                if(vaccineGoodies > 0) {
                    clearInfection();
                    vaccineGoodies--;
                } 
                break;
            case KEY_PRESS_TAB:
                if(landmineGoodies > 0) {
                    getWorld()->addActor(new Landmine(IID_LANDMINE, getX(), getY(), 0, 1, 1, getWorld()));
                    landmineGoodies--;
                }
                break;
                // …
        }
    }
}

// Increase the number of vaccines the object has.
void Penelope::increaseVaccines()
{
    vaccineGoodies++;
}

// Increase the number of flame charges the object has.
void Penelope::increaseFlameCharges()
{
    gasCanGoodies += 5;
}

// Increase the number of landmines the object has.
void Penelope::increaseLandmines()
{
    landmineGoodies += 2;
}

/*
void Penelope::addGoodie(int goodie) {
    if(goodie == LANDMINE_GOODIE_TAG) {
        landmineGoodies += 2;
    }
    if(goodie == GAS_CAN_GOODIE_TAG) {
        gasCanGoodies += 5;
    }
    if(goodie == VACCINE_GOODIE_TAG) {
        vaccineGoodies++;
    }
}
*/

Citizen::Citizen(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Person(imageID, startX, startY, dir, depth, size, game_world)
{};

void Citizen::doSomething()
{
    if(!isAlive()) {
        return;
    }
    if(getInfectionDuration() > 0) {
        increaseCount();
    }
    if(getInfectionDuration() > 500) {
        setDead();
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        getWorld()->increaseScore(-1000);
        if(randInt(1, 10) <= 7) {
            getWorld()->addActor(new DumbZombie(IID_ZOMBIE, getX(), getY(), 0, 0, 1, getWorld()));
        }
        else {
            getWorld()->addActor(new SmartZombie(IID_ZOMBIE, getX(), getY(), 0, 0, 1, getWorld()));
        }
        return;
    }
    
    if(checkAndReplaceParalysis()) return;
    double pX;
    double pY;
    double zombieDist = getWorld()->distanceToZombie(getX(), getY());
    double PenelopeDist = getWorld()->distanceToPenelope(getX(), getY(), pX, pY);
    double dx = 0;
    double dy = 0;
    if((PenelopeDist < zombieDist || zombieDist == -1) && PenelopeDist < 80) {
        dy = (pY - getY()) / abs(pY - getY()) * 2;
        dx = (pX - getX()) / abs(pX - getX()) * 2;
        if(pX == getX()) {
            setDirection(180 - dy * 45);
            if(!getWorld()->blocks(getX(), getY() + dy, this))
               moveTo(getX(), getY() + dy);
        }
        else if(pY == getY()) {
            setDirection(90 - dx * 45);
            if(!getWorld()->blocks(getX() + dx, getY(), this))
                moveTo(getX() + dx, getY());
        }
        else {
            if(randInt(1, 2) == 1) {
                if(!getWorld()->blocks(getX(), getY() + dy, this))
                    moveTo(getX(), getY() + dy);
                else if(!getWorld()->blocks(getX() + dx, getY(), this))
                    moveTo(getX() + dx, getY());
            }
            else {
                if(!getWorld()->blocks(getX() + dx, getY(), this))
                    moveTo(getX() + dx, getY());
                else if(!getWorld()->blocks(getX(), getY() + dy, this))
                    moveTo(getX(), getY() + dy);
            }
        }
    }
    else if(zombieDist < 80 && zombieDist != -1) {
        double maxDist = -1;
        double maxDir = -1;
        if(!getWorld()->blocks(getX() + 2, getY(), this)) {
            maxDist = getWorld()->distanceToZombie(getX() + 2, getY());
            maxDir = 0;
            dx = 2;
            dy = 0;
        }
        if(!getWorld()->blocks(getX() - 2, getY(), this)) {
            double temp = getWorld()->distanceToZombie(getX() - 2, getY());
            if(temp > maxDist || maxDist == -1) {
                maxDist = temp;
                maxDir = 180;
                dx = -2;
                dy = 0;
            }

        }
        if(!getWorld()->blocks(getX(), getY() + 2, this)) {
            double temp = getWorld()->distanceToZombie(getX(), getY() + 2);
            if(temp > maxDist || maxDist == -1) {
                maxDist = temp;
                maxDir = 90;
                dx = 0;
                dy = 2;
            }

        }
        if(!getWorld()->blocks(getX(), getY() - 2, this)) {
            double temp = getWorld()->distanceToZombie(getX(), getY() - 2);
            if(temp > maxDist || maxDist == -1) {
                maxDist = temp;
                maxDir = 270;
                dx = 0;
                dy = -2;
            }
        }
        if(maxDir == -1 || maxDist < zombieDist) return;
        setDirection(maxDir);
        moveTo(getX() + dx, getY() + dy);
    }
}

Zombie::Zombie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:MovingThing(imageID, startX, startY, dir, depth, size, game_world),  moves_left(0)
{};

bool Zombie::checkAndReplaceMoves() {
    if(moves_left == 0) {
        moves_left = randInt(3, 10);
        return false;
    }
    moves_left --;
    return true;
}

bool Zombie::tryToInfect() {
    if(randInt(1, 3) == 1) {
        int dx;
        int dy;
        getWorld()->calculateDirection(getDirection(), dx, dy);
        double newx = dx * SPRITE_WIDTH + getX();
        double newy = dy * SPRITE_HEIGHT + getY();
        return getWorld()->createVomit(newx, newy);
    }
    return false;
}

SmartZombie::SmartZombie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Zombie(imageID, startX, startY, dir, depth, size, game_world)
{};

void SmartZombie::doSomething() {
    if(!isAlive()) return;
    if(!checkAndReplaceParalysis()) {
        if(tryToInfect()) return;
        if(!checkAndReplaceMoves())
        {
            setDirection(getWorld()->findClosestPerson(getX(), getY()));
        }
        
        int dx;
        int dy;
        getWorld()->calculateDirection(getDirection(), dx, dy);
        double newx = dx + getX();
        double newy = dy + getY();
        
        if(!getWorld()->blocks(newx, newy, this)) {
            moveTo(newx, newy);
        }
         
    }
};

DumbZombie::DumbZombie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Zombie(imageID, startX, startY, dir, depth, size, game_world)
{};

void DumbZombie::doSomething() {
    if(!isAlive()) return;
    if(!checkAndReplaceParalysis()) {
        tryToInfect();
        if(!checkAndReplaceMoves())
        {
            setDirection(90 * randInt(0, 3));
        }
        
        int dx;
        int dy;
        getWorld()->calculateDirection(getDirection(), dx, dy);
        double newx = dx + getX();
        double newy = dy + getY();
        
        if(!getWorld()->blocks(newx, newy, this)) {
            moveTo(newx, newy);
        }
    }
};


Damager::Damager(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world), ticks(0)
{};

void Damager::tick() {
    ticks++;
    if(ticks == 2) setDead();
}

Landmine::Landmine(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Damager(imageID, startX, startY, dir, depth, size, game_world), isActive(false), safety_ticks(0)
{};

void Landmine::doSomething()
{
    if(!isAlive()) return;
    if(!isActive) {
        if(safety_ticks >= 30) isActive = true;
        safety_ticks++;
    }
    if(isActive) {
        if(getWorld()->checkExplode(getX(), getY())) setDead();
    }
}

Pit::Pit(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Damager(imageID, startX, startY, dir, depth, size, game_world)
{};

void Pit::doSomething()
{
    getWorld()->damageObjects(getX(), getY());
}

Flame::Flame(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Damager(imageID, startX, startY, dir, depth, size, game_world)
{};

void Flame::doSomething()
{
    if(!isAlive()) return;
    tick();
    getWorld()->damageObjects(getX(), getY());
}

Vomit::Vomit(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Damager(imageID, startX, startY, dir, depth, size, game_world)
{};

void Vomit::doSomething()
{
    if(!isAlive()) return;
    tick();
    getWorld()->infectObjects(getX(), getY());
}


Goodie::Goodie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world)
{};

VaccineGoodie::VaccineGoodie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Goodie(imageID, startX, startY, dir, depth, size, game_world)
{};

void VaccineGoodie::doSomething()
{
    if(getWorld()->checkGoodieOverlap(getX(), getY(), this)) setDead();
}

void VaccineGoodie::overlaps(Penelope *a) {
    a->increaseVaccines();
}
GasCanGoodie::GasCanGoodie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Goodie(imageID, startX, startY, dir, depth, size, game_world)
{};

void GasCanGoodie::doSomething()
{
    if(getWorld()->checkGoodieOverlap(getX(), getY(), this)) setDead();

}

void GasCanGoodie::overlaps(Penelope *a) {
    a->increaseFlameCharges();
}

LandmineGoodie::LandmineGoodie(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Goodie(imageID, startX, startY, dir, depth, size, game_world)
{};

void LandmineGoodie::doSomething()
{
    if(getWorld()->checkGoodieOverlap(getX(), getY(), this)) setDead();
}

void LandmineGoodie::overlaps(Penelope *a) {
    a->increaseLandmines();
}
/*
Externality::Externality(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world)
{};
*/
Wall::Wall(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world)
{};
 

Exit::Exit(int imageID, double startX, double startY, Direction dir, int depth, double size, StudentWorld* game_world)
:Actor(imageID, startX, startY, dir, depth, size, game_world)
{};

void Exit::doSomething() {
    getWorld()->checkForCitizens(getX(), getY());
}


