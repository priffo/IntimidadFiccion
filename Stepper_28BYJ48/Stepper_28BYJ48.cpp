#include "Arduino.h"
#include "Stepper_28BYJ48.h"
#define ANCHO_PULSO_FULL_STEP  2000
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
  etapaActual = 1;
  Inicializar(PIN_FASE1,PIN_FASE2,PIN_FASE3,PIN_FASE4,tamanoStep,pinSensor);
}

void Stepper_28BYJ48::Inicializar(int PIN_FASE1,int PIN_FASE2,int PIN_FASE3,int PIN_FASE4,int tamanoStep, int pinSensor)
{
  _posicionMotorActual = 0;
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
	//Serial.println("iniciando calibracion motor");
	if(hayCampoPresente())
    {
      while(hayCampoPresente())
      {
        moverMotor(1,GIRO_SENTIDO_HORARIO);
        delayPorStep();
      }
    }
    //busca el primer punto donde se detecte el campo magnético del imán
    while(!hayCampoPresente())
    {
        moverMotor(1,GIRO_SENTIDO_ANTIHORARIO);
        delayPorStep();
    }
    int nStepsConIman = 0;
    while(hayCampoPresente())
    {
        nStepsConIman++;
        moverMotor(1,GIRO_SENTIDO_ANTIHORARIO);
        delayPorStep();
    }


    if(nStepsConIman > 1)
    {
        moverMotor(nStepsConIman/2,GIRO_SENTIDO_HORARIO);
        delayPorStep();
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
  int diferencia = _posicionMotorActual - posicion;
  if(diferencia < 0)
  {
    moverMotor(diferencia,true);
  }
  else if (diferencia > 0)
  {
    if(diferencia > 1024 )
    moverMotor(diferencia,false);
  }
  if(posicion > _posicionMotorActual )
  {
    moverMotor(_posicionMotorActual - posicion,true);
  }
  else if(posicion < _posicionMotorActual)
  {
    moverMotor(_posicionMotorActual - posicion,false);
  }
}

void Stepper_28BYJ48::moverMotor(int nSteps,bool sentidoHorario)
{

  for(int i = 0; i < abs(nSteps) ; i++)
  {
    moverUnStep(sentidoHorario,FULL_STEP);
    if(sentidoHorario)
    {
      if(_posicionMotorActual == 2047)
      {
        _posicionMotorActual = 0;
      }
      else
      {
        _posicionMotorActual++;
      }
    }
    else
    {
      if(_posicionMotorActual == 0)
      {
        _posicionMotorActual = 2047;
      }
      else
      {
        _posicionMotorActual--;
      }
    }
  }
}

void Stepper_28BYJ48::resetPosicion()
{
  _posicionMotorActual = 0;
}

void Stepper_28BYJ48::etapa1FullStep()
{
  //Serial.println("etapa 1 ");
  digitalWrite(_pinFase1,HIGH);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,LOW);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa2FullStep()
{
  //Serial.println("etapa 2 ");
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,HIGH);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,LOW);
}
void Stepper_28BYJ48::etapa3FullStep()
{
  //Serial.println("etapa 3 ");
  digitalWrite(_pinFase1,LOW);
  digitalWrite(_pinFase2,LOW);
  digitalWrite(_pinFase3,HIGH);
  digitalWrite(_pinFase4,HIGH);
}
void Stepper_28BYJ48::etapa4FullStep()
{
  //Serial.println("etapa 4 ");
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
  if(sentidoHorario)
  {
    _posicionMotorActual++;
    if(_posicionMotorActual >= 2048)
    {
      _posicionMotorActual = 0;
    }
  }
  else
  {
    _posicionMotorActual--;
    if(_posicionMotorActual<0)
    {
      _posicionMotorActual = 2047;
    }
  }

  if(tamanoStep == FULL_STEP)
  {
    switch(etapaActual)
    {
      case 1:
        //Serial.println("moverUnStep 1 ");
        etapaActual = sentidoHorario?2:4;
        etapa1FullStep();
        //sentidoHorario?:etapa4FullStep();
      break;
      case 2:
        //Serial.println("moverUnStep 2 ");  
        etapaActual = sentidoHorario?3:1;
        etapa2FullStep();
        //sentidoHorario?etapa3FullStep():etapa1FullStep();
        break;
      case 3:
        //Serial.println("moverUnStep 3 ");
        etapaActual = sentidoHorario?4:2;
        etapa3FullStep();
        //sentidoHorario?etapa4FullStep():etapa2FullStep();
      break;
      case 4:
        //Serial.println("moverUnStep 4 ");
        etapaActual = sentidoHorario?1:3;
        etapa4FullStep();
        //sentidoHorario?etapa1FullStep():etapa3FullStep();
      break;
      default:
        //Serial.println("moverUnStep default ");
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
        break;
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
  if(_posicionMotorActual != posicionFinal)
  {
    moverUnStep(sentidoHorario,FULL_STEP);
  }
  else
  {
    //Serial.println("posicion igual");
  }
}