/*
Written by: Ahmed El Fakharany afakharany93@gmail.com

an example would be controlling a DC motor with an encoder, the motor rpm range is between 0 to 1000 rpm, and the motor driver is driven by a pwm signal from the microcontroller 
on which the fuzzy controller library runs (0 - 255), the input range to the controller is 0 to 1000 rpm as it is the range the sensor will measure, the output range will be 0 - 
255 as this is the range of the output signal the controller needs to produce.
 the motor is required to rotate at 200 rpm.
*/

#include "fuzzy.h"

/*
creating an instance of the fuzzy controller.
the constructor takes in first argument is the number of fuzzy sets of the both error and change in error,
the second argument is the max of the input range,
third argument is the minimum of the input range,
forth argument is the maximum of the output range,
fifth argument is the minimum of the output range
*/
fuzzy cntrlr(9,1000,0,255,0);	


int op = 0;	//current output
int ch_op = 0;	//change in output
int ip = 0 ;	//input, sensor reading
void setup() 
{
	Serial.begin(115200);


}

void loop() 
{
	ip = analogRead(A0);	//input, lets pretend that the encoder output is analog :D
	ch_op = cntrlr.fuzzy_controller(ip , 200);	//arguments to the fuzzy controller (input, setpoint)
	op = op + ch_op;	//add change output to previous output
	analogWrite(9, op);
	Serial.println(op);
	delay(100);	//sampling time


}