#include "kb.h"
#include "screen.h"
#include "system.h"
#include "screen_control.h"

#include <stdint.h>

void initShiftLookup();
void switchCapsLightOff();
void switchCapsLightOn();
void sendByte(uint8_t port, uint8_t data);
uint8_t capsOn = 0;

uint32_t keyStatus = 0;
/* What its bits signify:-
	0x1 			ctrl pressed
	0x2 			Left shift pressed
	0x4			Right Shift pressed
	0x8			Alt pressed
	0x10			F1 pressed
	0x20			F2 pressed
	0x40			F3 pressed
	0x80			F4 pressed
	0x100			F5 pressed
	0x200			F6 pressed
	0x400			F7 pressed
	0x800			F8 pressed
	0x1000			F9 pressed
	0x2000			F10 pressed
	0x4000			Num Lock pressed
	0x8000			Scroll Lock pressed
	0x10000			Home Key pressed
	0x20000			Up Arrow pressed
	0x40000			Page Up pressed
	0x80000			Left Arrow pressed
	0x100000		Right Arrow pressed
	0x200000		End Key pressed
	0x400000		Down Arrow pressed
	0x800000		Page Down pressed
	0x1000000		Insert Key pressed
	0x2000000		Delete Key pressed
	0x4000000		F11 pressed
	0x8000000		F12 pressed
	0x10000000		Caps Lock pressed
*/

void keyboard_handler(struct regs *r) {
	uint8_t scancode;
	
	scancode = inb(0x60);
	
	if(scancode & 0x80) {
		scancode &= 0x7F;
		switch(kbdus[scancode]) {
		case '$':
			keyStatus &= 0xfffffffE;
			break;
		case 'L':
			keyStatus &= 0xfffffffD;
			break;
		case 'R':
			keyStatus &= 0xfffffffB;
			break;
		case 'A':
			keyStatus &= 0xfffffff7;
			break;
		case 'D':
			keyStatus &= 0xffffffEf;
			break;
		case 'E':
			keyStatus &= 0xffffffDf;
			break;
		case 'F':
			keyStatus &= 0xffffffBf;
			break;
		case 'G':
			keyStatus &= 0xffffff7f;
			break;
		case 'H':
			keyStatus &= 0xfffffEff;
			break;
		case 'I':
			keyStatus &= 0xfffffDff;
			break;
		case 'J':
			keyStatus &= 0xfffffBff;
			break;
		case 'K':
			keyStatus &= 0xfffff7ff;
			break;
		case '?':
			keyStatus &= 0xffffEfff;
			break;
		case 'M':
			keyStatus &= 0xffffDfff;
			break;
		case 'N':
			keyStatus &= 0xffffBfff;
			break;
		case 'S':
			keyStatus &= 0xffff7fff;
			break;
		case 'T':
			keyStatus &= 0xfffEffff;
			break;
		case 'U':
			keyStatus &= 0xfffDffff;
			break;
		case '!':
			keyStatus &= 0xfffBffff;
			break;
		case 'Z':
			keyStatus &= 0xfff7ffff;
			break;
		case 'V':
			keyStatus &= 0xffEfffff;
			break;
		case '"':
			keyStatus &= 0xffDfffff;
			break;
		case 'X':
			keyStatus &= 0xffBfffff;
			break;
		case '@':
			keyStatus &= 0xff7fffff;
			break;
		case '#':
			keyStatus &= 0xfEffffff;
			break;
		case '%':
			keyStatus &= 0xfDffffff;
			break;
		case '^':
			keyStatus &= 0xfBffffff;
			break;
		case '&':
			keyStatus &= 0xf7ffffff;
			break;
		}
	}
	else {
		uint8_t ch;
		switch(kbdus[scancode]) {
		case '$':
			keyStatus |= 0x00000001;
			break;
		case 'L':
			keyStatus |= 0x00000002;
			break;
		case 'R':
			keyStatus |= 0x00000004;
			break;
		case 'A':
			keyStatus |= 0x00000008;
			break;
		case 'D':
			keyStatus |= 0x00000010;
			break;
		case 'E':
			keyStatus |= 0x00000020;
			break;
		case 'F':
			keyStatus |= 0x00000040;
			break;
		case 'G':
			keyStatus |= 0x00000080;
			break;
		case 'H':
			keyStatus |= 0x00000100;
			break;
		case 'I':
			keyStatus |= 0x00000200;
			break;
		case 'J':
			keyStatus |= 0x00000400;
			break;
		case 'K':
			keyStatus |= 0x00000800;
			break;
		case '?':
			keyStatus |= 0x00001000;
			break;
		case 'M':
			keyStatus |= 0x00002000;
			break;
		case 'N':
			keyStatus |= 0x00004000;
			break;
		case 'S':
			keyStatus |= 0x00008000;
			break;
		case 'T':
			keyStatus |= 0x00010000;
			break;
		case 'U':
			keyStatus |= 0x00020000;
			moveCursorUp();
			break;
		case '!':
			keyStatus |= 0x00040000;
			break;
		case 'Z':
			keyStatus |= 0x00080000;
			moveCursorLeft();
			break;
		case 'V':
			keyStatus |= 0x00100000;
			moveCursorRight();
			break;
		case '"':
			keyStatus |= 0x00200000;
			break;
		case 'X':
			keyStatus |= 0x00400000;
			moveCursorDown();
			break;
		case '@':
			keyStatus |= 0x00800000;
			break;
		case '#':
			keyStatus |= 0x01000000;
			break;
		case '%':
			keyStatus |= 0x02000000;
			putc(127); // delete ASCII code
			break;
		case '^':
			keyStatus |= 0x04000000;
			break;
		case '&':
			keyStatus |= 0x08000000;
			break;
		case 'C':
			if(isCapsOn()) {
				keyStatus &= 0xEffffff;
				switchCapsLightOff();
			}
			else {
				keyStatus |= 0x10000000;
				switchCapsLightOn();
			}
			break;
		default:
			ch = kbdus[scancode];
			if(ch >= 97 && ch <= 122 && ((isRightShiftPressed() || isLeftShiftPressed() && !isCapsOn()) || (isCapsOn() && !isRightShiftPressed() && !isLeftShiftPressed()))) {
				putc(ch - 32);
			}
			else if(ch >= 97 && ch <= 122 && isCapsOn() && (isRightShiftPressed() || isLeftShiftPressed())) {
				putc(ch);
			}
			else {
				if(isLeftShiftPressed() || isRightShiftPressed()) {
					putc(shiftLookup[ch]);
				}
				else putc(ch);
			}				
		}
	}
}

