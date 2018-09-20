
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

#include <EEPROM.h>
#include "ssd1306.h"

int const defaultLng = 9;
long const defaultCounter = 1000000;

long getDefaultCounter() {
	return defaultCounter;
}

String formatCounter(long countdowm) {
	String str = String(countdowm);
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

static void showCounter(long num) {
	String str = formatCounter(num);
	char cstr[16];
	str.toCharArray(cstr, 16);
	ssd1306_printFixedN(1, 32, cstr, STYLE_NORMAL, FONT_SIZE_2X);
}

void closeDoor() {
	Serial.println("close door");
}

void openDoor() {
	Serial.println("open door");
}

void isFinished(long num) {
	if (num == 0) {
		ssd1306_clearScreen();
		ssd1306_printFixedN(6, 32, "CONSEGUIDO", STYLE_NORMAL, FONT_SIZE_2X);
		openDoor();
	} else if (num > 0)	{
		showCounter(num);

		if(num == defaultCounter) {
			closeDoor();
		}
	}
}

int getAddrBackup(int addr) {
	return addr + 4;
}

long readCounter(int addr) {
	int addrBackup = getAddrBackup(addr);
	long num = -1;
	long value = EEPROMReadlong(addr);
	long backup = EEPROMReadlong(addrBackup);

	Serial.print("Read: ");
	Serial.println(value);

	if (value == backup && value > 0) {
		num = value;
	} else if (value > 0) {
		num = value;
	} else if (backup > 0) {
		num = backup;
	}

	return num;
}

void saveCounter(int addr, long num) {
	if (num % 100 == 0) {
		Serial.print("Save: ");
		Serial.println(num);
		int addrBackup = getAddrBackup(addr);
		EEPROMWritelong(addr, num);
		EEPROMWritelong(addrBackup, num);
	}
}

void startMsg(long num) {
	ssd1306_clearScreen();
	ssd1306_printFixedN(6, 32, " WELCOME", STYLE_NORMAL, FONT_SIZE_2X);
	delay(3000);
	ssd1306_clearScreen();
	isFinished(num);
}

void initDisplay() {
	ssd1306_setFixedFont(ssd1306xled_font6x8);
	ssd1306_128x64_i2c_init();
	ssd1306_fillScreen(0x00);
}