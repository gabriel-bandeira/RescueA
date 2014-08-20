/*
	Sensorino - Library for reading digital and analog sensor data and controlling digital output
	Implementation started at May 2012
	By:
	-Gabriel Bandeira
	e-mail: gmbandeira3@gmail.com
	github: github.com/gmbandeira
	-Lucas Cavalcanti
	e-mail: lucas.cavalcanti.santos@gmail.com

	Using Sharp Sensor Filter implemented by:
	Ivan Seidel
	github: github.com/ivanseidel

	+ Digital Ultrassonic Interface
	+ Analog Infrared Interface
	+ Digital Touch Sensor Interface
	+ Digital Output Interface

	--------------------------------------------------------------------------------------------------------------------------------------

	1º Edition
	Sensorino1.2 	- September - 2013
	By:
	-Gabriel Bandeira
	e-mail: gmbandeira3@gmail.com
	github: github.com/gmbandeira
	Revised by:
	-Lucas Cavalcanti
	e-mail: lucas.cavalcanti.santos@gmail.com

	+ Identation
	+ Documentation
	+ Translation
	+ I2C Compass Sensor Functions

	--------------------------------------------------------------------------------------------------------------------------------------

	2º Edition
	Sensorino1.2.1 	- August - 2014
	By:
	-Gabriel Bandeira
	e-mail: gmbandeira3@gmail.com
	github: github.com/gmbandeira/rescueA

	+ Better Identation
	+ Even more documentation
	+ Future work
	+ It's OpenSource!
	+ Example

	--------------------------------------------------------------------------------------------------------------------------------------

	Future work:
	Sensorino1.3

	+ Communication by ultrassonic sensor
	+ Implement PING Ultrassonic sensor interface
	+ Activate sharp sensor using a digital out
	+ Documentation of the CPP file

*/

#ifndef _SENSORINO_1_2_1_INCLUDED_

	#define _SENSORINO_1_2_1_INCLUDED_

	#include "Arduino.h"
	#include <Wire.h>


	// Time limit for waiting ultrassonic reflection
	#define US_TIME_LIMIT 20
	// Time limit to don't break the program reading the compass sensor if it's not responding
	#define COMPASS_READING_TIME_LIMIT 50 		// getReading; check;


	// Digital Ultrassonic Sensor
	class US
	{
		public:
			// Class contructor
			// trig = the pulse emissor of the sensor
			// echo = the pulse receiver of the sensor
			US(int trig, int echo);
			// Return the distance read by the ultrassonic (range 0 - 255)
			// If any error accurs, return 255
			int getDistance();
			// TODO
			// send a int using the emissor
			void send(int data);
			// TODO
			// read a int using the receptor
			int receive();
		private:
			// US pins for sending and receiving ultrassonic pulses
			int _echo;
			int _trig;
			// Initial and final time for calculating the distance
			long initialTime;
			long finalTime;
			// Initial time to calculate the waiting limit
			long us_InitialTime;
			// Sending, receiving or waiting states
			// TODO
			// Check if char works just fine as int worked
			char state;
	};

	// Analog Accelerometer Sensor
	// Sensorino1.2* only gets the X axis value, later versions may read the 3 axis
	class Accel
	{
		public:
			// Class constructor
			// xPin = the pin used to read the X value of the sensor
			// yPin = the pin used to read the Y value of the sensor
			// zPin = the pin used to read the Z value of the sensor
			Accel(unsigned int xPin, unsigned int yPin = 0, unsigned int zPin = 0);
			// return X axis accelerometer reading (between 0 - 1023)
			int xValue();
		private:
			// Stores the pins of the sensor to read
			int _pinX;
			int _pinY;
			int _pinZ;
	};

	// Analog Infrared Sensor
	class Sharp
	{
		public:
			// Class constructor
			// aInput = the analog input to read the sensor value
			Sharp(int aInput);
			// Return the distance (value between 0 - 80)
			int getDistance();
		private:
			int _analogInput;
			double _in[12];
			double _out[12];
	};

	// General Purpose Digital Output
	class DigOut
	{
		public:
			// Class constructor
			// digPin = the digital pin to use
			// onState = the boolean value to be to turn on, default is true. If false, the off value is defined as true
			DigOut(int digPin, boolean onState = true);
			// Set a digital state (true or false)
			void setState(boolean state);
		private:
			// Internal digital output
			int _digPin;
			// Internal on value
			boolean _onState;
	};          

	// General Purpose Digital Input
	class DigIn
	{
		public:
			// Class constructor
			// pin = the digital pin to read
			// pressedState = the boolean value when is turned on, default is false. If true, the off value is defined as false
			DigIn(int pin, boolean pressedState = false);
			// Get a digital state (true or false) acording to the pressedState defined above
			boolean getState();
		private:
			// Internal digital input
			int _pin;
			// Internal on value
			boolean _pressedState;
	};

	// I²C Compass. HMC6352
	class Compass
	{
		public:
			// Class contructor
			// ledCompassError = an output of this library to turn on when the compass is not responding
			// precision = a value to divide the compass reading, if not defined as 0
			// compassAddress = the I²C address
			Compass(DigOut *ledCompassError = NULL, int precision = 10, int compassAddress = 0x42);
			// Compass current reading
			int getReading();
			// Compass relative angle (-180 - +180)
			// initialAngle = the angle to read a relative angle with repect to
			int relativeAngle(int inicialAngle);
			// A simple check to know if everything is ok with the sensor
			boolean check();
			//void calibrate();
			// A custom angle reading that filter a few wrong reading that usually occur in the first readings
			// precision = number of reading to make
			// delayTime = delay in milliseconds between the readings
			// numOfDescartedReadings = number of wrong readings to descart
			int getInitialAngle(int precision = 10, int delayTime = 15, int numOfDescartedReadings = 10);

		private:
			// type		name				used by

			// Descart the wrong readings of the sensor
			int 		filter();				// getInitialAngle;
			// Internal sensor reading used for put the two bytes together before return it value
			int 		_compassValue;			// getReading;
			// The initial time used to do not stop the program if the sensor is not responding (check COMPASS_READING_TIME_LIMIT)
			long 		_timeLimit_InitialTime;		// getReading; check;
			// Internal variable to get the relative angle
			int 		_delta;				// relativeAngle;
			// Internal digital output to turn on when the compass have a timeout error when reading
			DigOut 		*_ledCompassError;		// Compass; getReading;

			// Internal variables
			int 		_compassAddress;			// Compass, getReading; check;
			int 		_precision;			// Compass; getReading;

			int 		_retorno[10];			// getInitialAngle;

			// Internal variables used to filter the reading in the filter() function
			int 		_max[3];				// filter;
			int 		_min[3];				// filter;
			int 		_amount;				// filter;
			int 		_cont;				// filter;
	};

#endif
