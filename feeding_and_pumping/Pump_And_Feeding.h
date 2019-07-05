#pragma once
#include <Servo.h>

class pump_and_feeding {
private:
	int state_feeding;	// state=0 do nothing, =1 open the door =2 close, =3 auto state
	int step_feeding = 0; // step=0 waiting for signal, step=1 open the door, step=2 close the door, step=3 end step
	int cmd=0;			// used to choose which function when in auto state, cmd=3: feeding, cmd=4: pump
	int state_pump;		// 1: pump in , 2: pump out , 3 auto , 0 do nothing  
	int degree = 0;		// used to spin the vervo
	int duration, distanceCm;	// get the distance of the water
	float upper_level;  // the highest height of the tank
	float lower_level;	// the lowest height of the tank
	int height_tank;
	int step_pump = 0;
	int relay_in;
	int relay_out;
	int pin_trig;
	int pin_echo;
	Servo motor;		// init the servo
	float get_Distance()
	{
		digitalWrite(pin_trig, LOW);
		delayMicroseconds(2);
		digitalWrite(pin_trig, HIGH);
		delayMicroseconds(10);
		digitalWrite(pin_trig, LOW);
		duration = pulseIn(pin_echo, HIGH, 50000);
		distanceCm = int(duration * 0.034 / 2);
		return distanceCm;
	}
	void feeding_fish()
	{
		if (step_feeding == 0)
		{
			if (state_feeding == 1)
			{
				if (degree<50)
				degree++;
				motor.write(degree);
			}
			else if (state_feeding == 2)
			{
				if (degree>0)
				degree--;
				motor.write(degree);
			}
			else if (state_feeding == 3 && cmd == 3) step_feeding = 1;
		}
		else if (step_feeding!=0)
		{
			switch (step_feeding)
			{
			case 1:
				motor.write(degree);
				degree++;
				if (degree > 50) step_feeding = 2;
				break;
			case 2:
				motor.write(degree);
				degree--;
				if (degree == 0) step_feeding = 3;
				break;
			case 3:
				cmd = 0;
				step_feeding = 0;
				break;
			}
		}
	}
	void pump_offIN()
	{
		digitalWrite(relay_in, LOW);
		//Serial.println("off pumping");
	}
	void pump_offOUT()
	{
		digitalWrite(relay_out, LOW);
		//Serial.println("off pumping");
	}
	void pump_in()
	{
		digitalWrite(relay_out, LOW);
		digitalWrite(relay_in, HIGH);
		Serial.println("in");
	}
	void pump_out()
	{
		digitalWrite(relay_out, HIGH);
		digitalWrite(relay_in, LOW);
		Serial.println("out");
	}
	void pump_water()
	{
		switch (state_pump) {
			case 0: {
				pump_offIN();
				pump_offOUT();
				step_pump = 0;
				break;
			}
			case 1: {
				pump_out();
				pump_offIN();
				step_pump = 0;
				break;
			}
			case 2: {
				pump_offOUT();
				pump_in();
				step_pump = 0;
				break;
			}
			case 3: {
				switch (step_pump) {
					case 0: {
						pump_offOUT();
						pump_offIN();
						if (cmd == 4) step_pump++;
						break;
					}
					case 1: {
						pump_out();
						pump_offIN();
						if (distanceCm > height_tank - lower_level) step_pump++;
						break;
					}
					case 2: {
						pump_in();
						pump_offOUT();
						if (distanceCm < height_tank - upper_level) step_pump++;
						break;
					}
					case 3: {
						pump_offOUT();
						pump_offIN();
						step_pump = 0;
						cmd = 0;
						break;
					}
				}
			}

		}
	}
public:
	pump_and_feeding(int servo_pin, int relayIn, int relayOut, int pinTrig, int pinEcho, int lowerLevel = 5, int upperLevel = 30, int heightTank = 50)
	{
		pin_echo = pinEcho;
		pin_trig = pinTrig;
		relay_in = relayIn;
		relay_out = relayOut;
		upper_level = upperLevel;
		lower_level = lowerLevel;
		height_tank = heightTank;
		motor.attach(servo_pin);
		Serial.println(degree);
		pinMode(pin_trig, OUTPUT);
		pinMode(pin_echo, INPUT);
		pinMode(relay_in, OUTPUT);
		pinMode(relay_out, OUTPUT);
	}
	void receive_state(int stateFeeding, int statePump, int _cmd)
	{
		manual_cal_distance();
		//cmd =3 auto feeding, =4 auto pump
		// cuz feeding and pumping function is discrete function and can be seperated into various steps with small time
		// so we use Ticker.h for every 0.01s the receive_state void will be called again and do step by step
		state_feeding = stateFeeding;
		state_pump = statePump;
		if (_cmd != 0) cmd = _cmd;
		feeding_fish();
		pump_water();
	}
	void manual_cal_distance()
	{
		get_Distance(); // auto get the distance and save it for pumping function
		Serial.println(distanceCm);
	}

};


