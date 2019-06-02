#pragma once
#include <stdint.h>

class button {
       private:
	int pin;
	unsigned long delay;
	unsigned long ftdelay;
	unsigned long t = 0;

       public:
	button(int pin, int delay = 200) {
		this->pin = pin;
		this->delay = delay;
	}

	void pres(int &state, int state_num, void (*f)()) {
		if (!digitalRead(pin)) {
			t = millis();
			ftdelay = 10;
		}
		if (t <= millis() - ftdelay) {
			t = millis();
			ftdelay = delay;
			state = (state + 1) % state_num;
			f();
		}
	}
};
