#include<reg51.h>
#include<string.h>
#include<stdio.h>
#include"myPrint.h"
#include"myBuzzer.h"

unsigned long explodeTime = 15, defuseTime = 4, plantTime = 4;
unsigned long cnt1ms = 0;               // 以上计时值可更改

sbit button = P2^7;
sbit buzzer = P2^5;

void Timer1Int_1ms(void) interrupt 3 {
	TH1 = 0xFC;
	TL1 = 0x18;
	TR1 = 1;
	++ cnt1ms;
}

void delayms(unsigned long time) {      // 典型计时方式
	unsigned long t1 = cnt1ms + time;
	while (cnt1ms < t1);
}

/*
void playSong(void) {
	xdata char score[] = "115566550044332211005544332200554433220011556655004433221100";
	int len = strlen(score), i;
	TR1 = 0;
	for (i=0; i<len; ++i) {
		beep(score[i] - '0', 400);
	}
}
*/

void exploded() {               // 炸弹爆炸
	int i;
	strcpy(printStr, "    boom");   // 显示文字
	print();
	for (i=670; i>=20; i-=10) {     // 爆炸音效
		beep(i, 54-0.08*i);
	}
	while (1) {
		if (cnt1ms % 2000 < 1000) { // 文字闪烁
			strcpy(printStr, "    boom");
		} else {
			strcpy(printStr, "        ");
		}
		print();
	}
}

void defused(void) {            // 拆弹成功
	int i;
	strcpy(printStr, "congrats");
	print();
	for (i=3; i>=1; --i) {      // 拆弹成功音效
		beep(-i, 100);
	}
	while (1) {
		if (cnt1ms % 2000 < 1000) { // 文字闪烁
			strcpy(printStr, "congrats");
		} else {
			strcpy(printStr, "        ");
		}
		print();
	}
}

void defusing(unsigned long bombEndTime) {      // 拆弹过程，参数为炸弹爆炸时间
	unsigned long startTime = cnt1ms, endTime = cnt1ms + 1000 * defuseTime;
	memset(printStr, 0, sizeof printStr);
	
	while (cnt1ms < endTime) {
		printStr[(cnt1ms - startTime) / 125 / defuseTime] = '-';
		print();                                // 拆弹进度条
		
		setFreq(-12);
		if ((cnt1ms - startTime) % 500 < 100) {
			TR0 = 1;
		} else {
			TR0 = 0;
		}
		
		if (button == 1) {
			delayms(50);
			if (button == 1) {
				TR0 = 0;
				return ;
			}
		}
		if (cnt1ms >= bombEndTime) {        // 拆弹过程中炸弹爆炸
			TR0 = 0;
			exploded();
		}
	}
	TR0 = 0;
	defused();
}

void planted(void) {        // 计时待爆
	unsigned long startTime = cnt1ms, endTime = cnt1ms + 1000 * explodeTime;
	char flg = 0;
	int i;
	for (i=1; i<=3; ++i) {  // 安放成功音效
		beep(-i, 100);
	}
	while (cnt1ms < endTime) {
		sprintf(printStr, "%8ld", (endTime - cnt1ms)/1000+1);
		print();                                    // 显示倒计时
		
		if ((cnt1ms - startTime) % 1000 < 200) {    // 计时音效
			buzzer = 1;
		} else {
			buzzer = 0;
		}
			
		if (button == 0) {
			if (flg) {			    // 隔离安放炸弹与拆弹的按键操作
				delayms(50);
				buzzer = 0;
				defusing(endTime);
			}
		} else {
			flg = 1;
		}
	}
	buzzer = 0;
	exploded();
}

void planting(void) {       // 安放过程
	xdata char password[] = "7355608_";
	unsigned long startTime = cnt1ms, endTime = cnt1ms + 1000 * plantTime;  // 设置计时终点
	memset(printStr, 0, sizeof printStr);
	while (cnt1ms < endTime) {  // 典型的计时方法
		printStr[(cnt1ms - startTime) / 125 / plantTime] = password[(cnt1ms - startTime) / 125 / plantTime];
		printStr[(cnt1ms - startTime) / 125 / plantTime + 1] = '_';
		
		setFreq(-12);                               // 设置音调为 G5
		if ((cnt1ms - startTime) % 500 < 100) {     // 计时的同时播放音效
			TR0 = 1;
		} else {
			TR0 = 0;
		}
		
		print();
		if (button == 1) {      // 按钮松开，中止安放炸弹
			delayms(50);
			if (button == 1) {
				TR0 = 0;        // 切换到其他下一状态前关闭音效
				return ;
			}
		}
	}
	TR0 = 0;
	planted();      // 计时结束，炸弹已安放
}

void standby(void) {        // 待机状态
	while (1) {
		strcpy(printStr, " standby");
		print();            // 显示文字
		if (button == 0) {  // 按下按钮，开始安放炸弹
			delayms(50);    // 防抖动
			planting();     // 状态切换
		}
	}
}

void main(void) {   // 主函数
	TMOD = 0x10;    // 初始化计时器 1
	EA = ET1 = 1;
	TH1 = 0xFC;
	TL1 = 0x18;
	TR1 = 1;
	button = 1;     // 向半双工端口输出 1
	beep(0, 0);     // 初始化蜂鸣器
	standby();      // 进入待机模式
}
