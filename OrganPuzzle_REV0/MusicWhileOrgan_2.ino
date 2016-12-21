#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>


//int d;
int nextKey = 0;
int nextPin = 0;
int step = 0;

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


const long interval = 10000;           // interval at which to blink (milliseconds)
const long dotInterval = 1000;


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
		if (elapsedMillis >= dotInterval)
		{
			Serial.print("  .");
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
  player.play();
	Serial.println("You won it! You get a nice message via the YouWon Function! youWonIt is true now!");
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

			if ((millis() - lastDebounceTime) > debounceDelay)
				// whatever the reading is at, it's been there for longer
				// than the debounce delay, so take it as the actual current state:
			{

				// if the state change was a press (as opposed to release):
				if (pbState[thisPin] == HIGH ) //> lastState[thisPin]
				{
					lastKeyPress = thisPin + 1;
					
				}
			}
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
