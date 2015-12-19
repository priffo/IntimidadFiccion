#include "Arduino.h"
#include <Stepper_28BYJ48.h>

#define nMotor(n)  n-1
Stepper_28BYJ48 _motor[13];

unsigned int registroPosicion[13][3];
unsigned int pocisionUsuario[2];
int posicionRelativaMotores[13][2];

void setup()
{
  inicializarArrays();
  inicializarMotores();
}

<<<<<<< Updated upstream
void loop() {
  obtenerPosicionUsuario();
  calcularPosiciones();
  moverMotores();
}

// Refresca la posicion del usuario

void obtenerPosicionUsuario()
{
  posicionUsuario[0] = 0;
  posicionUsuario[1] = 0;
}

// Calcula las nuevas posiciones de los motores

void calcularPosiciones()
{
  
=======
void loop()
{
  moverMotores();
}

void inicializarMotores()
{
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
>>>>>>> Stashed changes
}

// Mueve los motores a la ultima ubicacion calculada en el ciclo

void moverMotores()
{
  /*
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
  */
  for(int i=1;i<=13;i++)
  {
<<<<<<< Updated upstream
    _motor[nMotor(i)].moverMotorHaciaPosicion();
  }
}

// Inicializa los arrays para comunicacion de estados

void incializarArrays()
{
  // Setea la posicion relativa de los motores a 0
  for (int i=1;i<=13;i++) {
    for (int j=0;j<3;j++) {
      posicionRelativaMotores[nMotor(i)][j] = 0;
    }
  }
  // Carga las posiciones relativas de los motores respecto al radar
  posicionRelativaMotores[nMotor(1)][0] = 0;
  posicionRelativaMotores[nMotor(1)][1] = 0;
  posicionRelativaMotores[nMotor(2)][0] = 0;
  posicionRelativaMotores[nMotor(2)][1] = 0;
  posicionRelativaMotores[nMotor(3)][0] = 0;
  posicionRelativaMotores[nMotor(3)][1] = 0;
  posicionRelativaMotores[nMotor(4)][0] = 0;
  posicionRelativaMotores[nMotor(4)][1] = 0;
  posicionRelativaMotores[nMotor(5)][0] = 0;
  posicionRelativaMotores[nMotor(5)][1] = 0;
  posicionRelativaMotores[nMotor(6)][0] = 0;
  posicionRelativaMotores[nMotor(6)][1] = 0;
  posicionRelativaMotores[nMotor(7)][0] = 0;
  posicionRelativaMotores[nMotor(7)][1] = 0;
  posicionRelativaMotores[nMotor(8)][0] = 0;
  posicionRelativaMotores[nMotor(8)][1] = 0;
  posicionRelativaMotores[nMotor(9)][0] = 0;
  posicionRelativaMotores[nMotor(9)][1] = 0;
  posicionRelativaMotores[nMotor(10)][0] = 0;
  posicionRelativaMotores[nMotor(10)][1] = 0;
  posicionRelativaMotores[nMotor(11)][0] = 0;
  posicionRelativaMotores[nMotor(11)][1] = 0;
  posicionRelativaMotores[nMotor(12)][0] = 0;
  posicionRelativaMotores[nMotor(12)][1] = 0;
  posicionRelativaMotores[nMotor(13)][0] = 0;
  posicionRelativaMotores[nMotor(13)][1] = 0;
  // Inicializa la posicion del usuario
  posicionUsuario[0] = 0;
  posicionUsuario[1] = 0;
}

// Iniciliza los motores para su uso

void inicializarMotores()
{
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
=======
    if(esMovimientoValido)
    {
      _motor[nMotor(i)].moverMotorHaciaPosicion(registroPosicion[nMotor(idMotor)][0],esSentidoHorario(idMotor));
    }
  }
}

bool esMovimientoValido(int idMotor)
{
  if((((registroPosicion[nMotor(idMotor)][0] - registroPosicion[nMotor(idMotor)][1]) < 0) 
    && ((registroPosicion[nMotor(idMotor)][1] - registroPosicion[nMotor(idMotor)][2]) < 0))|| 
    (((registroPosicion[nMotor(idMotor)][0] - registroPosicion[nMotor(idMotor)][1]) > 0) 
    && ((registroPosicion[nMotor(idMotor)][1] - registroPosicion[nMotor(idMotor)][2]) > 0)))
  {
    return true;
  }
  return false;
}

bool esSentidoHorario(int idMotor)
{
  if((registroPosicion[nMotor(idMotor)][0] - registroPosicion[nMotor(idMotor)][1]) < 0)
  {
    return false;
  }
  return true;
}

>>>>>>> Stashed changes

