#include "snake.hpp"
#include <iostream>

using namespace std;

int main()
{
    Snake snake;
    
    // Start random seed
    srand(time(NULL));

    // Initiate screen and start game logic
    snake.draw();
    logic();

    return 0;
}