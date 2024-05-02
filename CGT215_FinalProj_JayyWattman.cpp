// CGT215_FinalProj_JayyWattman.cpp : This file contains the 'main' function. Brick Breaker game with music.
//  Bar should be moveable with right and left arrow keys, bricks should change color after 1 hit and disappear after 2
// coded by Jordyn "Jayy" Wattman on 04/15/2024 Updated 05/02/2024
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <map> // need this so I can map the hit counts for each brick in each physicsShapeList
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

//keyboard speed constant, affects how much power each key press imparts on the cbow
const float KB_Speed = 0.7; // magic number, might need to adjust

//function for loading textures that can be referenced later
void LoadTex(Texture& tex, string filename)
{
    if (!tex.loadFromFile(filename))
    {
        cout << "Could not load " << filename << endl;
        exit(1);
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
    int lives(3); //start at 5 so it can decrement down

    map<PhysicsShape*, int> hitCounts; //map to keep track of hits to each brick

    //get some music going
    Music music;
    if (!music.openFromFile("loop1.ogg")) {
        cout << "failed to load loop music";
        exit(6);
    }
    music.play();

    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "Could not load font" << endl;
        exit(0);
    } //font exit incase it didn't transfer

    //make our walls, ceiling, and floor so the ball doesn't go flying off screen
    PhysicsRectangle floor; 
    floor.setSize(Vector2f(1000, 10)); 
    floor.setCenter(Vector2f(500, 795)); 
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
    ball.setCenter(Vector2f(500, 715)); //set it to start here so hopefully there's time for player to react to the drop
    ball.setRadius(15);
    //ball.setStatic(true);
    world.AddPhysicsBody(ball);
    
    //add some velo to get the direction going
    ball.applyImpulse(Vector2f(-0.04, 1.1));

    //make the bounce bar
    PhysicsSprite& bar = *new PhysicsSprite(); 
    Texture barTex;
    LoadTex(barTex, "images/bar2.png");
    bar.setTexture(barTex);
    Vector2f sz = bar.getSize();
    bar.setCenter(Vector2f(500, 770-(sz.y/2)));
    bar.setStatic(true);
    world.AddPhysicsBody(bar);

    floor.onCollision = [&lives, &ball,&bar, &sz](PhysicsBodyCollisionResult result)
        {
            if (result.object2 == ball)
            {
                lives--;
                bar.setCenter(Vector2f(500, 770 - (sz.y / 2)));
                ball.setCenter(Vector2f(500, 715)); 
                //ball.applyImpulse(Vector2f(-0.025, 1));

            }
        };


   
    //let's start making the bricks
    Texture redTex, orangeTex, yellTex, greenTex, blueTex, purpTex, hitTex;
    LoadTex(hitTex, "images/hit1.png");
    LoadTex(redTex, "images/redBrick.png");
    PhysicsShapeList<PhysicsSprite> redBricks;
    for (int i(0); i < 11; i++) 
    {
        PhysicsSprite& rBrick = redBricks.Create();
        hitCounts[&rBrick] = 0; //hit counts with reference to red bricks
        rBrick.setTexture(redTex);
        Vector2f sz = rBrick.getSize();
        rBrick.setCenter(Vector2f((((1000 / 10) * i)), 280)); 
        rBrick.setStatic(true); //don't want em moving
        world.AddPhysicsBody(rBrick);
  
        rBrick.onCollision = [&ball, &world, &rBrick, &redBricks, &score, &hitTex, &i, &redTex,&hitCounts](PhysicsBodyCollisionResult result)
            {
                
                    if (result.object2 == ball)//if the ball hits rBrick
                    {
                        ++hitCounts[(PhysicsShape*)&result.object1]; //hit count for whichever red brick was hit goes up by one
                        rBrick.setTexture(hitTex); //change the brick color so the player knows it has been hit
                        
                    }

                    if (hitCounts[(PhysicsShape*)&result.object1] == 2) //if hit count for particular brick equals 2
                    {
                        world.RemovePhysicsBody(rBrick);
                        redBricks.QueueRemove(rBrick);
                        score += 50;
                      
                    }
                
            };
    }

    LoadTex(orangeTex, "images/orangeBrick.png");
    PhysicsShapeList<PhysicsSprite> orangeBricks;
    for (int i(0); i < 11; i++)
    {
        PhysicsSprite& oBrick = orangeBricks.Create();
        hitCounts[&oBrick] = 0;
        oBrick.setTexture(orangeTex);
        Vector2f sz = oBrick.getSize();
        oBrick.setCenter(Vector2f((((1000 / 10) * i)), 245));
        oBrick.setStatic(true);
        world.AddPhysicsBody(oBrick);
        oBrick.onCollision = [&ball, &world, &oBrick, &orangeBricks, &score, &hitTex,&hitCounts](PhysicsBodyCollisionResult result)
            {
                if (result.object2 == ball)
                {
                    ++hitCounts[(PhysicsShape*)&result.object1];
                    oBrick.setTexture(hitTex);

                }
                if (hitCounts[(PhysicsShape*)&result.object1] == 2)
                {
                    world.RemovePhysicsBody(oBrick);
                    orangeBricks.QueueRemove(oBrick);
                    score += 50;

                }
            };
    }

    LoadTex(yellTex, "images/yellowBrick.png");
    PhysicsShapeList<PhysicsSprite> yellowBricks;
    for (int i(0); i < 11; i++)
    {
        PhysicsSprite& yBrick = yellowBricks.Create();
        hitCounts[&yBrick] = 0;
        yBrick.setTexture(yellTex);
        Vector2f sz = yBrick.getSize();
        yBrick.setCenter(Vector2f((((1000 / 10) * i)), 210));
        yBrick.setStatic(true);
        world.AddPhysicsBody(yBrick);
        yBrick.onCollision = [&ball, &world, &yBrick, &yellowBricks, &score, &hitTex, &hitCounts](PhysicsBodyCollisionResult result)
            {
                if (result.object2 == ball)
                {
                    ++hitCounts[(PhysicsShape*)&result.object1];
                    yBrick.setTexture(hitTex);

                }
                if (hitCounts[(PhysicsShape*)&result.object1] == 2)
                {
                    world.RemovePhysicsBody(yBrick);
                    yellowBricks.QueueRemove(yBrick);
                    score += 50;

                }
            };
    }

    LoadTex(greenTex, "images/greenBrick.png");
    PhysicsShapeList<PhysicsSprite> greenBricks;
    for (int i(0); i < 11; i++)
    {
        PhysicsSprite& gBrick = greenBricks.Create();
        hitCounts[&gBrick] = 0;
        gBrick.setTexture(greenTex);
        Vector2f sz = gBrick.getSize();
        gBrick.setCenter(Vector2f((((1000 / 10) * i)), 175));
        gBrick.setStatic(true);
        world.AddPhysicsBody(gBrick);
        gBrick.onCollision = [&ball, &world, &gBrick, &greenBricks, &score, &hitTex, &hitCounts](PhysicsBodyCollisionResult result)
            {
                if (result.object2 == ball)
                {
                    ++hitCounts[(PhysicsShape*)&result.object1];
                    gBrick.setTexture(hitTex);

                }
                if (hitCounts[(PhysicsShape*)&result.object1] == 2)
                {
                    world.RemovePhysicsBody(gBrick);
                    greenBricks.QueueRemove(gBrick);
                    score += 50;
                }
            };
    }

    LoadTex(blueTex, "images/blueBrick.png");
    PhysicsShapeList<PhysicsSprite> blueBricks;
    for (int i(0); i < 11; i++)
    {
        PhysicsSprite& bBrick = blueBricks.Create();
        hitCounts[&bBrick] = 0;
        bBrick.setTexture(blueTex);
        Vector2f sz = bBrick.getSize();
        bBrick.setCenter(Vector2f((((1000 / 10) * i)), 140));
        bBrick.setStatic(true);
        world.AddPhysicsBody(bBrick);
        bBrick.onCollision = [&ball, &world, &bBrick, &blueBricks, &score, &hitTex, &hitCounts](PhysicsBodyCollisionResult result)
            {
                if (result.object2 == ball)
                {
                    ++hitCounts[(PhysicsShape*)&result.object1];
                    bBrick.setTexture(hitTex);

                }
                if (hitCounts[(PhysicsShape*)&result.object1] == 2)
                {
                    world.RemovePhysicsBody(bBrick);
                    blueBricks.QueueRemove(bBrick);
                    score += 50;
                }
            };
    }

    LoadTex(purpTex, "images/purpBrick.png");
    PhysicsShapeList<PhysicsSprite> purpBricks;
    for (int i(0); i < 11; i++)
    {
        PhysicsSprite& pBrick = purpBricks.Create();
        hitCounts[&pBrick] = 0;
        pBrick.setTexture(purpTex);
        Vector2f sz = pBrick.getSize();
        pBrick.setCenter(Vector2f((((1000 / 10) * i)), 105)); 
        pBrick.setStatic(true);
        world.AddPhysicsBody(pBrick);
        pBrick.onCollision = [&ball, &world, &pBrick, &purpBricks, &score, &hitTex, &hitCounts](PhysicsBodyCollisionResult result)
            {
                if (result.object2 == ball)
                {
                    ++hitCounts[(PhysicsShape* )&result.object1];
                    pBrick.setTexture(hitTex);

                }
                if (hitCounts[(PhysicsShape*)&result.object1] == 2)
                {
                    world.RemovePhysicsBody(pBrick);
                    purpBricks.QueueRemove(pBrick);
                    score += 50;
                }
            };
    }

    Clock clock;
    Time lastTime(clock.getElapsedTime());
 
    while ((lives > 0)) 
    {
        Time currentTime(clock.getElapsedTime());
        Time deltaTime(currentTime - lastTime);
        int deltaTimeMS(deltaTime.asMilliseconds());
        if (deltaTimeMS > 0.1)
        {
            world.UpdatePhysics(deltaTimeMS);
            lastTime = currentTime;
            MoveBar(bar, deltaTimeMS);
        }

        window.clear(Color(0, 0, 0)); 
        window.draw(bar);
        window.draw(ball);
        redBricks.DoRemovals();
        for (PhysicsShape& rBrick : redBricks) 
        {
            window.draw((PhysicsSprite&)rBrick); 
        }
        orangeBricks.DoRemovals();
        for (PhysicsShape& oBrick : orangeBricks)
        {
            window.draw((PhysicsSprite&)oBrick);
        }
        yellowBricks.DoRemovals();
        for (PhysicsShape& yBrick : yellowBricks)
        {
            window.draw((PhysicsSprite&)yBrick);
        }
        greenBricks.DoRemovals();
        for (PhysicsShape& gBrick : greenBricks)
        {
            window.draw((PhysicsSprite&)gBrick);
        }
        blueBricks.DoRemovals();
        for (PhysicsShape& bBrick : blueBricks)
        {
            window.draw((PhysicsSprite&)bBrick);
        }
        purpBricks.DoRemovals(); 
        for (PhysicsShape& pBrick : purpBricks)
        {
            window.draw((PhysicsSprite&)pBrick);
        }
        Text scoreText;
        scoreText.setString("Score: " + (to_string(score)));
        scoreText.setFont(font);
        FloatRect scoreSz(scoreText.getGlobalBounds());
        scoreText.setPosition(Vector2f(920 - (scoreSz.width), 760 - (scoreSz.height)));
        window.draw(scoreText);

        Text livesText;
        livesText.setString("Lives: " + (to_string(lives)));
        livesText.setFont(font);
        FloatRect livesSz(livesText.getGlobalBounds()); 
        livesText.setPosition(Vector2f(30, 760 - (livesSz.height)));
        window.draw(livesText);

        window.draw(floor);
        //window.draw(LeftWall);
        //window.draw(RightWall);
        window.draw(ceiling);
        
        //world.VisualizeAllBounds(window);
        window.display();
    }
    bool exitGame(false); 
    PhysicsRectangle backboard; 
    backboard.setSize(Vector2f(600, 400)); 
    backboard.setCenter(Vector2f(500, 400)); 
    backboard.setFillColor(Color(18, 197, 34)); // green
    backboard.setStatic(true); 

    PhysicsRectangle frontboard;
    frontboard.setSize(Vector2f(560, 360));
    frontboard.setCenter(Vector2f(500, 400));
    frontboard.setFillColor(Color(0, 0, 0)); //black
    frontboard.setStatic(true);

    Text gameOverT; 
    gameOverT.setFont(font); 
    gameOverT.setString("Game Over");
    gameOverT.setFillColor(Color(225, 240, 245)); 
    FloatRect goSz = gameOverT.getGlobalBounds();
    gameOverT.setPosition(Vector2f(500 - (goSz.width / 2), 266 - (goSz.height)));

    Text finalScore;
    finalScore.setFont(font);
    finalScore.setString("You scored: " + to_string(score) + " points!");
    finalScore.setFillColor(Color(225, 240, 245));
    FloatRect FinScoreSz = finalScore.getGlobalBounds();
    finalScore.setPosition(Vector2f(500 - (FinScoreSz.width / 2), 400 - (FinScoreSz.height)));

    Text leaveText;
    leaveText.setFont(font);
    leaveText.setString("Press SPACE to exit game");
    leaveText.setFillColor(Color(225, 240, 245));
    FloatRect ltSz = leaveText.getGlobalBounds();
    leaveText.setPosition(Vector2f(500 - (ltSz.width / 2), 500 - (ltSz.height)));

    while (!exitGame)
    {
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            exitGame = true;
        }
        window.clear();
        window.draw(backboard);
        window.draw(frontboard);
        window.draw(gameOverT);
        window.draw(finalScore);
        window.draw(leaveText);
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
