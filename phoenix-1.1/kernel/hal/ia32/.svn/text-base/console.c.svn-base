/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Debug console
 *
 * Copyright 2001 Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * Phoenix-RTOS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Phoenix-RTOS kernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phoenix-RTOS kernel; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <hal/current/locore.h>
#include <hal/current/types.h>
#include <hal/current/defs.h>
#include <hal/current/interrupts.h>
#include <hal/current/console.h>
#include <hal/current/vga.h>
#include <init/std.h>


void keyb_init(void);


/* Console parameters */
struct {
	unsigned char rows;       /* number of screen rows */
	unsigned char cols;       /* number of screen cols */
	uint_t        rowsize;    /* row size in bytes */
	uint_t        screensize; /* screen size - characters with attributes */
	void          *addr;      /* memory address */
	unsigned char lrow;       /* last cursor position - row */
	unsigned char lcol;       /* last cursor position - col */
	uint_t        crt_addr;   /* CRT controller base address */
	uint_t        mode;       /* mode - reserved for future use */
	mutex_t       mutex;      /* access mutex */
} conpar;



/*
 * Function initializes console in specified mode. At this time
 * only TEXT80x25 mode is allowed
 */
int console_init(unsigned int mode)
{
	uint_t color = 0;
	
	/* Test monitor type */
	color = (bus_inb(MAIN_MISCIN) & 0x01);
	
	conpar.rows = 25;
	conpar.cols = 80;
	conpar.rowsize = 80 * 2;
	conpar.screensize = 80 * 25 * 2;
	conpar.addr = KERNEL_BASE + (color ? COLOR_VRAM : MONO_VRAM);
	conpar.lrow = 0;
	conpar.lcol = 0;
	conpar.mode = mode;
	conpar.crt_addr = color ? COLOR_BASE : MONO_BASE;
	
	unlock(&conpar.mutex);	
	keyb_init();
	
	return color;
}


/* Function clears console */
int console_clear(void)
{
	unsigned int k;
	
	for (k = 0; k < conpar.screensize; k += 2) {
		*(char *)((char *)conpar.addr + k) = 0;
		*(char *)((char *)conpar.addr + k + 1) = 7;
	}
	
	return 0;
}


/* Function locks access to console for other tasks */
void console_lock(void)
{
	lock(&conpar.mutex);	
	return;
}


/* Function releases console for other tasks */
void console_unlock(void)
{
	unlock(&conpar.mutex);	
	return;
}


/* Function draws string on the console */
void console_puts(uint_t syn, char *s)
{
	char *p;
	ushort_t addr;
	uint_t k;
	
	for (p = s; *p; p++) {
		switch (*p) {
		case '\r':
		case '\n':
			conpar.lrow++;
			conpar.lcol = 0;
			break;
			
		/* delete seqence - delete last char */
		case '\177':
			if (conpar.lcol)
				conpar.lcol--;
			else {
				if (conpar.lrow) {
					conpar.lrow--;
					conpar.lcol = conpar.cols - 1;
				}
				else
					break;
			}
			*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2) = 0;
			if (syn)
				*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2 + 1) = (char)KERNMSG_COLOR;
			else
				*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2 + 1) = (char)USRMSG_COLOR;
		break;

		default:		
			*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2) = *p;
			
			if (syn)
				*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2 + 1) = (char)KERNMSG_COLOR;
			else
				*(char *)(conpar.addr + conpar.lrow * conpar.rowsize + conpar.lcol * 2 + 1) = (char)USRMSG_COLOR;
			conpar.lcol++;	
			break;
		}
		
		/* When EOL is reached go to the next row */
		if (conpar.lcol >= conpar.cols) {
			conpar.lrow++;
			conpar.lcol = 0;
		}
		
		/* Scroll console down, if max row is reached */
		if (conpar.lrow >= conpar.rows) {
			memcpy((char *)conpar.addr, (char *)(conpar.addr + conpar.rowsize), conpar.screensize - conpar.rowsize);
			conpar.lrow--;
			conpar.lcol = 0;
			
			/* Clear the last row of the screen */
			for (k = 0; k < conpar.rowsize; k += 2) {
				*(char *)(conpar.addr + conpar.screensize - conpar.rowsize + k) = 0;
				if (syn)
					*(char *)(conpar.addr + conpar.screensize - conpar.rowsize + k + 1) = (char)KERNMSG_COLOR;
				else
					*(char *)(conpar.addr + conpar.screensize - conpar.rowsize + k + 1) = (char)USRMSG_COLOR;
			}
		}	
	}
	
	/* Set hardware cursor position */
	addr = conpar.lrow * conpar.cols + conpar.lcol;	
	bus_outb(conpar.crt_addr, CRTC_CURSORH);
	bus_outb(conpar.crt_addr + 1, addr >> 8);
	bus_outb(conpar.crt_addr, CRTC_CURSORL);
	bus_outb(conpar.crt_addr + 1, addr & 0xff);
	
	return;
}


