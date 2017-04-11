/*
 * DemoOpenBionicsOET.ino
 *
 * Created: 10/05/2017 13:30:00
 * Authors:
 *          PENA MORALES Carolina
 *          BOUILLON Fanny
 *          GLEDEL Mathieu
 *
 * This work is licensed under the Creative Commons Attribution 4.0 International License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/.
 *
 */

#include <FingerLib.h>

// number of controllable fingers (number of motors)
#define NUM_FINGERS 5

// Uncomment one of the following to select which hand is used
#define HANDFLAG LEFT
//#define HANDFLAG RIGHT

// Pins and leds configuration
#define LED_1     16
#define LED_2     24
#define SWITCH_1  21

// machine states
#define STATE_WAITING             0
#define STATE_MOV                 1
#define STATE_DISABLE_MOTORS      2

// hand positions
#define POSITION_ONE_BY_ONE 0
#define POSITION_OK         1
#define POSITION_VICTORY    2
#define POSITION_OPEN       3
#define MAX_POSITION    POSITION_OPEN

//state en cours
int state = STATE_WAITING;
int movCounter = 0;

// Variables for time
unsigned long timeBefore;
unsigned long timeAfter;

// Finger array
Finger finger[NUM_FINGERS];

void setup()
{
	//vitesse de la console
	MYSERIAL.begin(9600);

	// Hand pins configuration for motors
	pinAssignment();

	//open hand
	openHand();

	// attach interruption to a pin, in this case pin 21
	attachInterrupt(digitalPinToInterrupt(SWITCH_1), interruption, HIGH);

	state = STATE_WAITING;
}

void loop()
{
	switch (state) {
	//this state does noting, it just waits for the interruption
	case STATE_WAITING:
		break;
	case STATE_MOV:
		// motor activation
		for (int i=0;i<NUM_FINGERS;i++)
			finger[i].enableMotor();
		// movCounter gives the actual position of the hand
		if(movCounter == POSITION_VICTORY)
			positionVictory();
		else if (movCounter == POSITION_OK)
			positionOK();
		else if (movCounter == POSITION_ONE_BY_ONE)
			positionOneByOne();
		else if (movCounter == POSITION_OPEN)
			openHand();
		// next state
		state = STATE_DISABLE_MOTORS;
		break;
	case STATE_DISABLE_MOTORS:
		//disable motors
		for (int i=0;i<NUM_FINGERS;i++)
			finger[i].disableMotor();
		// next state
		state = STATE_WAITING;
		break;
	default:
		//security state
		state = STATE_WAITING;
		break;
	}
}

// attach the pins to the finger instances
void pinAssignment(void)
{
#if HANDFLAG==RIGHT
		finger[0].attach(13,4,A5);       // Right motor connector
		finger[1].attach(3,6,A1);
		finger[2].attach(7,8,A2);
		finger[3].attach(10,9,A3);
		finger[4].attach(11,12,A4);
#else
		finger[0].attach(5,2,A0);       // Left motor connector
		finger[1].attach(11,12,A4);
		finger[2].attach(10,9,A3);
		finger[3].attach(7,8,A2);
		finger[4].attach(3,6,A1);
#endif
	pinMode(SWITCH_1, INPUT_PULLUP);
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
}

//gives the time between two  motor movments for the position one by one
void timeAnimation(unsigned long time)
{
	timeBefore = millis();
	do{
		timeAfter = millis();
	} while((unsigned long) timeAfter - timeBefore < time);
}

//function to open the hand
void openHand(void)
{
	finger[0].open();
	finger[1].open();
	finger[2].open();
	finger[3].open();
	finger[4].open();
	timeAnimation(1000);
}

// position in which only the index and the middle fingers are opened, the others are closed
void positionVictory(void)
{
	finger[0].close();
	finger[1].open();
	finger[2].open();
	finger[3].close();
	finger[4].close();
	timeAnimation(1000);
}

//index and thumb are closed the others are opened
void positionOK(void)
{
	finger[0].close();
	finger[1].close();
	finger[2].open();
	finger[3].open();
	finger[4].open();
	timeAnimation(1000);
}

// Open all the fingers one by one, there is a little time before each movement given by the function timeAnimation
void positionOneByOne(void)
{
	finger[4].close();
	timeAnimation(150);
	finger[3].close();
	timeAnimation(150);
	finger[2].close();
	timeAnimation(150);
	finger[1].close();
	timeAnimation(150);
	finger[0].close();
	timeAnimation(1000);
}

//Interruption : each time an interruption is produced, the state changes to STATE_MOV and
//movCounter++ to change the hand position
void interruption(void){
	// debounce method
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 180)
	{
		// For debug purpose
		//MYSERIAL.println("interruption");
		state = STATE_MOV;
		movCounter++;
		if(movCounter > MAX_POSITION){
			movCounter = 0;
		}
	}
	last_interrupt_time = interrupt_time;
}

