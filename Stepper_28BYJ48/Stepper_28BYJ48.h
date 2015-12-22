/*
  Stepper_28BYJ48.h - Librería para manejar motor 28BYJ-48 con driver ULN2003.
  Creado por Pedro Riffo. 19/09/2015 Santiago, Chile.
  Para el proyecto Fondart: La intimidad de la ficción(LIFI).
*/
#ifndef Stepper_28BYJ48_h
#define Stepper_28BYJ48_h

#define VUELTAS_POR_GIRO_SHAFT			64
#define STEPS_POR_GIRO_ROTOR_FULLSTEP	32
#define STEPS_POR_GIRO_ROTOR_HALFSTEP	64

 //Tiene una presición de 0.18º en FULL STEP
#define STEPS_POR_GIRO_SHAFT_FULLSTEP	STEPS_POR_GIRO_ROTOR_FULLSTEP * VUELTAS_POR_GIRO_SHAFT

//Tiene una presición de 0.09º en FULL STEP
#define STEPS_POR_GIRO_SHAFT_FHALFSTEP	STEPS_POR_GIRO_ROTOR_HALFSTEP * VUELTAS_POR_GIRO_SHAFT

#define GIRO_SENTIDO_HORARIO			true
#define GIRO_SENTIDO_ANTIHORARIO		false
#define FULL_STEP 						0
#define HALF_STEP 						1

#include "Arduino.h"
#include "../SensorEfectoHall/SensorEfectoHall.h"


class Stepper_28BYJ48
{
public:
	Stepper_28BYJ48();
	Stepper_28BYJ48(int PIN_FASE1,int PIN_FASE2,int PIN_FASE3,int PIN_FASE4,int tamanoStep, int pinSensor);
	void Inicializar(int PIN_FASE1,int PIN_FASE2,int PIN_FASE3,int PIN_FASE4,int tamanoStep, int pinSensor);
	void moverMotor(int nSteps,bool sentidoHorario);
	int getPosicion();
	void resetPosicion();
	void irAPosicion(int posicion);
	void EncontrarCeroMotor(int);
	void EncontrarCeroMotor();
	void moverMotorHaciaPosicion(int posicionFinal, bool sentidoHorario);

	void delayPorStep();
	
	void etapa1FullStep();
	void etapa2FullStep();
	void etapa3FullStep();
	void etapa4FullStep();
	
	void etapa1HalfStep();
	void etapa2HalfStep();
	void etapa3HalfStep();
	void etapa4HalfStep();
	void etapa5HalfStep();
	void etapa6HalfStep();
	void etapa7HalfStep();
	void etapa8HalfStep();

private:
    int  etapaActual;
	void moverMotorUnStep(bool sentidoHorario);
	void moverUnStep(bool sentidoHorario,int tamanoStep);

	
	void apagarMotor();
	bool hayCampoPresente();
  	void setPin(int);

	int _pinFase1;
	int _pinFase2;
	int _pinFase3;
	int _pinFase4;
	int _tamanoStep;

	int _posicionMotorActual;

	int _pinSensor;
};
#endif