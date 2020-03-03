#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include "Actor.h"
#include <list>

class Actor;
class Penelope;
class Goodie;

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();
    void addActor(Actor *a) {li.push_front(a);};
    
    bool blocks(double x, double y, Actor* self);
    bool checkForCitizens(const int x, const int y);
    //bool contactPlayer(double x, double y, int tag);
    bool overlapping(double x1, double x2, double y1, double y2) const;
    int findClosestPerson(double x, double y);
    void damageObjects(double x, double y);
    bool infectObjects(double x, double y);
    bool createVomit(double x, double y);
    void sendFlames(int direction);
    void calculateDirection(int dir, int &x, int &y);
    bool checkExplode(double x, double y);
    void explodeLandmine(double x, double y);
    
    bool checkGoodieOverlap(double x, double y, Goodie* a);
    
    double distanceToPenelope(double x, double y, double &xPenelope, double &yPenelope);
    double distanceToZombie(double x, double y);
    
private:
    Penelope* p;
    list<Actor*> li;
    bool finished;
};

#endif // STUDENTWORLD_H_
