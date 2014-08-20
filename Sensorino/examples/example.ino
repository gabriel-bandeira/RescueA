
// Include all library necessary
#include <RescueA.h>
#include <Wire.h>
#include <Sensorino.h>

// Define the variables of the library that we want to use
// I²C Sensors
Compass compass;

// Analog Sensors
Accel accel(11);

//Analog SHARP Sensors
Sharp sharp0(12);
Sharp sharp1(13);

// US Sensors
US us0(26, 28);
US us1(30, 32);

// Buttons
DigIn BT1(3);
DigIn BT2(4);
DigIn BT3(5, true); 		// Just to show what happen when we invert the default value of the library

// LED Outputs
DigOut ST1(25);
DigOut ST2(27);
DigOut ST3(29);
DigOut ST4(23, false); 	// Just to show what happen when we invert the default value of the library

// Other variables
bool state = true;
long initialTime = millis();

void setup()
{
	for(int i = 0; i < 10; i++)
		accel.xValue();

	Serial.begin(9600);
	Serial.println("Initing...");

	while(BT1.getState());
}

void loop()
{
	// Read the sensor values and print it
	Serial.print("Compass:\t");
	Serial.println(compass.getInitialAngle(), DEC);

	Serial.print("Accelerometer:\t");
	Serial.println(accel.xValue(), DEC);

	Serial.print("Sharp0:\t");
	Serial.print(sharp0.getDistance(), DEC);
	Serial.print("\tSharp1:\t");
	Serial.println(sharp1.getDistance(), DEC);
	
	Serial.print("Ulstrassonic0:\t");
	Serial.print(us0.getDistance(), DEC);
	Serial.print("\tUlstrassonic1:\t");
	Serial.println(us1.getDistance(), DEC);

	Serial.print("Button1:\t");
	Serial.print(BT1.getState(), DEC);
	Serial.print("\tButton2:\t");
	Serial.print(BT2.getState(), DEC);
	Serial.print("\tButton3:\t");
	Serial.println(BT3.getState(), DEC);

	// Set the leds states every 300 milliseconds
	if(millis() - initialTime > 300)
	{
		ST1.setState(state);
		ST2.setState(state);
		ST3.setState(state);
		ST4.setState(state);
		state = !state;
		initialTime = millis();
	}
}
