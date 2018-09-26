#include "functions.h"

const int btnPin1 = 2;  
const int btnPin2 = 3; 

int buttonState;
unsigned long debounceDelay = 50;
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
				buttonClicked(state1, state2);
			}
		}
	}

	lastState = state;
}

boolean readButton(int btn) {
	int state = digitalRead(btn);

	return state == HIGH;
}

void setup() {
	Serial.begin(9600);
	initServo();
	initDisplay();
	pinMode(btnPin1, INPUT);
	pinMode(btnPin2, INPUT);
	retrieveCounter();
	startMsg();
}

void loop() {
	int const btn1State = readButton(btnPin1);
	int const btn2State = readButton(btnPin2);	
			
	isAnyClicked(btn1State, btn2State);	
}