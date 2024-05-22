#include "snake.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <list>

using namespace std;

// All Variables
int width = 100;
int height = 30;
int score = 0;
int maxScore = 0;
bool movement = false;
char start = '\0';
Fruit fruit;
Snake snake;

// Game logic
void logic(){
    while(!movement){
        
        // Wait for player to start
        cout << "\033[1;42mENTER 'S' to START\033[0m\n";
        cout << "\033[1;42mENTER 'Q' to QUIT\033[0m\n";
        cin >> start;
        
        if(start == 's' || start == 'S') {
            movement = true;
            score = 0;
            snake.dir = 'd';
            snake.speed = 60;
            snake.length = 1;
            multiThread();
        } else if(start == 'q' || start == 'Q') abort();
    }
};

// Initiate Multithreading
void multiThread(){
    Snake snake;
    mutex mtx;
    // Create prompt thread
    thread promptThread([&snake, &mtx](){
        while (movement) {
            
            // Acquire mutex lock before modifying shared resources (Class Snake)
            mtx.lock(); // Prevent race condition
            
            snake.prompt();
            
            // Release mutex lock
            mtx.unlock();
        }
    });

    thread moveThread([&snake](){
        snake.move();
    });
    
    //Join prompt before exiting
    promptThread.join();
    moveThread.join();
}

// Game Over
void gameOver(){
    // Stop snake movement and call logic() to restart game
    movement = false;
    start = '\0';
    logic();
};



/* SNAKE CLASS */
// Draw game screen (instructions, walls, fruit, head, body segments, scores)
void Snake::draw(){
    this_thread::sleep_for(chrono::milliseconds(snake.speed));
    
    // Clear screen (2J) and moves cursor to top left (1;1H)
    cout << "\033[2J \033[1;1H";

    // Print control instructions
    cout << "    \33[1;4;32mCONTROLS\33[0m                          \33[1;4;32mFRUIT EFFECTS\33[0m\n";
    cout << "\33[1;36mW + ENTER\33[0m to move \33[1;36mUP\33[0m                 \33[1;41m \033[0m: \033[1;31mNo Effect (+10 POINTES)\033[0m\n";
    cout << "\33[1;36mA + ENTER\33[0m to move \33[1;36mLEFT\33[0m               \33[1;43m \033[0m: \033[1;33mIncrease Speed (+20 POINTS)\033[0m\n";
    cout << "\33[1;36mS + ENTER\33[0m to move \33[1;36mDOWN\33[0m               \33[1;45m \033[0m: \033[1;35mDecrease Speed (+20 POINTS)\033[0m\n";
    cout << "\33[1;36mD + ENTER\33[0m to move \33[1;36mRIGHT\33[0m\n";

    // For loop for printing grid
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            
            // Print boundary
            if(j==0 || j==width-1 || i==0 || i==height-1){
                // Print " " with blue background
                cout << "\033[44m \033[0m";

            // Print fruit
            } else if(i==fruit.fruitY && j==fruit.fruitX){

                switch (fruit.fruitType){
                    case 0: // Reset speed
                        // Print " " with bright red background (1;41) and reset (0)
                        cout << "\033[1;41m \033[0m";
                        break;
                    
                    case 1: // Increase speed
                        // Print " " with bright yellow background (1;41) and reset (0)
                        cout << "\033[1;43m \033[0m";
                        break;

                    case 2: // Decrease speed
                        // Print " " with bright magenta background (1;41) and reset (0)
                        cout << "\033[1;45m \033[0m";
                        break;

                    default:
                        continue;
                        break;
                }

            // Print head position
            } else if(i==head.y && j==head.x){
                // Print ":" with Bold green background (1;42) and reset (0)
                cout << "\033[1;42m:\033[0m";

            // Print body segments
            } else {
                bool print = false;
                for(int k=0; k<length; k++){
                    if(i==tailY[k] && j==tailX[k]){
                        // Print " " with Bright green background (1;42) and reset (0)
                        cout << "\033[1;42m \033[0m";
                        print = true;
                    }
                    
                    // Print " " if no body segments need to be printed
                } if(!print) {cout << " ";}
            }
        }
        
        // Set cursor to next line
        cout << endl;
    }
    
    // Print scoreboard
    cout << "\033[1;31m SCORE: " << score << "\033[0m" << endl;
    cout << "\033[1;4;33m HIGHEST SCORE: " << maxScore << "\033[0m\n" << endl;
};

// Snake movement logic
void Snake::move(){   
    while (movement){
        bodyPosition();
        changeDir();
        collision();
        fruit.genFruitPos();
        draw();
    }
}; 

// Prompt user input for direction
void Snake::prompt(){
    char c;
    cin >> c;
    if(c == 'w' || c == 'W' || c == 'a' || c == 'A' || c == 's' || c == 'S' || c == 'd' || c == 'D') dir = c;;
};


// Change snake direction
void Snake::changeDir(){
    switch (Snake::dir){
        // Upwards
        case 'W':
        case 'w':
            head.y--;
            break;
    
        // Leftwards
        case 'A':
        case 'a':
            head.x--;
            break;

        // Downwards
        case 'S':
        case 's':
            head.y++;
            break;

        // Rightwards
        case 'D':
        case 'd':
            head.x++;
            break;

        default:
            return;
    }
}

// Collision rule
void Snake::collision(){
        // Snake eating fruit
        if (head.x==fruit.fruitX && head.y==fruit.fruitY){
            fruit.spawn = true;
            fruit.effect();
            length += 5;
            if(score > maxScore) maxScore = score;

        // Snake hitting wall
        } else if (head.x<1 || head.x>width-1 || head.y<1 || head.y>height-2){
            std::cout << "You hit a wall!" << std::endl;
            gameOver();

        // Snake hitting itself
        } else for(int i=0; i<length; i++){
            if(head.x==tailX[i] && head.y==tailY[i]){
                std::cout << "You hit yourself!" << std::endl;
                gameOver();
            }
        }
};

// Body segment positions (stored in array)
void Snake::bodyPosition(){
    // [0] element to X, Y buffer
    int lastX = tailX[0];
    int lastY = tailY[0];

    // current head pos to [0] element
    tailX[0] = head.x;
    tailY[0] = head.y;

    // Initiate second set of buffer variables (2X, 2Y)
    int last2X, last2Y;

    // For loop to cycle buffer variables
    for(int i=1; i<length; i++){
        // [i] element -> 2X, 2Y
        last2X = tailX[i]; 
        last2Y = tailY[i];

        // X, Y -> [i] element
        tailX[i] = lastX;
        tailY[i] = lastY;

        // 2X, 2Y to X, Y
        lastX = last2X;
        lastY = last2Y;
    }
};



/* FRUIT CLASS*/
// Generate random fruit position
void Fruit::genFruitPos(){
    if(spawn == true){
        int x = width - 10;  // 2 units less than width
        int y = height - 10; // 2 units less than height
        fruitX = rand() % x + 5; // Random number between 5 and width-10
        fruitY = rand() % y + 5; // Random number between 5 and height-10
        fruitType = rand() % 3;
        spawn = false;
    }
};

// Different effects
void Fruit::effect(){
    switch (fruitType){
        case 0: // No Effect
            score += 10;
            break;
        
        case 1: // Increase speed
            if(snake.speed > 5) snake.speed -= 5;
            score += 20;
            break;

        case 2: // Decrease speed
            snake.speed += 5;
            score += 20;
            break;

        default:
            break;
    }
};