#ifndef KB_H
#define KB_H

#include <stdint.h>

uint8_t kbdus[128] = {
	
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'-', '=', '\b',
	'\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'\n', '$',	//ctrl
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 'L',	//left shift
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'R', //right shift
	'*',
	'A',	// alt
	' ',
	'C',	//CAPS LOCK
	'D',	// F1 to F10
	'E', 'F', 'G', 'H', 'I', 'J', 'K', '?',
	'M',
	'N',	// num lock
	'S',	// scroll lock
	'T',	// home key
	'U',	// Up arrow
	'!',	// page up
	'-',
	'Z',	// left arrow
	0,	
	'V',	// right arrow
	'+',
	'"',	// end key
	'X',	// down arrow
	'@',	// page down
	'#',	// insert key
	'%',	// delete key
	0, 0, 0,
	'^',	// f11
	'&',	// f12
	0
};

uint8_t shiftLookup[128];

extern uint32_t isAltPressed();
extern uint32_t isCtrlPressed();
extern uint32_t isLeftShiftPressed();
extern uint32_t isRightShiftPressed();
extern uint32_t isCapsOn();
extern uint32_t isF1Pressed();
extern uint32_t isF2Pressed();
extern uint32_t isF3Pressed();
extern uint32_t isF4Pressed();
extern uint32_t isF5Pressed();
extern uint32_t isF6Pressed();
extern uint32_t isF7Pressed();
extern uint32_t isF8Pressed();
extern uint32_t isF9Pressed();
extern uint32_t isF10Pressed();
extern uint32_t isNumLockPressed();
extern uint32_t isScrollLockPressed();
extern uint32_t isHomeKeyPressed();
extern uint32_t isUpArrowPressed();
extern uint32_t isPageUpPressed();
extern uint32_t isLeftArrowPressed();
extern uint32_t isRightArrowPressed();
extern uint32_t isEndKeyPressed();
extern uint32_t isDownArrowPressed();
extern uint32_t isPageDownPressed();
extern uint32_t isInsertKeyPressed();
extern uint32_t isDeleteKeyPressed();
extern uint32_t isF11Pressed();
extern uint32_t isF12Pressed();

#endif
