#include <SoftReset.h>
#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>


//int d;
int nextKey = 0;
int nextPin = 0;
int step = 0;

static int solvedOutput = 9;
static int pinCount = 6;
static int pbPins[] = { 2, 3, 4, 5, 6, 7 };
int pbState[] = { 0, 0, 0, 0, 0, 0 };
int lastState[] = { 0, 0, 0, 0, 0, 0 };
int lastKeyPress = 0;



bool shownPrompt = false;
bool youWonIt = false;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


int solutionBuffer[] = { 5, 4, 3, 2, 1, 2 };
const int sizeOfSolution = 6;
//char* solutionNotes[] = { "G", "F", "E", "D", "C#", "D" };
char* notes[] = { "C#", "D", "E", "F", "G", "A" };

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = 0;
unsigned long elapsedMillis = 0;

unsigned long previousDotMillis = 0;
unsigned long elapsedDotMillis = 0;

const long interval = 4000;           // Allowed interval between keypresses (milliseconds)
const long dotInterval = 1000;



/*
AUTO-RESET after music finished playing
*/

//static int resetDelaySeconds = 10;
static unsigned long resetDelayMillis = 442000; // running length is 530 sec; 540 000 milli delay ends up being 2 minutes (1.2bar times) too long. try 442 sec
//static unsigned long resetDelayMillis = 10000;

//This timer also overcomes the 50 day millis() limit problem
//this setup will allow 10 timers, 0-9, 
//replace "10" with a higher or lower number if you need more or less timers
//the next two lines must be added to any program using this code
unsigned long autotimer[10];
byte timerState[10];

void setup()
{
	Serial.begin(115200);

  player.keyDisable();
  player.begin();
  player.setPlayMode(PM_NORMAL_PLAY);
  //player.playOne("LongRoad.mp3");          //play a song with its name
  player.scanAndPlayAll();

	// Initialize pb pins as INPUTs
	for (int thisPin = 0; thisPin < pinCount; thisPin++)
	{
		pinMode(pbPins[thisPin], INPUT);
	} //Init pb pins as Input

	// Initialize solution output pin as OUTPUT
	pinMode(solvedOutput, OUTPUT);
	digitalWrite(solvedOutput, LOW);

	//player.maxVolume();
	player.setVolume(0); //number is -dB => lower number is louder.
	

}

void loop()
{
	while (youWonIt == false)
	{
		nextKey = getNextKey(step);
		if (shownPrompt == false)
		{
			Serial.print("Press key #");
			Serial.print(nextKey);
			Serial.print(" - ");
			Serial.println(getNote(nextKey));
			Serial.println();

			shownPrompt = true;
		}


		if (step != 0)
		{
			if (step < sizeOfSolution + 1)
			{
				timer();
				checkKey();
			}
			else
			{
				Serial.println("Hooray! You solved it! So clever!");
			}

		}
		else
		{
			checkKey();
		}
	}
	
	//Serial.println("Ok, go ahead and reset it now.");
}

void timer()
{
	currentMillis = millis();
	elapsedMillis = currentMillis - previousMillis;

	elapsedDotMillis = currentMillis - previousDotMillis;

	// IF time is up
	if (elapsedMillis >= interval)
	{
		// save the last time you blinked the LED
		//previousMillis = currentMillis;
		resetTimer();

		Serial.println("too slow. back to the start for you.");
		step = 0;
		lastKeyPress = 0;
		shownPrompt = false;
	}
	else
	{
		if (elapsedDotMillis >= dotInterval)
		{
			Serial.print("  .");
			previousDotMillis = currentMillis;
		}
	}
}

void resetTimer()
{
	// save the last time you blinked the LED
	currentMillis = millis();
	previousMillis = currentMillis;
}


int getNextKey(int thisStep)
{
	int nextKey;
	if (thisStep < 6)
	{
		nextKey = solutionBuffer[thisStep];
	}
	else
	{
		//nextKey = 0;
		youWon();
	}
	return nextKey;
}

