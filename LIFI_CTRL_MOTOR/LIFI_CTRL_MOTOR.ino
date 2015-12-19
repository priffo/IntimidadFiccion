#include "Arduino.h"
#include <Stepper_28BYJ48.h>


#define nMotor(n)  n-1
Stepper_28BYJ48 _motor[13];



void setup()
{
  inicializarMotores();
}

void loop() {
  moverMotores();

  

}

void inicializarMotores()
{
  //11,12,13,4,5,6
  _motor[nMotor(1)].Inicializar(A3,A2,A1,A0,FULL_STEP,A4);
  _motor[nMotor(2)].Inicializar(A8,A7,A6,A5,FULL_STEP,A9);
  _motor[nMotor(3)].Inicializar(A13,A12,A11,A10,FULL_STEP,A14);
  _motor[nMotor(4)].Inicializar(47,49,51,53,FULL_STEP,45);
  _motor[nMotor(5)].Inicializar(37,39,41,43,FULL_STEP,35);
  _motor[nMotor(6)].Inicializar(27,29,31,33,FULL_STEP,25);
  _motor[nMotor(7)].Inicializar(2,14,15,16,FULL_STEP,3);
  _motor[nMotor(8)].Inicializar(7,6,5,4,FULL_STEP,8);
  _motor[nMotor(9)].Inicializar(12,11,10,9,FULL_STEP,13);
  _motor[nMotor(10)].Inicializar(20,19,18,17,FULL_STEP,21);
  _motor[nMotor(11)].Inicializar(30,28,26,24,FULL_STEP,32);
  _motor[nMotor(12)].Inicializar(40,38,36,34,FULL_STEP,42);
  _motor[nMotor(13)].Inicializar(50,48,46,44,FULL_STEP,52);

  
  
  
  
  
  
  
  
}

void moverMotores()
{
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa1FullStep();
  }
  _motor[0].delayPorStep();
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa2FullStep();
  }
  _motor[0].delayPorStep();
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa3FullStep();
  }
  _motor[0].delayPorStep();
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa4FullStep();
  }
  _motor[0].delayPorStep();
  
}
