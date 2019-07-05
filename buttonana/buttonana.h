#pragma once
#include <stdint.h>

class buttonana {
       private:
	int pin;
	int low;
	int up;
	unsigned long delay;
	unsigned long ftdelay;
	unsigned long t = 0;

       public:
	buttonana(int pin, int delay = 1000, int l = 0, int u = 0) {
		this->pin = pin;
		this->delay = delay;
		this->low = l;
		this->up = u;
	}

	void pres(int &state, int state_num, void (*f)()) {
		int v = analogRead(pin);
		if(v < low || v > up) {
			t = millis();
			ftdelay = 10;
		}
		if(t <= millis() - ftdelay) {
			t = millis();
			ftdelay = delay; 
			state = (state + 1) % state_num;
			f();
		}
	}
};
