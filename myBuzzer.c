#include<reg51.h>
#include<stdio.h>
#include"myBuzzer.h"
#include"myPrint.h"

xdata unsigned int noteFreq[] = {
	0, 262, 294, 330, 349, 392, 440, 494, 		// nop, C4 - B4
	523, 587, 659, 698, 784, 880, 988,			// C5 - B5
	1047, 1175, 1319, 1397, 1568, 1760, 1976	// C6 - B6
};

sbit buzzer = P2^5;

unsigned long cnt0;             // 计数变量
unsigned int reload;            // 存放当前频率对应的计数初值
unsigned char enableBuzzer = 1; // 当beep工作在延时模式时使
                                // enableBuzzer = 0，蜂鸣器不响
void Timer0Int(void) interrupt 1 {
	TH0 = reload >> 8;          // 载入计数初值
	TL0 = reload & 0x00FF;
	TR0 = 1;
	++ cnt0;
	if (enableBuzzer) {
		buzzer = ~buzzer;       // 生成特定频率的声音
	}
}

int setFreq(int freq) {         // 根据频率设置reload变量
	if (freq < 0) {             // 输入为表中音高
		freq = noteFreq[-freq];
	}
	reload = 65536 - 500000 / freq;
	return freq;
}
	
void beep(int freq, unsigned long duration) {
	unsigned long endTime;      // 存放计数次数
	if (!freq) {
		reload = 65536 - 1000;
		endTime = duration;
		enableBuzzer = 0;
	} else {
        freq = setFreq(freq);
		endTime = duration * freq / 500;
		enableBuzzer = 1;
	}
	TMOD = (TMOD & 0xF0) | 0x01;    //不改变T1对应的TMOD
	EA = ET0 = 1;
	TH0 = reload >> 8;
	TL0 = reload & 0x00FF;
	cnt0 = 0;
	buzzer = 0;
	TR0 = 1;
	while (cnt0 < endTime) {
		print();                    // 保持屏幕显示
	}
	TR0 = 0;
	buzzer = 0;
	enableBuzzer = 1;
}
