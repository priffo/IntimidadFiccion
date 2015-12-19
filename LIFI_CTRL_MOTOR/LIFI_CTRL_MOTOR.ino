#include "Arduino.h"
#include <Stepper_28BYJ48.h>

#define nMotor(n)  n-1

// Define la cantidad de pasos a considerar radialmente
// para el buffer de un motor
#define maxPasosRadiales 20
// Define el minimo radio necesario para generar una respuesta
#degine minPasosRadiales 10
// Distancia entre radios equidistantes de un motor
#define pasosSize 10

// Dark: agregar descripcion de esta instanciacion
Stepper_28BYJ48 _motor[13];

// Registro de posicion de los motores:
// - id del motor
// - instante de tiempo (0= t-0, 1= t-1, 2= t-2)
unsigned int registroPosicion[13][3];

// Registro de la posicion del usuario
// - instante de tiempo (0= t-0, 1= t-1)
unsigned int posicionUsuario[2][2];

// Posiciones relativs de cada morot respecto al radar
// - id del motor
// - distancia cartesiana relativa al radar (0= x, 1= y)
int posicionRelativaMotores[13][2];

// Buffer para activacion de motores
// - id del motor
// - distancia radial en steps de la activacion futura
int bufferActivacion[13][maxPasosRadiales]

void setup()
{
  inicializarArrays();
  inicializarMotores();
}

void loop() {
  obtenerPosicionUsuario();
  calcularPosiciones();
  moverMotores();
}

// Refresca la posicion del usuario

void obtenerPosicionUsuario()
{
  posicionUsuario[1][0] = posicionUsuario[0][0];
  posicionUsuario[1][1] = posicionUsuario[0][1];
  posicionUsuario[0][0] = 0; // Dark: completar con theta del radar
  posicionUsuario[0][1] = 0; // Dark: completar con distancia del radar
}

// Calcula las nuevas posiciones de los motores

void calcularPosiciones()
{
  // Checking si radar mide 0,0
  bool valid = true;
  for(int i=0; i<2; i++)
  {
    for (int j=0; j<2; j++)
    {
      if (posicionUsuario(i][j] == 0)
      {
        valid = false;
        i = 2;
        j = 2;
      }
    }
  }
  if(valid)
  {
    // procedimiento de calculo
    int distanciaRecorrida = distanciaRecorrida();
    for(int i=1; i<=13; i++)
    {
      int distanciaFinal = distanciaPuntoFinal(i);
      int traslacionAngular = traslacionAngular(i);
      int affectedStep = distanciaFinal / pasosSize;
      if(affectedStep - 1 < minPasosRadiales || affectedStep + 1 > maxPasosRadiales - 1)
      {
        // fuera de rango re respuesta
      } else {
        int movimiento = (traslacionAngular > 0)? 1 : -1;
        movimiento *= distanciaRecorrida;
        bufferActivacion[i][affectedStep-1] = (bufferActivacion[i][affectedStep-1]+movimiento)/2
        bufferActivacion[i][affectedStep] = movimiento;
        bufferActivacion[i][affectedStep+1] = (bufferActivacion[i][affectedStep+1]+movimiento)/2
      }
    }
  } else {
    // procedimiento de stand by
  }
  for(int i=0;i<13;i++)
  {
    for(int j=1;j<maxPasosRadiales;j++)
    {
      bufferActivacion[i][j-1] = bufferActivacion[i][j];
    }
  }
}

// Calcula la distancia en cm a la posicion final

int distanciaPuntoFinal(int idMotor)
{
  int xComp = cos(posicionUsuario[0][0])*posicionUsuarop[0][1] - posicionRelativaMotores[nMotor(idMotor)][0];
  int yComp = sin(posicionUsuario[0][0])*posicionUsuario[0][1] - posicionRelativaMotores[nMotor(idMotor)][1];
  return sqrt(xComp*xCom + yComp*yComp);
}

// Calcula la distancia recorrida entre los 2 últimos instantes

int distanciaRecorrida()
{
  int distancia = 0;
  distancia += posicionUsuario[0][1]*posicionUsuario[0][1];
  distancia -= posicionUsuario[1][1]*posicionUsuario[1][1];
  distancia -= 2*posicionUsuario[0][1]*posicionUsuario[1][1]*cos(posicionUsuario[0][0]*cos(posicionUsuario[1][0];
  distancia -= 2*posicionUsuario[0][1]*posicionUsuario[1][1]*sin(posicionUsuario[0][0]*sin(posicionUsuario[1][0];
  return sqrt(distancia);
}

// Calcula la traslacion angular del ultimo movimiento

int traslacionAngular(int idMotor)
{
  int den = sin(posicionUsuario[0][0] - posicionRelativaMotores[nMotor(idMotor)][1];
  int num = cos(posicionUsuario[0][0] - posicionRelativaMotores[nMotor(idMotor)][0];
  int theta = arctan(den/num);
  den = sin(posicionUsuario[1][0] - posicionRelativaMotores[nMotor(idMotor)][1];
  num = cos(posicionUsuario[1][0] - posicionRelativaMotores[nMotor(idMotor)][0];
  theta -= arctan(den/num);
  return theta;
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
    if(esMovimientoValido)
    {
      _motor[nMotor(i)].moverMotorHaciaPosicion(registroPosicion[nMotor(i)][0],esSentidoHorario(i));
    }
  }
}

// Inicializa los arrays para comunicacion de estados

void inicializarArrays()
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
  posicionUsuario[0][0] = 0;
  posicionUsuario[0][1] = 0;
  posicionUsuario[1][0] = 0;
  posicionUsuario[1][1] = 0;
  // Inicializa el bufer de activacion
  for(int i=0; i<13; i++)
  {
    for(int j=0; j<maxPasosRadiales; j++)
    {
      bufferActivacion[i][j] = 0;
    }
  }
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

// Dark: Agregar descripcion de esta funcion

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

// Dark: agregar descripcion de esta funcion

bool esSentidoHorario(int idMotor)
{
  if((registroPosicion[nMotor(idMotor)][0] - registroPosicion[nMotor(idMotor)][1]) < 0)
  {
    return false;
  }
  return true;
}