/*
 * PC 101-key keyboard handler derived from the BSD 4.4 Lite kernel
 */
 
#define KB_NONE    1
#define KB_ASCII   2
#define KB_CTL     4
#define KB_SHIFT   8
#define KB_KP      16
#define KB_ALT     32
#define KB_CAPS    64
#define KB_NUM     128
#define KB_SCROLL  256
#define KB_ALTGR   512
#define KB_FUNC    1024


#define KB_NUM_KEYS  128

typedef struct _keymap_t {
	uint_t type;
	char *unshift;
	char *shift;
	char *ctl;
	char *altgr;
	char *shift_altgr;
} keymap_t;
	 

/*
 * U.S 101 keys keyboard map
 */
 
static keymap_t	scan_codes[KB_NUM_KEYS] = {
/*  type       unshift   shift     control   altgr     shift_altgr scancode */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 0 unused */
  { KB_ASCII,  "\033",   "\033",   "\033",   "",       "" }, /* 1 ESCape */
  { KB_ASCII,  "1",      "!",      "!",      "",       "" }, /* 2 1 */
  { KB_ASCII,  "2",      "@",      "\000",   "",       "" }, /* 3 2 */
  { KB_ASCII,  "3",      "#",      "#",      "",       "" }, /* 4 3 */
  { KB_ASCII,  "4",      "$",      "$",      "",       "" }, /* 5 4 */
  { KB_ASCII,  "5",      "%",      "%",      "",       "" }, /* 6 5 */
  { KB_ASCII,  "6",      "^",      "\036",   "",       "" }, /* 7 6 */
  { KB_ASCII,  "7",      "&",      "&",      "",       "" }, /* 8 7 */
  { KB_ASCII,  "8",      "*",      "\010",   "",       "" }, /* 9 8 */
  { KB_ASCII,  "9",      "(",      "(",      "",       "" }, /* 10 9 */
  { KB_ASCII,  "0",      ")",      ")",      "",       "" }, /* 11 0 */
  { KB_ASCII,  "-",      "_",      "\037",   "",       "" }, /* 12 - */
  { KB_ASCII,  "=",      "+",      "+",      "",       "" }, /* 13 = */
  { KB_ASCII,  "\177",   "\177",   "\010",   "",       "" }, /* 14 backspace */
  { KB_ASCII,  "\t",     "\t",     "\t",     "",       "" }, /* 15 tab */
  { KB_ASCII,  "q",      "Q",      "\021",   "",       "" }, /* 16 q */
  { KB_ASCII,  "w",      "W",      "\027",   "",       "" }, /* 17 w */
  { KB_ASCII,  "e",      "E",      "\005",   "",       "" }, /* 18 e */
  { KB_ASCII,  "r",      "R",      "\022",   "",       "" }, /* 19 r */
  { KB_ASCII,  "t",      "T",      "\024",   "",       "" }, /* 20 t */
  { KB_ASCII,  "y",      "Y",      "\031",   "",       "" }, /* 21 y */
  { KB_ASCII,  "u",      "U",      "\025",   "",       "" }, /* 22 u */
  { KB_ASCII,  "i",      "I",      "\011",   "",       "" }, /* 23 i */
  { KB_ASCII,  "o",      "O",      "\017",   "",       "" }, /* 24 o */
  { KB_ASCII,  "p",      "P",      "\020",   "",       "" }, /* 25 p */
  { KB_ASCII,  "[",      "{",      "\033",   "",       "" }, /* 26 [ */
  { KB_ASCII,  "]",      "}",      "\035",   "",       "" }, /* 27 ] */
  { KB_ASCII,  "\r",     "\r",     "\n",     "",       "" }, /* 28 return */
  { KB_CTL,    "",       "",       "",       "",       "" }, /* 29 control */
  { KB_ASCII,  "a",      "A",      "\001",   "",       "" }, /* 30 a */
  { KB_ASCII,  "s",      "S",      "\023",   "",       "" }, /* 31 s */
  { KB_ASCII,  "d",      "D",      "\004",   "",       "" }, /* 32 d */
  { KB_ASCII,  "f",      "F",      "\006",   "",       "" }, /* 33 f */
  { KB_ASCII,  "g",      "G",      "\007",   "",       "" }, /* 34 g */
  { KB_ASCII,  "h",      "H",      "\010",   "",       "" }, /* 35 h */
  { KB_ASCII,  "j",      "J",      "\n",     "",       "" }, /* 36 j */
  { KB_ASCII,  "k",      "K",      "\013",   "",       "" }, /* 37 k */
  { KB_ASCII,  "l",      "L",      "\014",   "",       "" }, /* 38 l */
  { KB_ASCII,  ";",      ":",      ";",      "",       "" }, /* 39 ; */
  { KB_ASCII,  "'",      "\"",     "'",      "",       "" }, /* 40 ' */
  { KB_ASCII,  "`",      "~",      "`",      "",       "" }, /* 41 ` */
  { KB_SHIFT,  "\001",   "",       "",       "",       "" }, /* 42 shift */
  { KB_ASCII,  "\\",     "|",      "\034",   "",       "" }, /* 43 \ */
  { KB_ASCII,  "z",      "Z",      "\032",   "",       "" }, /* 44 z */
  { KB_ASCII,  "x",      "X",      "\030",   "",       "" }, /* 45 x */
  { KB_ASCII,  "c",      "C",      "\003",   "",       "" }, /* 46 c */
  { KB_ASCII,  "v",      "V",      "\026",   "",       "" }, /* 47 v */
  { KB_ASCII,  "b",      "B",      "\002",   "",       "" }, /* 48 b */
  { KB_ASCII,  "n",      "N",      "\016",   "",       "" }, /* 49 n */
  { KB_ASCII,  "m",      "M",      "\r",     "",       "" }, /* 50 m */
  { KB_ASCII,  ",",      "<",      "<",      "",       "" }, /* 51 , */
  { KB_ASCII,  ".",      ">",      ">",      "",       "" }, /* 52 . */
  { KB_ASCII,  "/",      "?",      "\037",   "",       "" }, /* 53 / */
  { KB_SHIFT,  "\002",   "",       "",       "",       "" }, /* 54 shift */
  { KB_KP,     "*",      "*",      "*",      "",       "" }, /* 55 kp * */
  { KB_ALT,    "",       "",       "",       "",       "" }, /* 56 alt */
  { KB_ASCII,  " ",      " ",      "\000",   "",       "" }, /* 57 space */
  { KB_CAPS,   "",       "",       "",       "",       "" }, /* 58 caps */
  { KB_FUNC,   "\033[M", "\033[Y", "\033[k", "",       "" }, /* 59 f1 */
  { KB_FUNC,   "\033[N", "\033[Z", "\033[l", "",       "" }, /* 60 f2 */
  { KB_FUNC,   "\033[O", "\033[a", "\033[m", "",       "" }, /* 61 f3 */
  { KB_FUNC,   "\033[P", "\033[b", "\033[n", "",       "" }, /* 62 f4 */
  { KB_FUNC,   "\033[Q", "\033[c", "\033[o", "",       "" }, /* 63 f5 */
  { KB_FUNC,   "\033[R", "\033[d", "\033[p", "",       "" }, /* 64 f6 */
  { KB_FUNC,   "\033[S", "\033[e", "\033[q", "",       "" }, /* 65 f7 */
  { KB_FUNC,   "\033[T", "\033[f", "\033[r", "",       "" }, /* 66 f8 */
  { KB_FUNC,   "\033[U", "\033[g", "\033[s", "",       "" }, /* 67 f9 */
  { KB_FUNC,   "\033[V", "\033[h", "\033[t", "",       "" }, /* 68 f10 */
  { KB_NUM,    "",       "",       "",       "",       "" }, /* 69 num lock */
  { KB_SCROLL, "",       "",       "",       "",       "" }, /* 70 scroll lock */
  { KB_KP,     "7",      "\033[H", "7",      "",       "" }, /* 71 kp 7 */
  { KB_KP,     "8",      "\033[A", "8",      "",       "" }, /* 72 kp 8 */
  { KB_KP,     "9",      "\033[I", "9",      "",       "" }, /* 73 kp 9 */
  { KB_KP,     "-",      "-",      "-",      "",       "" }, /* 74 kp - */
  { KB_KP,     "4",      "\033[D", "4",      "",       "" }, /* 75 kp 4 */
  { KB_KP,     "5",      "\033[E", "5",      "",       "" }, /* 76 kp 5 */
  { KB_KP,     "6",      "\033[C", "6",      "",       "" }, /* 77 kp 6 */
  { KB_KP,     "+",      "+",      "+",      "",       "" }, /* 78 kp + */
  { KB_KP,     "1",      "\033[F", "1",      "",       "" }, /* 79 kp 1 */
  { KB_KP,     "2",      "\033[B", "2",      "",       "" }, /* 80 kp 2 */
  { KB_KP,     "3",      "\033[G", "3",      "",       "" }, /* 81 kp 3 */
  { KB_KP,     "0",      "\033[L", "0",      "",       "" }, /* 82 kp 0 */
  { KB_KP,     ",",      "\177",   ",",      "",       "" }, /* 83 kp . */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 84 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 85 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 86 0 */
  { KB_FUNC,   "\033[W", "\033[i", "\033[u", "",       "" }, /* 87 f11 */
  { KB_FUNC,   "\033[X", "\033[j", "\033[v", "",       "" }, /* 88 f12 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 89 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 90 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 91 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 92 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 93 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 94 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 95 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 96 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 97 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 98 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 99 0 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 100 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 101 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 102 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 103 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 104 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 105 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 106 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 107 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 108 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 109 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 110 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 111 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 112 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 113 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 114 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 115 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 116 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 117 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 118 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 119 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 120 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 121 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 122 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 123 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 124 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 125 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 126 */
  { KB_NONE,   "",       "",       "",       "",       "" }, /* 127 */
};


