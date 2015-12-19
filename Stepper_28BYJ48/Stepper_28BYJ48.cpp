#include "Arduino.h"
#include "Stepper_28BYJ48.h"
#define ANCHO_PULSO_FULL_STEP  3000
#define ANCHO_PULSO_HALF_STEP  1000
#define MOTOR_POSICION_MAXIMA  256
#define MOTOR_POSICION_MINIMA  -1*MOTOR_POSICION_MAXIMA + 1
#define CANTIDAD_MOVIMIENTOS_MAXIMA	10000

Stepper_28BYJ48::Stepper_28BYJ48()
{
  //Serial.begin(9600);
}

Stepper_28BYJ48::Stepper_28BYJ48(int PIN_FASE1,int PIN_FASE2,int PIN_FASE3,int PIN_FASE4,int tamanoStep, int pinSensor)
{
  Inicializar(PIN_FASE1,PIN_FASE2,PIN_FASE3,PIN_FASE4,tamanoStep,pinSensor);
}

void Stepper_28BYJ48::Inicializar(int PIN_FASE1,int PIN_FASE2,int PIN_FASE3,int PIN_FASE4,int tamanoStep, int pinSensor)
{
  _posicionMotor = 0;
  _tamanoStep = tamanoStep;
  _pinFase1 = PIN_FASE1;
  _pinFase2 = PIN_FASE2;
  _pinFase3 = PIN_FASE3;
  _pinFase4 = PIN_FASE4;
    //configura pines del motor como salida
    pinMode(_pinFase1, OUTPUT);
    pinMode(_pinFase2, OUTPUT);
    pinMode(_pinFase3, OUTPUT);
    pinMode(_pinFase4, OUTPUT);

    setPin(pinSensor);

    //apaga el motor
    apagarMotor();
  
}

void Stepper_28BYJ48::EncontrarCeroMotor(int offSet)
{
	int countMovimientosRealizados = 0;
	Serial.println("iniciando calibracion motor");
	if(hayCampoPresente())
    {
      while(hayCampoPresente())
      {
        moverMotor(1,GIRO_SENTIDO_HORARIO);
      }
    }

    //busca el primer punto donde se detecte el campo magnético del imán
    while(!hayCampoPresente())
    {
        moverMotor(1,GIRO_SENTIDO_ANTIHORARIO);
    }
    int nStepsConIman = 0;
    while(hayCampoPresente())
    {
        nStepsConIman++;
        moverMotor(1,GIRO_SENTIDO_ANTIHORARIO);
    }


    if(nStepsConIman > 1)
    {
        moverMotor(nStepsConIman/2,GIRO_SENTIDO_HORARIO);
    }

	//irAPosicion(offSet);
	
    //irAPosicion(MOTOR_POSICION_MAXIMA + offSet);

    resetPosicion();
}

void Stepper_28BYJ48::EncontrarCeroMotor()
{
    EncontrarCeroMotor(0);    
}

void Stepper_28BYJ48::apagarMotor()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);

}

void Stepper_28BYJ48::irAPosicion(int posicion)
{
  if(posicion > _posicionMotor)
  {
    moverMotor(_posicionMotor - posicion,true);
  }
  else if(posicion < _posicionMotor)
  {
    moverMotor(_posicionMotor - posicion,false);
  }
}

void Stepper_28BYJ48::moverMotor(int nSteps,bool sentidoHorario)
{

  for(int i = 0; i < abs(nSteps) ; i++)
  {

    moverMotorUnStep(sentidoHorario);
    sentidoHorario?_posicionMotor++:_posicionMotor--;
  }
}

void Stepper_28BYJ48::moverMotorUnStep(bool sentidoHorario)
{
  if(_tamanoStep == FULL_STEP)
  {
    if(sentidoHorario)
    {
      etapa1FullStep();
      delayPorStep();
      etapa2FullStep();
      delayPorStep();
      etapa3FullStep();
      delayPorStep();
      etapa4FullStep();
      delayPorStep();
    }
    else
    {
      etapa4FullStep();
      delayPorStep();
      etapa3FullStep();
      delayPorStep();
      etapa2FullStep();
      delayPorStep();
      etapa1FullStep();
      delayPorStep();
    }
  }

  if(_tamanoStep == HALF_STEP)
  {
    if(sentidoHorario)
    {
      etapa1HalfStep();
      delayPorStep();
      etapa2HalfStep();
      delayPorStep();
      etapa3HalfStep();
      delayPorStep();
      etapa4HalfStep();
      delayPorStep();
      etapa5HalfStep();
      delayPorStep();
      etapa6HalfStep();
      delayPorStep();
      etapa7HalfStep();
      delayPorStep();
      etapa8HalfStep();
      delayPorStep();
    }
    else
    {
      etapa8HalfStep();
      delayPorStep();
      etapa7HalfStep();
      delayPorStep();
      etapa6HalfStep();
      delayPorStep();
      etapa5HalfStep();
      delayPorStep();
      etapa4HalfStep();
      delayPorStep();
      etapa3HalfStep();
      delayPorStep();
      etapa2HalfStep();
      delayPorStep();
      etapa1HalfStep();
      delayPorStep();
    }
  }
}
void Stepper_28BYJ48::resetPosicion()
{
  _posicionMotor = 0;
}

