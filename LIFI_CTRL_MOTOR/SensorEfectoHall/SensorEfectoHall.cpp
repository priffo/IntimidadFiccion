#include "arduino.h"
#include "SensorEfectoHall.h"

SensorEfectoHall::SensorEfectoHall()
{
	
}

SensorEfectoHall::SensorEfectoHall(int pinSensor)
{
	setPin(pinSensor);
}

void SensorEfectoHall::setPin(int pinSensor)
{
	_pinSensor = pinSensor;
	pinMode(_pinSensor, INPUT);
}

bool SensorEfectoHall::hayCampoPresente()
{
	return !digitalRead(_pinSensor);
}