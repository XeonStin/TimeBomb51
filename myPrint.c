#include<reg51.h>
#include"myPrint.h"

xdata unsigned char numTable[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
char printStr[10] = "";         // 输出缓冲区            // 上方为数字 0~9 对应的段码

unsigned char dict(char x) {    // 将字符翻译为对应段码
	if (x >= '0' && x <= '9') {
		return numTable[x - '0'];
	} else if (x >= 'A' && x <= 'Z') {
		x += 'a' - 'A';
	}
	switch (x) {
		default : return 0; 	break;
		case '-': return 0x40; 	break;
		case '_': return 8; 	break;
		case 'a': return 119; 	break;
		case 'b': return 124; 	break;
		case 'c': return 0x39; 	break;
		case 'd': return 94; 	break;
		case 'g': return numTable[9]; break;
		case 'm': return 55; 	break;
		case 'n': return 84; 	break;
		case 'o': return 92; 	break;
		case 'r': return 80; 	break;
		case 's': return numTable[5]; break;
		case 't': return 120; 	break;
		case 'y': return 110; 	break;
	}
}

void print(void) {      // 将缓冲区的字符动态输出到数码管
	unsigned char dgt = 0x80;   // 从左到右输出
	char i, j;
	for (i=7; i>=0; --i) {
		P0 = 0xFF;              // 禁用位码，防止鬼影
		P1 = dict(printStr[i]); // 设置段码
		P0 = ~dgt;              // 设置位码
		for (j=0; j<20; ++j) ;  // 软件延迟，防止闪烁
		dgt >>= 1;
	}
}