/* Get characters from keyboard */
char *keyb_get(void)
{
	uchar_t dt;
	static uchar_t extended = 0, lock_state = 0;
	static uint_t shift_state = 0;
	static uchar_t capchar[2];

	dt = bus_inb(0x60);
		
	capchar[0] = 0;
	capchar[1] = 0;
	
	/* Key is released */
	if (dt & 0x80) {
		dt &= 0x7f;
		switch (scan_codes[dt].type) {
		case KB_NUM:
			shift_state &= ~KB_NUM;
			break;
		case KB_CAPS:
			shift_state &= ~KB_CAPS;
			break;
		case KB_SCROLL:
			shift_state &= ~KB_SCROLL;
			break;
		case KB_SHIFT:
			shift_state &= ~KB_SHIFT;
			break;
		case KB_ALT:
			if (extended)
				shift_state &= ~KB_ALTGR;
			else
				shift_state &= ~KB_ALT;
			break;
		case KB_CTL:
			shift_state &= ~KB_CTL;
			break;
		}
	}
	
	/* Key is pressed */
	else {
		switch (scan_codes[dt].type) {

		/* Locking keys - Caps, Scroll, Num */
		case KB_NUM:
			if (shift_state & KB_NUM)
				break;
			shift_state |= KB_NUM;
			lock_state ^= KB_NUM;
			/*async_update();*/
			break;
		case KB_CAPS:
			if (shift_state & KB_CAPS)
				break;
			shift_state |= KB_CAPS;
			lock_state ^= KB_CAPS;
			/*async_update();*/
			break;
		case KB_SCROLL:
			if (shift_state & KB_SCROLL)
				break;
			shift_state |= KB_SCROLL;
			lock_state ^= KB_SCROLL;
			/*async_update();*/
			break;

		/* Special no locking keys */
		case KB_SHIFT:
			shift_state |= KB_SHIFT;
			break;
		case KB_ALT:
			if (extended)
				shift_state |= KB_ALTGR;
			else
				shift_state |= KB_ALT;
			break;
		case KB_CTL:
			shift_state |= KB_CTL;
			break;
		
		/* Regular ASCII */
		case KB_ASCII:
			
			/* Control is pressed */
			if (shift_state & KB_CTL)
				capchar[0] = scan_codes[dt].ctl[0];
			
			/* Right alt and right alt with shift */
			else if (shift_state & KB_ALTGR) {
				if (shift_state & KB_SHIFT)
					capchar[0] = scan_codes[dt].shift_altgr[0];
				else
					capchar[0] = scan_codes[dt].altgr[0];
						
			/* Shift */
			} else {
				if (shift_state & KB_SHIFT)
					capchar[0] = scan_codes[dt].shift[0];
				
				/* Only key without special keys */
				else
					capchar[0] = scan_codes[dt].unshift[0];
			}
			
			/* If CAPS is active capitalize letters */
			if ((lock_state & KB_CAPS) && capchar[0] >= 'a' && capchar[0] <= 'z') {
				capchar[0] -= ('a' - 'A');
			}
			
			/* Left ALT */
			capchar[0] |= (shift_state & KB_ALT);
			extended = 0;
			return capchar;
		
		/* Key without meaning */	
		case KB_NONE:
			break;
			
		/* Function key */	
		case KB_FUNC: {
			char *more_chars;
			if (shift_state & KB_SHIFT)
				more_chars = scan_codes[dt].shift;
			else if (shift_state & KB_CTL)
				more_chars = scan_codes[dt].ctl;
			else
				more_chars = scan_codes[dt].unshift;
			extended = 0;
			return more_chars;
		}
		
		/* Keypad */
		case KB_KP: {
			char *more_chars;
			
			/* Reboot sequence */
			if ((shift_state & KB_ALT) && (shift_state & KB_CTL) && (dt == 83)) {
				std_printf("Rebooting ...\n");
				reboot();
				return capchar;
			}
			
			if (shift_state & (KB_SHIFT | KB_CTL) || (lock_state & KB_NUM) == 0 || extended)
				more_chars = scan_codes[dt].shift;
			else
				more_chars = scan_codes[dt].unshift;
			extended = 0;
			return more_chars;
		}
		}
	}

	extended = 0;
	return capchar;
}


/* Keyboard interrupt dummy handler */
void keyb_intr_handler(uint_t intr)
{
	__intr_end(intr);
	std_printf("%c", *keyb_get());
	return;
}


/* Function initializes dummy keyboard handler */
void keyb_init(void)
{
	set_intr_handler(1, (void *)&keyb_intr_handler);
	return;
}
