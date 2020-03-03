#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), p(nullptr), finished(false)
{}

int StudentWorld::init()
{
    
    p = new Penelope(IID_PLAYER, 0, 0, 0, 0, 1, this);
    Level lev(assetPath());
    string levelFile;
    if(getLevel() < 10){
        levelFile = "level0" + to_string(getLevel()) + ".txt";
    }
    else {
        levelFile = "level" + to_string(getLevel()) + ".txt";
    }
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
        //cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        cout << li.size() << endl;
        //Level::MazeEntry ge = lev.getContentsOf(5,10); // level_x=5, level_y=10
        for(int i = 0; i < LEVEL_WIDTH; i++) {
            for(int j = 0; j < LEVEL_HEIGHT; j++) {
                Level::MazeEntry ge = lev.getContentsOf(i,j); // level_x=5, level_y=10
                switch (ge) // so x=80 and y=160
                {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        //cout << "Location 80,160 starts with a smart zombie" << endl;
                        break;
                    case Level::dumb_zombie:
                        li.push_front(new DumbZombie(IID_ZOMBIE, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 1, 1, this));
                        break;
                    case Level::player:
                        p->moveTo(i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        cout << "Location 80,160 is where Penelope starts" << endl;
                        break;
                    case Level::citizen:
                        li.push_front(new Citizen(IID_CITIZEN, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 0, 1, this));
                        break;
                    case Level::exit:
                        li.push_front(new Exit(IID_EXIT, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 1, 1, this));
                        break;
                    case Level::wall:
                        li.push_front(new Wall(IID_WALL, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 0));
                        break;
                    case Level::pit:
                        li.push_front(new Pit(IID_PIT, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 0, 1, this));
                        break;
                    case Level::landmine_goodie:
                        li.push_front(new LandmineGoodie(IID_LANDMINE_GOODIE, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 1, 1, this));
                        break;
                    case Level::vaccine_goodie:
                        li.push_front(new VaccineGoodie(IID_VACCINE_GOODIE, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 1, 1, this));
                        break;
                    case Level::gas_can_goodie:
                        li.push_front(new GasCanGoodie(IID_GAS_CAN_GOODIE, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, 0, 1, 1, this));
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    if(!p->isAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    p->doSomething();
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->isAlive()) (*p)->doSomething();
        else {
            Actor* a = *p;
            delete a;
            p = li.erase(p);
            p--;
        }
    }
    setGameStatText("Score: " + to_string(getScore()) + "  Level: " + to_string(getLevel()) + "  Lives: " + to_string(getLives()) + "  Vaccines: " + to_string(p->getNumVaccines()) + "  Flames: " + to_string(p->getNumFlameCharges()) + "  Mines: " + to_string(p->getNumLandmines()) + "  Infected: " + to_string(p->getInfectionDuration()));
    if(finished) {
        finished = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete p;
    p = nullptr;
    
    
    
    for(list<Actor*>::iterator itr = li.begin(); itr != li.end(); itr++)
    {
        Actor* a = *itr;
        delete a;
        itr = li.erase(itr);
        itr--;
    }
    li.clear();
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

bool StudentWorld::blocks(double x, double y, Actor* self) {
    if(p != self && abs((p)->getX() - x) < SPRITE_WIDTH && abs((p)->getY() - y) < SPRITE_HEIGHT) {
        return true;
    }

    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if(*p != self && (*p)->isBlock() && abs((*p)->getX() - x) < SPRITE_WIDTH && abs((*p)->getY() - y) < SPRITE_HEIGHT) {
            return true;
        }
    }
    return false;
    
}

bool StudentWorld::checkForCitizens(const int x, const int y) {
    bool no_citizens = true;
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->triggersZombieVomit()) {
            no_citizens = false;
            /*
            double xChange = abs((*p)->getX() - x);
            double yChange = abs((*p)->getY() - y);
            if(xChange*xChange + yChange*yChange <= 100) {
                //set Citizen to dead
            }
             */
            //
            if(overlapping((*p)->getX(), x, (*p)->getY(), y)) {
                increaseScore(500);
                (*p)->setDead();
                playSound(SOUND_CITIZEN_SAVED);
            }

        }
    }
    //if(no_citizens) {
        if(overlapping(p->getX(), x, p->getY(), y)) finished = true;
    //}
    return true;
}

void StudentWorld::calculateDirection(int dir, int &x, int &y) {
    switch (dir)
    {
        case 180:
            x = -1;
            y = 0;
            
            break;
        case 0:
            x = 1;
            y = 0;
            
            break;
        case 90:
            x = 0;
            y = 1;
            
            break;
        case 270:
            x = 0;
            y = -1;
            break;
        default:
            break;
    }
}
bool StudentWorld::overlapping(double x1, double x2, double y1, double y2) const {
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) <= 100;
}
/*
bool StudentWorld::contactPlayer(double x, double y, int tag) {
    if(overlapping(x, p->getX(), y, p->getY())) {
        increaseScore(50);
        
        p->addGoodie(tag);
        return true;
    }
    return false;
}
*/
int StudentWorld::findClosestPerson(double x, double y) {
    
    double min = sqrt((p->getX()-x)*(p->getX()-x) + (p->getY()-y)*(p->getY()-y));
    double minX = p->getX();
    double minY = p->getY();
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        double temp = sqrt(((*p)->getX()-x)*((*p)->getX()-x) + ((*p)->getY()-y)*((*p)->getY()-y));
        if(temp < min) {
            min = temp;
            minX = (*p)->getX();
            minY = (*p)->getX();
        }
    }
    int dir = 0;
    if(minX - x == 0) {
        dir = (minY - y) / abs(minY - y) * (90) + 180;

    }
    else if(minY - y == 0) {
        dir = (minX - x) / abs(minX - x) * (-90) + 90;

    }
    else {
        int dir1 = (minX - x) / abs(minX - x) * (-90) + 90;
        int dir2 = (minY - y) / abs(minY - y) * (90) + 180;
        if(randInt(1, 2) == 1) {
            dir = dir1;
        }
        else {
            dir = dir2;
        }
    }

    return dir;
}