void youWon()
{
	youWonIt = true;
	//player.maxVolume();
	//delay(50);
	player.play();
	digitalWrite(solvedOutput, HIGH);
	Serial.println("You won it! You get a nice message via the YouWon Function! youWonIt is true now!");
	
	
	//delay(resetDelayMillis);
	//if (delaySeconds(1, resetDelaySeconds)==1)
	//{

	delay(resetDelayMillis);
		Serial.println("Resetting now!");
		digitalWrite(solvedOutput, LOW);
		delay(100);
		digitalWrite(solvedOutput, HIGH);
		delay(100);
		digitalWrite(solvedOutput, LOW);
		delay(100);
		digitalWrite(solvedOutput, HIGH);
		delay(100);
		digitalWrite(solvedOutput, LOW);
		delay(100);
		digitalWrite(solvedOutput, HIGH);
		delay(100);
		digitalWrite(solvedOutput, LOW);
		delay(100);
		digitalWrite(solvedOutput, HIGH);
		delay(100);
		
		soft_restart();
	//}
}

char* getNote(int keyNum)
{
	char* noteResult;
	noteResult = notes[keyNum];
	return noteResult;
}


void readKeys()
{
	// For each of the six pushbuttons:  
	for (int thisPin = 0; thisPin < pinCount; thisPin++)
	{
		//lastState[thisPin] = pbState[thisPin];
		pbState[thisPin] = digitalRead(pbPins[thisPin]);

		// if the button state has changed (due to noise or pressing):
		if (pbState[thisPin] != lastState[thisPin])
		{
			lastDebounceTime = millis(); //reset the debounce timer

			//if ((millis() - lastDebounceTime) > debounceDelay)
				// whatever the reading is at, it's been there for longer
				// than the debounce delay, so take it as the actual current state:
			//{

				// if the state change was a press (as opposed to release):
				if (pbState[thisPin] == HIGH ) //> lastState[thisPin]
				{
					lastKeyPress = thisPin + 1;
					
				}
			//}
		}
		lastState[thisPin] = pbState[thisPin];
	}

	

}

void checkKey()
{
	readKeys();
	if (lastKeyPress != 0)
	{
		Serial.print("You Pressed Key #");
		Serial.print(lastKeyPress);
		Serial.print(" - ");
		Serial.println(getNote(lastKeyPress));
		Serial.println();

		if (lastKeyPress == nextKey)
		{
			Serial.println("[ Correct! ]");
			Serial.println("--------------------");
			Serial.println();
			step++;
			lastKeyPress = 0;
		}
		else
		{
			Serial.println("[ Incorrect! ]");
			Serial.println("--------------------");
			Serial.println();
			step = 0;
			lastKeyPress = 0;

		}
		shownPrompt = false;
		resetTimer();
	}
}



//int delayHours(byte timerNumber, unsigned long delaytimeH) {    //Here we make it easy to set a delay in Hours
//	delayMilliSeconds(timerNumber, delaytimeH * 1000 * 60 * 60);
//}
//int delayMinutes(byte timerNumber, unsigned long delaytimeM) {    //Here we make it easy to set a delay in Minutes
//	delayMilliSeconds(timerNumber, delaytimeM * 1000 * 60);
//}
//int delaySeconds(byte timerNumber, unsigned long delaytimeS) {    //Here we make it easy to set a delay in Seconds
//	delayMilliSeconds(timerNumber, delaytimeS * 1000);
//}
//
//int delayMilliSeconds(int timerNumber, unsigned long delaytime) {
//	unsigned long timeTaken;
//	if (timerState[timerNumber] == 0) {    //If the timer has been reset (which means timer (state ==0) then save millis() to the same number timer, 
//		autotimer[timerNumber] = millis();
//		timerState[timerNumber] = 1;      //now we want mark this timer "not reset" so that next time through it doesn't get changed.
//	}
//	if (millis()> autotimer[timerNumber]) {
//		timeTaken = millis() + 1 - autotimer[timerNumber];    //here we see how much time has passed
//	}
//	else {
//		timeTaken = millis() + 2 + (4294967295 - autotimer[timerNumber]);    //if the timer rolled over (more than 48 days passed)then this line accounts for that
//	}
//	if (timeTaken >= delaytime) {          //here we make it easy to wrap the code we want to time in an "IF" statement, if not then it isn't and so doesn't get run.
//		timerState[timerNumber] = 0;  //once enough time has passed the timer is marked reset.
//		return 1;                          //if enough time has passed the "IF" statement is true
//	}
//	else {                               //if enough time has not passed then the "if" statement will not be true.
//		return 0;
//	}
//}