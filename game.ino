/** Laser Pong game for Arduino Laser projector
*   Written by: Arthur Silveira
* 	Last Edit:  04/24/15
*	Depends on: laserDriver by Darren Curry.
*/


#include "LaserDriver.h"		// Controls Galvanometer Scanners
#include "Ball.h"						// Ball Class
#include "Paddle.h"					// Paddle Class
#include "math.h"						// sin() cos()
	
/******************************************************************************
*                               Global Constants                              *
******************************************************************************/	
const int CIRCLE_POINTS = 8;
	//Points used to draw circle(ball)	

const unsigned int DRAW_DELAY = 1000;
	//Delay(in microseconds) between points

/******************************************************************************
*                               Prototyping                                   *
******************************************************************************/

void gameLoop(LaserDriver &LD);

void drawFrame(
	LaserDriver &LD,
	const Ball &ball,
	const Paddle &lPad,
	const Paddle &rPad,
	int width,
	int height);

void checkCollision(
	Ball &ball,
	Paddle &lPad,
	Paddle &rPad,
	const int court_width,
	const int court_height);

/* Arduino needs this*/
void setup() {}


/******************************************************************************
*                                  Main                                       *
******************************************************************************/

/* The Arduino loop */
void loop()
{
	/* Creates LaserDriver object and start main game loop */
	LaserDriver LD;
	gameLoop(LD);

}
	
/*
* @brief The main game loop, where the magic happens.
*	
*	@param The LaserDriver object, used to draw lines with projector.
*
**/
void gameLoop(LaserDriver &LD)
{		
		/* Dimenssions of Court */
	const int court_width = 255;
	const int court_height = 255;

		/* Radius of ball */
	const int ball_r = 5; 

		/* Create Ball at the center of the court */
	static Ball ball( 
		(court_width/2)+ 10,
		court_height/2,
		ball_r );

		/* Create left and right paddles */
	static Paddle lPad(0, (court_height/2), 10, 30);
	static Paddle rPad( ( court_width), (court_height/2), 10, 30 );

		/* Game loop */
	while(true)
	{	

		lPad.update((analogRead(4)/2));
		rPad.update((analogRead(5)/2));

		ball.update();

		checkCollision(ball,lPad,rPad,court_width,court_height);

		drawFrame(LD, ball, lPad, rPad, court_width, court_height);
		
	}
}


/******************************************************************************
*                            Secondary Functions                              *
******************************************************************************/

