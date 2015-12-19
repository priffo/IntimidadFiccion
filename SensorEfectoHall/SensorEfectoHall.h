/*
  SensorEfectoHall.h - Librería para un sernsor de efecto hall.
  Creado por Pedro Riffo. 19/09/2015 Santiago, Chile.
  Para el proyecto Fondart: La intimidad de la ficción(LIFI).
*/
#ifndef SensorEfectoHall_h
#define SensorEfectoHall_h

#include "Arduino.h"

class SensorEfectoHall
{
  public:
  	SensorEfectoHall();
  	SensorEfectoHall(int pinSensor);
  	bool hayCampoPresente();
  	void setPin(int);
  private:
  	int _pinSensor;
 };
    
#endif