void StudentWorld::damageObjects(double x, double y) {
    cout << "start of damage" << endl;
    if(overlapping((p)->getX(), x, (p)->getY(), y)) {
        p->setDead();
        playSound(SOUND_PLAYER_DIE);
        increaseScore(p->scoreWhenDestroyed());
    }
    
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->isDamagable() && overlapping((*p)->getX(), x, (*p)->getY(), y)) {
            (*p)->setDead();
            if((*p)->triggersZombieVomit()) playSound(SOUND_CITIZEN_DIE);
            else playSound(SOUND_ZOMBIE_DIE);
            increaseScore((*p)->scoreWhenDestroyed());
        }
        if((*p)->isTriggeredByFlame() && overlapping((*p)->getX(), x, (*p)->getY(), y)) {
            explodeLandmine((*p)->getX(), (*p)->getY());
            (*p)->setDead();
        }

    }
}

bool StudentWorld::checkGoodieOverlap(double x, double y, Goodie* a) {
    if(overlapping((p)->getX(), x, (p)->getY(), y)) {
        increaseScore(50);
        a->overlaps(p);
        playSound(SOUND_GOT_GOODIE);
        return true;
    }
    return false;
}

bool StudentWorld::infectObjects(double x, double y) {
    bool sentVomit = false;
    if(overlapping((p)->getX(), x, (p)->getY(), y)) {
        //cout << "here" << endl;
        p->infect();
        //li.push_front(new Vomit(IID_VOMIT, x, y, 0, 0, 1, this));
        sentVomit = true;
    }
    int test = 0;
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->triggersZombieVomit() && overlapping((*p)->getX(), x, (*p)->getY(), y)) {
            test ++;
            (*p)->infect();
            //li.push_front(new Vomit(IID_VOMIT, x, y, 0, 0, 1, this));
            sentVomit = true;
        }
    }
    //if(test > 0) cout << test << endl;
    return sentVomit;
}

bool StudentWorld::createVomit(double x, double y) {
    bool sentVomit = false;
    if(overlapping((p)->getX(), x, (p)->getY(), y)) {
        playSound(SOUND_ZOMBIE_VOMIT);
        li.push_front(new Vomit(IID_VOMIT, x, y, 0, 0, 1, this));
        sentVomit = true;
    }
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->triggersZombieVomit() && overlapping((*p)->getX(), x, (*p)->getY(), y)) {
            playSound(SOUND_ZOMBIE_VOMIT);
            li.push_front(new Vomit(IID_VOMIT, x, y, 0, 0, 1, this));
            sentVomit = true;
        }
    }
    return sentVomit;
}

void StudentWorld::sendFlames(int dir) {
    
    int dx = 0;
    int dy = 0;
    calculateDirection(dir, dx, dy);
    
    dx *= SPRITE_WIDTH;
    dy *= SPRITE_HEIGHT;
   
    for(int i = 1; i < 4; i++) {
        for(list<Actor*>::iterator itr = li.begin(); itr != li.end(); itr++)
            if((*itr)->blocksFlame() && overlapping((*itr)->getX(),  p->getX() + dx*i, (*itr)->getY(), p->getY() + dy*i)) return;
        li.push_front(new Flame(IID_FLAME, p->getX() + dx*i, p->getY() + dy*i, 0, 1, 1, this));
    }
}

bool StudentWorld::checkExplode(double x, double y) {
    bool exploded = false;
    if(overlapping((p)->getX(), x, (p)->getY(), y)) {
        explodeLandmine(x, y);
        exploded = true;
    }
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->isBlock() && overlapping((*p)->getX(), x, (*p)->getY(), y)) {
            explodeLandmine(x, y);
            exploded = true;
        }
    }
    return exploded;
}

void StudentWorld::explodeLandmine(double x, double y) {
    playSound(SOUND_LANDMINE_EXPLODE);
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            li.push_front(new Flame(IID_FLAME, x + i*SPRITE_WIDTH, y + j*SPRITE_HEIGHT, 90, 0, 1, this));
        }
    }
    li.push_front(new Pit(IID_PIT, x, y, 0, 0, 1, this));
    cout << "end of explode" << endl;
}

double StudentWorld::distanceToPenelope(double x, double y, double &xPenelope, double &yPenelope) {
    xPenelope = p->getX();
    yPenelope = p->getY();
    return sqrt((x-p->getX())*(x-p->getX()) + (y-p->getY())*(y-p->getY()));
}

double StudentWorld::distanceToZombie(double x, double y) {
    if(li.size() == 0) return -1;
    double min = -1;
    for(list<Actor*>::iterator p = li.begin(); p != li.end(); p++) {
        if((*p)->isSearchedByCitizen()) {
            double temp = sqrt((x-(*p)->getX())*(x-(*p)->getX()) + (y-(*p)->getY())*(y-(*p)->getY()));
            if(temp < min || min < 0) min = temp;
        }
    }
    return min;
}