void switchCapsLightOff() {
	sendByte(0x60, 0xED);
	sendByte(0x60, 0x00);
}

void switchCapsLightOn() {
	sendByte(0x60, 0xED);
	sendByte(0x60, 0x04);
}

void sendByte(uint8_t port, uint8_t data) {
	while(1) {
		if((inb(0x64) & 2) == 0) break;
	}
	outb(port, data);
}

void keyboard_install() {
	irq_install_handler(1, keyboard_handler);
	switchCapsLightOff();
	initShiftLookup();
}

void initShiftLookup() {
	shiftLookup['/'] = '?';
	shiftLookup['.'] = '>';
	shiftLookup[','] = '<';
	shiftLookup['`'] = '~';
	shiftLookup['1'] = '!';
	shiftLookup['2'] = '@';
	shiftLookup['3'] = '#';
	shiftLookup['4'] = '$';
	shiftLookup['5'] = '%';
	shiftLookup['6'] = '^';
	shiftLookup['7'] = '&';
	shiftLookup['8'] = '*';
	shiftLookup['9'] = '(';
	shiftLookup['0'] = ')';
	shiftLookup['-'] = '_';
	shiftLookup['='] = '+';
	shiftLookup['['] = '{';
	shiftLookup[']'] = '}';
	shiftLookup[';'] = ':';
	shiftLookup['\''] = '"';
}

uint32_t isAltPressed() {
	return 0x8 & keyStatus;	
}

uint32_t isCtrlPressed() {
	return 0x1 & keyStatus;
}

uint32_t isLeftShiftPressed() {
	return 0x2 & keyStatus;
}

uint32_t isRightShiftPressed() {
	return 0x00000004 & keyStatus;
}

uint32_t isCapsOn() {
	return 0x10000000 & keyStatus;
}

uint32_t isF1Pressed() {
	return 0x10 & keyStatus;
}

uint32_t isF2Pressed() {
	return 0x20 & keyStatus;
}

uint32_t isF3Pressed() {
	return 0x40 & keyStatus;
}

uint32_t isF4Pressed() {
	return 0x80 & keyStatus;
}

uint32_t isF5Pressed() {
	return 0x100 & keyStatus;
}

uint32_t isF6Pressed() {
	return 0x200 & keyStatus;
}

uint32_t isF7Pressed() {
	return 0x400 & keyStatus;
}

uint32_t isF8Pressed() {
	return 0x800 & keyStatus;
}

uint32_t isF9Pressed() {
	return 0x1000 & keyStatus;
}

uint32_t isF10Pressed() {
	return 0x2000 & keyStatus;
}

uint32_t isNumLockPressed() {
	return 0x4000 & keyStatus;	
}

uint32_t isScrollLockPressed() {
	return 0x8000 & keyStatus;
}

uint32_t isHomeKeyPressed() {
	return 0x10000 & keyStatus;
}

uint32_t isUpArrowPressed() {
	return 0x20000 & keyStatus;
}

uint32_t isPageUpPressed() {
	return 0x40000 & keyStatus;
}

uint32_t isLeftArrowPressed() {
	return 0x80000 & keyStatus;
}

uint32_t isRightArrowPressed() {
	return 0x100000 & keyStatus;
}
uint32_t isEndKeyPressed() {
	return 0x200000 & keyStatus;
}

uint32_t isDownArrowPressed() {
	return 0x400000 & keyStatus;
}

uint32_t isPageDownPressed() {
	return 0x800000 & keyStatus;
}
uint32_t isInsertKeyPressed() {
	return 0x1000000 & keyStatus;
}

uint32_t isDeleteKeyPressed() {
	return 0x2000000 & keyStatus;
}

uint32_t isF11Pressed() {
	return 0x4000000 & keyStatus;
}
uint32_t isF12Pressed() {
	return 0x8000000 & keyStatus;
}
