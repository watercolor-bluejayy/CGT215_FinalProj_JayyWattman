// CGT215_FinalProj_JayyWattman.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

//keyboard speed constant, affects how much power each key press imparts on the cbow
const float KB_Speed = 0.4; // magic number, might need to adjust

//function for loading textures that can be referenced later
void LoadTex(Texture& tex, string filename)
{
    if (!tex.loadFromFile(filename))
    {
        cout << "Could not load " << filename << endl;
    }
}

//need keyboard controls function
void MoveBar(PhysicsSprite& bar, int elapsedMS) 
{
    if (Keyboard::isKeyPressed(Keyboard::Right)) 
    {
       Vector2f newPos(bar.getCenter());
       newPos.x = newPos.x + (KB_Speed * elapsedMS);
        bar.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) 
    {
        Vector2f newPos(bar.getCenter());
        newPos.x = newPos.x - (KB_Speed * elapsedMS);
        bar.setCenter(newPos);
    }
}

int main()
{
    RenderWindow window(VideoMode(1000, 800), "BrickBreaker");
    World world(Vector2f(0, 1)); //Gravity set to 1 so the ball will bounce
    int score(0); //start at zero because you haven't earned any points yet
    int lives(3); //start at 3 so it can decrement down

    //make our walls, ceiling, and floor so the ball doesn't go flying off screen
    PhysicsRectangle floor; 
    floor.setSize(Vector2f(1000, 10)); 
    floor.setCenter(Vector2f(500, 790)); 
    floor.setStatic(true); //makes sure that the floor doesn't move
    world.AddPhysicsBody(floor); 

    PhysicsRectangle LeftWall;
    LeftWall.setSize(Vector2f(10, 780));
    LeftWall.setCenter(Vector2f(995, 400));
    LeftWall.setStatic(true);
    world.AddPhysicsBody(LeftWall);

    PhysicsRectangle RightWall;
    RightWall.setSize(Vector2f(10, 780));
    RightWall.setCenter(Vector2f(5, 400));
    RightWall.setStatic(true);
    world.AddPhysicsBody(RightWall);

    PhysicsRectangle ceiling; 
    ceiling.setSize(Vector2f(1000, 10)); 
    ceiling.setCenter(Vector2f(500, 10)); 
    ceiling.setStatic(true); 
    world.AddPhysicsBody(ceiling); 

    //make the ball
    PhysicsCircle ball;
    ball.setCenter(Vector2f(500, 300)); //set it to start here so hopefully there's time for player to react to the drop
    ball.setRadius(15);
    world.AddPhysicsBody(ball);
    //add some velo to get the direction going
    ball.applyImpulse(Vector2f(-0.02, -0.5));

    //make the bounce bar
    PhysicsSprite& bar = *new PhysicsSprite(); 
    Texture barTex;
    LoadTex(barTex, "images/bar.png");
    bar.setTexture(barTex);
    Vector2f sz = bar.getSize();
    bar.setCenter(Vector2f(500, 770-(sz.y/2)));
    bar.setStatic(true);
    world.AddPhysicsBody(bar);
   
    //let's start making the bricks
    Texture redTex, orangeTex, yellTex, greenTex, blueTex, purpTex;
    LoadTex(redTex, "images/redBrick.png");
    PhysicsShapeList<PhysicsSprite> redBricks;
    for (int i(0); i < 11; i++) 
    {
        PhysicsSprite& rBrick = redBricks.Create();
        rBrick.setTexture(redTex);
        Vector2f sz = rBrick.getSize();
        rBrick.setCenter(Vector2f((((1000 / 10) * i)), 220)); 
        rBrick.setStatic(true);
        world.AddPhysicsBody(rBrick);
    }


    Clock clock;
    Time lastTime(clock.getElapsedTime());
 
    while ((lives > 0)) 
    {
        Time currentTime(clock.getElapsedTime());
        Time deltaTime(currentTime - lastTime);
        int deltaTimeMS(deltaTime.asMilliseconds());
        if (deltaTimeMS > 0)
        {
            world.UpdatePhysics(deltaTimeMS);
            lastTime = currentTime;
            MoveBar(bar, deltaTimeMS);
        }

        window.clear(Color(0, 0, 0)); 
        window.draw(bar);
        window.draw(ball);
        for (PhysicsShape& rBrick : redBricks) 
        {
            window.draw((PhysicsSprite&)rBrick); 
        }
        window.draw(floor);
        window.draw(LeftWall);
        window.draw(RightWall);
        window.draw(ceiling);
        
        //world.VisualizeAllBounds(window);
        window.display();
    }
    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
