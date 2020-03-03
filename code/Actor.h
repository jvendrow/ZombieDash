#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h"
#include <list>

class StudentWorld;
class Penelope;
using namespace std;


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class Actor: public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething() = 0;
    virtual int scoreWhenDestroyed() {return 0;};
    virtual bool isBlock() const {return false;};
    virtual bool isPenelope() const {return false;};
    virtual bool isSearchedByCitizen() const {return false;};
    virtual bool isDamagable() const {return false;};
    virtual void infect() {};
    virtual bool triggersZombieVomit() const {return false;}
    virtual bool blocksFlame() const {return false;};
    virtual bool isTriggeredByFlame() const {return false;}
    
    StudentWorld* getWorld() const;
    
    bool isAlive() {return m_isAlive;};
    void setDead() {m_isAlive = false;};

private:
    bool m_isAlive;
    StudentWorld* world;
    

};

class MovingThing: public Actor
{
public:
    MovingThing(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething() = 0;
    bool isBlock() const {return true;};
    virtual bool isDamagable() const {return true;};
    bool checkAndReplaceParalysis();
    
private:
    bool isParalyzed;
};

class Person: public MovingThing
{
public:
    Person(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething() = 0;
    virtual bool triggersZombieVomit() const {return true;}
    void infect();
    int getInfectionDuration();
    void increaseCount();
    void clearInfection();
    virtual int scoreWhenDestroyed() {return -1000;};
    
private:
    bool isInfected;
    int infection_count;
    
};

class Penelope: public Person
{
public:
    Penelope(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething();
    virtual bool isPenelope() const {return true;};
    void addGoodie(int goodie);
    
    // Increase the number of vaccines the object has.
    void increaseVaccines();
    
    // Increase the number of flame charges the object has.
    void increaseFlameCharges();
    
    // Increase the number of landmines the object has.
    void increaseLandmines();
    
    // How many vaccines does the object have?
    int getNumVaccines() const {return vaccineGoodies;};
    
    // How many flame charges does the object have?
    int getNumFlameCharges() const {return gasCanGoodies;};
    
    // How many landmines does the object have?
    int getNumLandmines() const {return landmineGoodies;};
    
private:
    //StudentWorld* world;
    int landmineGoodies;
    int gasCanGoodies;
    int vaccineGoodies;
};

class Citizen: public Person
{
public:
     Citizen(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual bool isCitizen() const {return true;};
    virtual void doSomething();
};

class Zombie: public MovingThing
{
public:
    Zombie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    bool tryToInfect();
    bool checkAndReplaceMoves();
    virtual bool isSearchedByCitizen() const {return true;};
    
private:
    int moves_left;
};

class SmartZombie: public Zombie
{
public:
    SmartZombie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual int scoreWhenDestroyed() {return 2000;};
    void doSomething();
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual int scoreWhenDestroyed() {return 1000;};
    void doSomething();
    
};

class Damager: public Actor
{
public:
    Damager(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    void tick();
    
private:
    int ticks;
};

class Landmine: public Damager
{
public:
    Landmine(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething();
    virtual bool isTriggeredByFlame() const {return true;}
    
private:
    int safety_ticks;
    bool isActive;

};

class Pit: public Damager
{
public:
    Pit(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething();
};

class Flame: public Damager
{
public:
    Flame(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething();
    
};

class Vomit: public Damager
{
public:
    Vomit(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething();
    
};

class Goodie: public Actor
{
public:
    Goodie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual bool isDamagable() const {return true;};
    virtual void overlaps(Penelope* a) {};
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    void doSomething();
    virtual void overlaps(Penelope* a);
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    void doSomething();
    virtual void overlaps(Penelope* a);
    
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    void doSomething();
    virtual void overlaps(Penelope* a);
private:
};
/*
class Externality: public Actor
{
public:
    Externality(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething() = 0;
    
};
 */

class Wall: public Actor
{
public:
    Wall(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld* game_world = nullptr);
    virtual void doSomething() {};
    bool isBlock() const {return true;};
    virtual bool blocksFlame() const {return true;};

    
};

class Exit: public Actor
{
public:
    Exit(int imageID, double startX, double startY, Direction dir = 0, int depth = 1, double size = 1.0, StudentWorld* game_world = nullptr);
    bool isFinished() {return finished;};
    virtual bool blocksFlame() const {return true;};
    void doSomething();
private:
    bool finished;
    
};

#endif // ACTOR_H_