void Stepper_28BYJ48::etapa1FullStep()
{
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa2FullStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa3FullStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,HIGH);
}
void Stepper_28BYJ48::etapa4FullStep()
{
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,HIGH);
}

void Stepper_28BYJ48::etapa1HalfStep()
{
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa2HalfStep()
{
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa3HalfStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa4HalfStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,LOW);
}

void Stepper_28BYJ48::etapa5HalfStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,LOW);
}

void Stepper_28BYJ48::etapa6HalfStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,HIGH);
}

void Stepper_28BYJ48::etapa7HalfStep()
{
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,HIGH);
}

void Stepper_28BYJ48::etapa8HalfStep()
{
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,HIGH);
}
void Stepper_28BYJ48::delayPorStep()
{
  if(_tamanoStep == FULL_STEP)
  {
    delayMicroseconds(ANCHO_PULSO_FULL_STEP);
  }
  else
  {
    delayMicroseconds(ANCHO_PULSO_HALF_STEP);
  }
}

void Stepper_28BYJ48::setPin(int pinSensor)
{
  _pinSensor = pinSensor;
  pinMode(_pinSensor, INPUT);
}

bool Stepper_28BYJ48::hayCampoPresente()
{
  return !digitalRead(_pinSensor);
}

void Stepper_28BYJ48::moverUnStep(bool sentidoHorario,int tamanoStep)
{
  static int etapaActual = 1;
  if(tamanoStep == FULL_STEP)
  {
    switch(etapaActual)
    {
      case 1:
        etapaActual = sentidoHorario?2:4;
        sentidoHorario?etapa2FullStep():etapa4FullStep();
      break;
      case 2:
        etapaActual = sentidoHorario?3:1;
        sentidoHorario?etapa3FullStep():etapa1FullStep();
      case 3:
        etapaActual = sentidoHorario?4:2;
        sentidoHorario?etapa4FullStep():etapa2FullStep();
      break;
      case 4:
        etapaActual = sentidoHorario?4:2;
        sentidoHorario?etapa1FullStep():etapa3FullStep();
      break;
      default:
        etapaActual = 1;
        etapa1FullStep();
      break;
    }
  }
  else
  {
    switch(etapaActual)
    {
      case 1:
        etapaActual = sentidoHorario?2:8;
        sentidoHorario?etapa2HalfStep():etapa8HalfStep();
      break;
      case 2:
        etapaActual = sentidoHorario?3:1;
        sentidoHorario?etapa3HalfStep():etapa1HalfStep();
      case 3:
        etapaActual = sentidoHorario?4:2;
        sentidoHorario?etapa4HalfStep():etapa2HalfStep();
      break;
      case 4:
        etapaActual = sentidoHorario?5:3;
        sentidoHorario?etapa5HalfStep():etapa3HalfStep();
      break;
      case 5:
        etapaActual = sentidoHorario?6:4;
        sentidoHorario?etapa6HalfStep():etapa4HalfStep();
      break;
      case 6:
        etapaActual = sentidoHorario?7:5;
        sentidoHorario?etapa7HalfStep():etapa5HalfStep();
      case 7:
        etapaActual = sentidoHorario?8:6;
        sentidoHorario?etapa8HalfStep():etapa6HalfStep();
      break;
      case 8:
        etapaActual = sentidoHorario?1:7;
        sentidoHorario?etapa1HalfStep():etapa7HalfStep();
      break;
      default:
        etapaActual = 1;
        etapa1HalfStep();
      break;
    }
  }
}

void Stepper_28BYJ48::moverMotorHaciaPosicion(int posicionFinal, bool sentidoHorario)
{
  if(_posicionMotor != posicionFinal)
  {
    moverUnStep(sentidoHorario,FULL_STEP);
  }
}