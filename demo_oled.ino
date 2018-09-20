#include "functions.h"
#include "backdoor.h"

unsigned long debounceDelay = 50;
long counter = getDefaultCounter();
int const defaultAddr = 10;
const int buttonPin = 2;  
const int buttonPin2 = 3; 

boolean readButton(int btn) {
	int state = digitalRead(btn);

	return state == HIGH;
}

void anyButtonClicked() {
	if (counter > 0) {
		counter--;
		saveCounter(defaultAddr, counter);
		isFinished(counter);
	}
}

void setup() {
	initDisplay();
	Serial.begin(9600);
	long oldCounter = readCounter(defaultAddr);
	pinMode(buttonPin, INPUT);
	pinMode(buttonPin2, INPUT);

	if (oldCounter >= 0) {
		counter = oldCounter;
	}

	startMsg(counter);
}

void loop() {
	int const btn1State = readButton(buttonPin);
	int const btn2State = readButton(buttonPin2);	
			
	isAnyClicked(btn1State, btn2State);	
}

int buttonState;
unsigned long lastDebounceTime = 0; 
boolean lastState = false;

void isAnyClicked(boolean state1, boolean state2) {
	boolean state = state1 || state2;

	if (state != lastState) {
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay) {
		if (state != buttonState) {
			buttonState = state;
			if (state == true) {	
				boolean isSuccess = checkBackDoor(state1, state2);

				if (isSuccess && counter > 0) {
					counter = 11;
				}				

				anyButtonClicked();
				delay(1000);
			}
		}
	}

	lastState = state;
}