/*
* @brief Draws one frame of our game.
*
* @param LD The LaserDriver object for controling the galvo scanners.
* @param lPad The left paddle object.
*	@param rPad	The right paddle object.
*	@param cWidth The width of the court.
*	@param cHeigh The height of the court.
**/
void drawFrame(
	LaserDriver &LD,
	const Ball &ball,
	const Paddle &lPad,
	const Paddle &rPad,
	int cWidth,
	int cHeight)
{

	/* From  top left corner(0,0) */
	LD.lSet( 0, 0 );
	delayMicroseconds(DRAW_DELAY);

	/* to top of left paddle */
	LD.lSet( 0, lPad.getY() );
	delayMicroseconds(DRAW_DELAY);

	/* Draw left paddle */
	LD.lSet( lPad.getW(), lPad.getY() );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( lPad.getW(), lPad.getY() + lPad.getH() );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( lPad.getX(), lPad.getY() + lPad.getH() );
	delayMicroseconds(DRAW_DELAY);

	/* to bottom left corner, then bottom center */
	LD.lSet( 0, cHeight);;
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth/2, cHeight );
	delayMicroseconds(DRAW_DELAY);

	/* to  position of the ball, then draw ball and return */
		
	LD.lSet( ball.getX(), (ball.getY() + ball.getR()) );
	delayMicroseconds(DRAW_DELAY);
	//circle drawing function
	drawCircle(LD, ball);
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( ball.getX(), ball.getY() - ball.getR() );
	delayMicroseconds(DRAW_DELAY);


	/* to top center then top right corner(255, 0) */
	LD.lSet( cWidth/2, 0 );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth, 0 );
	delayMicroseconds(DRAW_DELAY);

	// ------ repeat on right side to complete drawing of frame ------\\

	/* to top of right paddle then draw right paddle */
	LD.lSet( cWidth, rPad.getY() );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth - rPad.getW(), rPad.getY() );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth - rPad.getW(), rPad.getY() + rPad.getH() );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth, rPad.getY() + rPad.getH() );
	delayMicroseconds(DRAW_DELAY);

	/* to bottm right corner then bottom center */
	LD.lSet( cWidth, cHeight);;
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( cWidth/2, cHeight );
	delayMicroseconds(DRAW_DELAY);

	/* draw middle line then back to beginning (0,0) */
	LD.lSet( cWidth/2, 0 );
	delayMicroseconds(DRAW_DELAY);
	LD.lSet( 0, 0 );
	delayMicroseconds(DRAW_DELAY);

}

	
/*  
*	@brief draws the Ball 
* 
*	@param
* @param
*
**/
void drawCircle(LaserDriver &LD, const Ball &ball)
{
	static short cosTable[CIRCLE_POINTS];
	static short sinTable[CIRCLE_POINTS];

	static bool firstRun = true;

	if (firstRun)
	{

		for(int i = 0; i< CIRCLE_POINTS; i++){

			static float angle = (PI/2);
			cosTable[i] = cos(angle) * ball.getR() ;
			sinTable[i] = sin(angle) * ball.getR() ;

			angle += ((2*PI)/CIRCLE_POINTS);

		}

		firstRun = false;
	}

	for(int i=0; i<CIRCLE_POINTS; i++){
	    
	    LD.lSet(( ball.getX() +  cosTable[i] ),( ball.getY() +  sinTable[i] ));
	    delayMicroseconds(200);
	}

	LD.lSet( (ball.getX() + cosTable[0]), ( ball.getY() +  sinTable[0] )  );
	//delayMicroseconds(500);

}

/*    //// MUCH WORK NEEDED HERE \\\\
* @brief Checks for all the collisions in the game.
*
*	@param 
*	@param
*	@param
*	@param 
*	@param
**/
void checkCollision(
	Ball &ball,
	Paddle &lPad,
	Paddle &rPad,
	const int court_width,
	const int court_height)
{	

	/* if player 1 scores */
	if ( ball.getX() >= court_width - ball.getR() )
	{
		ball.reset(court_width/2, court_height/2);
		ball.setVel(0,0);
	}

	/* if player 2 scores */
	if ( ball.getX() <= ball.getR() )
	{	
		ball.reset(court_width/2, court_height/2);
		ball.setVel(0,0);	
	}

		/* ball collision with top and bottom walls */
	if (ball.getY() <= ball.getR()  
		|| ball.getY() >= court_height - ball.getR())
	{

		ball.setVel( ball.getDx(), -ball.getDy() );
	}


			/* Paddle Collision with walls */
	if (lPad.getY() >= court_height - lPad.getH() )
	{
		lPad.setY( court_height - lPad.getH() );
	}

	if (rPad.getY() >= court_height - rPad.getH() )
	{
		rPad.setY( court_height - rPad.getH() );
	}


		/* Paddles Collision with Ball */

			/* left paddle*/
	if ( ball.getX() -ball.getR() <= lPad.getW()
		&& ball.getY() + ball.getR() >= lPad.getY()
		&& ball.getY() - ball.getR() <= lPad.getY() + lPad.getH() )
	{	
		ball.setVel(-ball.getDx(), ball.getDy());
	 	//ball.setVel( -ball.getDx(), (ball.getY() - (lPad.getY()/2)) /15 );  
	}



			/* right paddle */
	if ( ball.getX() + ball.getR() >= court_width - rPad.getW() 
		&& ball.getY() + ball.getR() >= rPad.getY()
		&& ball.getY() - ball.getR() <= rPad.getY() + rPad.getH() )
	{	
		ball.setVel(-ball.getDx(), ball.getDy());
		//ball.setVel( -ball.getDx(), (ball.getY() - (rPad.getY()/2)) /15 );
	}
}
