
#include "Arduino.h"
#include "Sensorino.h"


US::US(int trig, int echo)
{
	pinMode(echo, INPUT);
	pinMode(trig, OUTPUT);
	_echo = echo;
	_trig = trig;
}

int US::getDistance()
{
	initialTime = 0;
	finalTime = 0;
	state = 0;

	digitalWrite(_trig, LOW);
	delayMicroseconds(2);
	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);
	digitalWrite(_trig, LOW);

	us_InitialTime = millis();
	while(state != 2 && (US_TIME_LIMIT > (millis() - us_InitialTime)))
	{
		if(state == 0 && digitalRead(_echo) == HIGH)
		{
			initialTime = micros();
			state = 1;
		}
		if(state == 1 && digitalRead(_echo) == LOW)
		{
			finalTime = micros();
			state = 2;
		}
	}

	if(finalTime < initialTime)
		return 255;
	else if((initialTime > 0) && (finalTime > 0))
		return (finalTime - initialTime) / 35;//((10^6) * 3.4);
	else
		return  255;
   
}

void US::send(int data)
{
	// TODO
	// send a int by the emissor
}

int US::receive()
{
	// TODO
	// receive a int by the receptor
	return 0;
}


Accel::Accel(unsigned int xPin, unsigned int yPin, unsigned int zPin)
{
	_pinX = xPin;
	_pinY = yPin;
	_pinZ = zPin;
}

int Accel::xValue()
{
	return (analogRead(_pinX));
}

Sharp::Sharp(int aInput)
{
	_analogInput = aInput;
	_in[0] = 0.4;
	_in[1] = 0.45;
	_in[2] = 0.51;
	_in[3] = 0.61;
	_in[4] = 0.74;
	_in[5] = 0.93;
	_in[6] = 1.08;
	_in[7] = 1.31;
	_in[8] = 1.64;
	_in[9] = 2.31;
	_in[10] = 2.74;
	_in[11] = 2.97;
	_in[12] = 3.15;

	_out[0] = 80.0;
	_out[1] = 70.0;
	_out[2] = 60.0;
	_out[3] = 50.0;
	_out[4] = 40.0;
	_out[5] = 30.0;
	_out[6] = 25.0;
	_out[7] = 20.0;
	_out[8] = 15.0;
	_out[9] = 10.0;
	_out[10] = 8;
	_out[11] = 6.5;
	_out[12] = 6.1;
}

int Sharp::getDistance()
{
	double val = analogRead(_analogInput)*0.0048828125;
	// take care the value is within range
	// constrain(val, in[0], in[13-1]);
	if (val <= _in[0])
		return _out[0] * 10;
	if (val >= _in[13-1])
		return _out[13-1] * 10;

	// search right interval
	int pos = 1;  // _in[0] allready tested
	while(val > _in[pos])
		pos++;

	// this will handle all exact "points" in the _in array
	if (val == _in[pos])
		return _out[pos];
	// interpolate in the right segment for the rest
	return 10 * ((val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1]);
}

DigOut::DigOut(int digPin, boolean onState)
{
	pinMode(digPin, OUTPUT);
	_digPin = digPin;
	_onState = onState;
}

void DigOut::setState(boolean state)
{
	if(_onState)
		digitalWrite(_digPin, state);
	else
		digitalWrite(_digPin, !state);
}

DigIn::DigIn(int pin, boolean pressedState)
{
	pinMode(pin, INPUT);
	_pin = pin;
	_pressedState = pressedState;
}

boolean DigIn::getState()
{
	if(_pressedState)
		return digitalRead(_pin);
	else
		return !digitalRead(_pin);
}

Compass::Compass(DigOut *ledCompassError, int precision, int compassAddress)
{
	Wire.begin();
	_precision = precision;
	_ledCompassError = ledCompassError;
	// TODO
	// Check if it's working just fine as it was when a variable was not set to the address
	_compassAddress = compassAddress >> 1;
}

int Compass::getReading()
{
	Wire.beginTransmission(_compassAddress);
	Wire.write('A');
	Wire.endTransmission();

	Wire.requestFrom(_compassAddress, 2);

	_timeLimit_InitialTime = millis();

	while(Wire.available() < 2)
	{
		if((millis() - _timeLimit_InitialTime) > COMPASS_READING_TIME_LIMIT)
		{
			if(_ledCompassError != NULL)
				_ledCompassError->setState(true);
			return -1;
		}
	}

	_compassValue = Wire.read();
	_compassValue = _compassValue << 8;
	_compassValue += Wire.read();

	if(_precision != 0)
		return _compassValue / _precision;
	else
		return _compassValue;
}

int Compass::relativeAngle(int initialAngle)
{
	_delta = getReading() - initialAngle;

	if (_delta > 180)
		_delta -= 360;
	if (_delta < -180)
		_delta += 360;
	return _delta;
}

boolean Compass::check()
{
	Wire.beginTransmission(_compassAddress);
	Wire.write('A');
	Wire.endTransmission();

	Wire.requestFrom(_compassAddress, 2);

	_timeLimit_InitialTime = millis();

	while(Wire.available() < 2)
	{
		if((millis() - _timeLimit_InitialTime) > COMPASS_READING_TIME_LIMIT)
		{
			return false;
		}
	}

	Wire.read();
	Wire.read();

	return true;
}

int Compass::getInitialAngle(int precision, int delayTime, int numOfDescartedReadings)
{
	int i;
	for(i = 0; i < numOfDescartedReadings; i++)
	{
		getReading();
		delay(delayTime);
	}

	 for(i = 0; i < precision; i++)
	 {
		_retorno[i] = this->getReading();
		delay(delayTime);

		#ifdef PRINT_SERIAL_COMPASS
			Serial.print(_retorno[i]);
			Serial.print("   ");
		#endif
	}

	#ifdef PRINT_SERIAL_COMPASS
		Serial.print("\t filtrated: \t");
		Serial.println(filter());
		delay(500);
	#endif

	return filter();
}

int Compass::filter()
{
	int i, x;
	for(i = 0; i < 3; i++)
	{
		_max[i] = -1000;
		_min[i] = 9999;
	}

	_amount = 0;
	_cont = 0;

	for(x = 0; x < 3; x++)
	{
		for(i = 0; i < 10; i ++)
		{
			_max[x] = max(_max[x], _retorno[i]);
			_min[x] = min(_min[x], _retorno[i]);
		}
	}

	for(x = 0; x < 3; x++)
	{
		for(i = 0; i < 10; i++)
		{
			if(_retorno[i] == _max[x])
				_retorno[i] = 0;
			else if(_retorno[i] == _min[x])
				_retorno[i] = 0;
		}
	}

	for(i = 0; i < 10; i++)
	{
		if(_retorno[i] != 0)
		{
			_cont++;
			_amount += _retorno[i];
		}
	}

	if(_cont != 0 && _cont != 10)
		return _amount / _cont;
	else
		return _max[0];
}

