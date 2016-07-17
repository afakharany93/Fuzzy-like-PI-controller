# Fuzzy-like-PI-controller
A fuzzy like PI controller to help engineers and makers control their actuators

written in : C++ (Arduino friendly)
coming soon: C and python

When to use the library? 
when you need to control an actuator in a closed loop system and make it operate at a certain set point, the advantage of fuzzy control over PID control is mainly that a fuzzy 
controller can control a system without knowing its parameters, it can control a system only by knowing the ranges of the controllers output and inputs, the controller input is 
the sensor reading, the output of the controller is the control signal the controller will produce.
an example would be controlling a DC motor with an encoder, the motor rpm range is between 0 to 1000 rpm, and the motor driver is driven by a pwm signal from the microcontroller 
on which the fuzzy controller library runs (0 - 255), the input range to the controller is 0 to 1000 rpm as it is the range the sensor will measure, the output range will be 0 - 
255 as this is the range of the output signal the controller needs to produce.

How to use the library? 
To deeply understand how fuzzy control works there are plenty of awesome online sources.
before operation, the controller takes in the ranges of the input and the output, the number of fuzzy sets of the the error and the change on error.
during operations the controller takes in the set point and the current value, then outputs the change in output, the user then adds the change in output to the previous output to 
produce the current output.
