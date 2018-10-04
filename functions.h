
#include <font6x8.h>
#include <nano_engine.h>
#include <nano_gfx.h>
#include <nano_gfx_types.h>
#include <sprite_pool.h>
#include <ssd1306.h>
#include <ssd1306_16bit.h>
#include <ssd1306_1bit.h>
#include <ssd1306_8bit.h>
#include <ssd1306_console.h>
#include <ssd1306_fonts.h>
#include <ssd1306_uart.h>

#include <Servo.h>
#include <EEPROM.h>
#include "ssd1306.h"
#include "backdoor.h"

int const defaultLng = 9;
long const defaultCounter = 1000000;
long counter = defaultCounter;

int const defaultAddr = 10;
int const backupAddr = defaultAddr + 4;

Servo servo;
int servoPosition = -1;

String formatCounter() {
	String str = String(counter);
	String formatted = "";
	int num = 0;

	for (int i = str.length(); i > 0; i--){
		if (num == 3) {
			formatted = "." + formatted;
			num = 0;
		}

		num++;
		formatted = str[i - 1] + formatted;
	}

	int rest = defaultLng - formatted.length();

	for (int i = rest; i > 0; i--) {
		formatted = " " + formatted;
	}

	return formatted;
}

void EEPROMWritelong(int address, long value) {
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}

long EEPROMReadlong(int address) {
	//Read the 4 bytes from the eeprom memory.
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	//Return the recomposed long by using bitshift.
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

static void showCounter() {
	String str = formatCounter();
	char cstr[16];
	str.toCharArray(cstr, 16);
	ssd1306_printFixedN(1, 32, cstr, STYLE_NORMAL, FONT_SIZE_2X);
}

void updateServo(int newPosition) {
	if(servoPosition != newPosition) {
		servoPosition = newPosition;
		Serial.println("update servo");
		Serial.println(servoPosition);
		servo.write(servoPosition); 
	}
}

void closeDoor() {
	Serial.println("close door");
	updateServo(0); 
}

void openDoor() {
	Serial.println("open door");
	updateServo(47);
}

void isFinished() {
	if (counter == 0) {
		ssd1306_clearScreen();
		ssd1306_printFixedN(6, 32, "CONSEGUIDO", STYLE_NORMAL, FONT_SIZE_2X);
		openDoor();
	} else if (counter > 0)	{
		showCounter();

		if(counter == defaultCounter) {
			closeDoor();
		}
	}
}

void startMsg() {
	ssd1306_clearScreen();
	ssd1306_printFixedN(6, 32, " WELCOME", STYLE_NORMAL, FONT_SIZE_2X);
	delay(3000);
	ssd1306_clearScreen();
	isFinished();
}

void retrieveCounter() {
	long value = EEPROMReadlong(defaultAddr);
	long backup = EEPROMReadlong(backupAddr);

	Serial.print("Read: ");
	Serial.println(value);

	if (value == backup && value > 0) {
		counter = value;
	} else if (value > 0) {
		counter = value;
	} else if (backup > 0) {
		counter = backup;
	}	
}

void saveCounter() {
	if (counter % 100 == 0) {
		Serial.print("Save: ");
		Serial.println(counter);
		EEPROMWritelong(defaultAddr, counter);
		EEPROMWritelong(backupAddr, counter);
	}
}

void initDisplay() {
	ssd1306_setFixedFont(ssd1306xled_font6x8);
	ssd1306_128x64_i2c_init();
	ssd1306_fillScreen(0x00);
}

void initServo() {
	servo.attach(5);
	closeDoor();
}

void downCounter() {
	if (counter > 0) {
		counter--;
		saveCounter();
		isFinished();
	}
}

void buttonClicked(boolean state1, boolean state2) {
	boolean isSuccess = checkBackDoor(state1, state2);

	if (isSuccess && counter > 0) {
		counter = 11;
	}				

	downCounter();
}
