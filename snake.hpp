#include <iostream>

// Global variables
extern int width;
extern int height;
extern int score;
extern int maxScore;
extern bool movement;

using namespace std;

// Game logic
void logic();
void multiThread();
void gameOver();

// Snake Class
class Snake {
    public:
        void draw();
        void move();
        void prompt();
        void changeDir();
        void collision();
        void bodyPosition();
        
        char dir = 'd';
        int speed = 60;
        int length = 1;
        int tailX[150], tailY[150];
        
        // Coordinates of Head
        struct Head
        {
            int x;
            int y;
            Head() : x(30), y(10) {}

        } head;
};

// Fruit Class
class Fruit {
    public:
        void genFruitPos();
        void effect();

        int fruitX, fruitY, fruitType;
        bool spawn = true;
};
