#include "screen.h"
#include "system.h"

#include <stdint.h>
#include <stdarg.h>

#define VIDEO_MEMORY 	0xb8000
#define GRAY_ON_CYAN 	0x9000
#define ROWS 		25
#define COLUMNS		80

int32_t cx, cy;
void update_cursor();
void move_cursor(uint8_t x, uint8_t y);
void scroll_if_needed();

void init_video() {
	for(uint8_t i = 0; i < ROWS; i++) {
		for(uint8_t j = 0; j < COLUMNS; j++) {
			uint16_t* ptr = (uint16_t*)VIDEO_MEMORY;
			ptr = ptr + (i * COLUMNS + j);
			*ptr = GRAY_ON_CYAN | ' ';
		}
	}
	cx = 0;
	cy = 0;
	update_cursor();
}

void update_cursor() {
	move_cursor(cx, cy);
}

void move_cursor(uint8_t x, uint8_t y) {
	uint16_t pos = y * COLUMNS + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void putc(uint8_t character) {
	uint16_t* ptr = (uint16_t*)VIDEO_MEMORY;
	ptr = ptr + (cy * COLUMNS + cx);
	if(character == '\n') {
		cy++;
		cx = 0;
	}
	else if(character == '\r') {
		cx = 0;
	}
	else if(character == '\b') {
		if(cx <= 0 && cy <= 0) {
			return;
		}
		uint16_t* lastChar = (uint16_t*)(VIDEO_MEMORY + (cy * COLUMNS + cx - 1) * 2);
		*lastChar = GRAY_ON_CYAN | ' ';
		cx--;
		if(cx < 0) {
			cx = COLUMNS - 1;
			cy--;
		}
	}
	else if(character == '\t') {
		uint16_t pos = cy * COLUMNS + cx;
		pos++;
		while(pos++ % 8 != 0);
		cy = pos / COLUMNS;
		cx = pos % COLUMNS;
	}
	else if(character == 127) { // the delete button
		*ptr = GRAY_ON_CYAN | ' ';
	}
	else {
		*ptr = GRAY_ON_CYAN | character;
		if(cx < COLUMNS - 1) cx++;
		else {
			cx = 0;
			cy++;
		}
	}
	
	scroll_if_needed();
	update_cursor();
}

void puts(uint8_t* string) {
	int i = 0;
	while(string[i]) {
		putc(string[i++]);
	}
}

void puti(uint32_t a) {
	if(a == 0) {
		putc(48);
		return;
	}
	int b = 1000000000;

	while(b > a) {
		b /= 10;
	}
	while(b) {
		putc((a / b + 48));
		a %= b;
		b /= 10;
	}
}

void puth(uint32_t a) {
	puts("0x");
	
	if(a == 0) {
		putc(48);
		return;
	}
	
	uint32_t b = 0x10000000;
	
	while(b > a) {
		b /= 0x10;
	}
	
	while(b) {
		uint8_t d = a / b;
		if(d > 9) {
			d -= 9;
			putc(64 + d);
		}
		else {
			putc((a / b) + 48);
		}
		a %= b;
		b /= 0x10;
	}
}

void putb(uint32_t a) {
	uint8_t i;
	for(i = 31; i >= 0; i--) {
		putc((uint8_t)((a & (1 << i))? 49 : 48));
	}
}

void scroll_if_needed() {
	while(cy >= ROWS) {
		uint8_t i;
		uint8_t *dest, *src;
		for(i = 1; i < ROWS; i++) {
			dest = (uint8_t*)(VIDEO_MEMORY + (i - 1) * COLUMNS * 2);
			src = (uint8_t*)(VIDEO_MEMORY + i * COLUMNS * 2);
			memcpy(dest, src, COLUMNS * 2);
		}
		uint16_t* ptr = (uint16_t*)VIDEO_MEMORY + (ROWS - 1) * COLUMNS;
		for(uint8_t i = 0; i < COLUMNS; i++) {
			ptr[i] = GRAY_ON_CYAN | ' ';
		}
		cy--;
	}
}

void putarri(uint32_t* arr, uint32_t n) {
	uint16_t i;
	for(i = 0; i < n; i++) {
		puti(arr[i]);
		puts(", ");
	}
}

void printf(uint8_t* str, ...) {
	va_list valist;
	va_start(valist, str);
	int j = 0;
	while(str[j]) {
		uint8_t ch = str[j];
		if(ch == '%') {
			ch = str[++j];
			switch(ch) {
			case 'd':
				puti(va_arg(valist, uint32_t));
				break;
			case 'x':
				puth(va_arg(valist, uint32_t));
				break;
			case '%':
				putc('%');
				break;
			case 'b':
				putb(va_arg(valist, uint32_t));
				break;
			case 's':
				puts(va_arg(valist, uint8_t*));
				break;
			case 'c':
				putc(va_arg(valist, uint32_t));
				break;
			}
		} else putc(str[j]);
		++j;
	}
	va_end(valist);
}

void moveCursorUp() {
	if(cy > 0) cy--;
	update_cursor();
}

void moveCursorDown() {
	if(cy < ROWS - 1) cy++;
	update_cursor();
}

void moveCursorRight() {
	if(cx < COLUMNS - 1) cx++;
	update_cursor();
}

void moveCursorLeft() {
	if(cx > 0) cx--;
	update_cursor();
}
