// ---------------------------------------------------------------------------
//	FM sound generator common timer module
//	Copyright (C) cisc 1998, 2000.
//	Copyright (C) 2011 ALOE. All rights reserved.
// ---------------------------------------------------------------------------
#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace FlMML {

class Timer {
private:
	int regta[2];
	int timera;int timera_count;
	int timerb;int timerb_count;
	int timer_step;
protected:
	int status;
	int regtc;
public:
	Timer() {
		//regta=new vector<int> (2);
	}
	virtual ~Timer() {
	}
	
	void Reset() {
		timera_count=0;
		timerb_count=0;
	}
	
	bool Count(int us) {
		bool f=false;
		
		if (timera_count!=0){
			timera_count-=us<<16;
			if (timera_count<=0){
				f=true;
				TimerA();
				
				while(timera_count<=0)
					timera_count+=timera;
				
				if ((regtc&4)!=0)
					SetStatus(1);
			}
		}
		if (timerb_count!=0){
			timerb_count-=us<<12;
			if (timerb_count<=0){
				f=true;
				while(timerb_count<=0)
					timerb_count+=timerb;
				
				if ((regtc&8)!=0)
					SetStatus(2);
			}
		}
		
		return f;
	}
	
	int GetNextEvent() {
		int ta=((timera_count+0xffff)>>16)-1;
		int tb=((timerb_count+0xfff)>>12)-1;
		return (ta<tb?ta:tb)+1;
	}
protected:
	void SetStatus(int bit) {}
	void ResetStatus(int bit) {}
	
	void SetTimerBase(int clock) {
		timer_step=(int )(1000000.0*65536/clock);
	}
	
	void SetTimerA(int addr,int data) {
		int tmp;
		regta[addr&1]=(int )(data);
		tmp=(regta[0]<<2)+(regta[1]&3);
		timera=(1024-tmp)*timer_step;
	}
	
	void SetTimerB(int data) {
		timerb=(256-data)*timer_step;
	}
	
	void SetTimerControl(int data) {
		int tmp=regtc^data;
		regtc=(int )(data);
		
		if ((data&0x10)!=0)
			ResetStatus(1);
		if ((data&0x20)!=0)
			ResetStatus(2);
		
		if ((tmp&0x01)!=0)
			timera_count=((data&1)!=0)?timera:0;
		if ((tmp&0x02)!=0)
			timerb_count=((data&2)!=0)?timerb:0;
	}
	
	void TimerA() {}
};

}
