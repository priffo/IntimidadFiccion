#include "Arduino.h"
#include <Stepper_28BYJ48.h>
#include <EasyTransfer.h>

#define nMotor(n) (n-1)

// Define el ID del arduino en uso para calcular las posiciones
// de los motores
#define arduinoID 2
// Define la cantidad de pasos a considerar radialmente
// para el buffer de un motor
#define maxPasosRadiales 5
// Define el minimo radio necesario para generar una respuesta
#define minPasosRadiales 0
// Distancia entre radios equidistantes de un motor
#define pasosSize 10
// Define el step del buffer que es integrado para la posicion
// futura del motor
#define stepIntegracion 5
// Factor de la distacia recorrida por el usuario para acentuar
// o disminuir el efecto sobre el movimiento
#define sensibilidadMovimiento 5.0
// Cantidad de steps de motor entre calculos de posicion
#define stepsPorCiclo 150
//Corresponde a todos los motores controlador por un cada 
//arduino mega 2560, pueden ser hasta 13.
Stepper_28BYJ48 _motor[13];
// Contador para controlar la velocidad de los motores
double contadorCiclo[13];
// Registro de posicion de los motores:
// - id del motor
// - instante de tiempo (0= t-0, 1= t-1, 2= t-2)
int registroPosicion[13][3];
// Registro de la posicion del usuario
// - instante de tiempo (0= t-0, 1= t-1)
float posicionUsuario[2][2];
// Posiciones relativs de cada morot respecto al radar
// - id del motor
// - distancia cartesiana relativa al radar (0= x, 1= y)
int posicionRelativaMotores[13][2];
// Buffer para activacion de motores
// - id del motor
// - distancia radial en steps de la activacion futura
int bufferActivacion[13][maxPasosRadiales];

EasyTransfer ET;
// Estructura para el dato del radar
struct RECEIVE_DATA_STRUCTURE{
  float angulo;
  int distanciaCM;
};
// Dato del radar
RECEIVE_DATA_STRUCTURE mydata;
// Objeto para la validacion de los datos del radar

// Contador global para el ciclo interno de movimiento de motor
int globalCounter = 0;
// Velocidades de rotacion
double velocidad[13];

bool acercando;

void setup()
{
  Serial.begin(9600); // Comunicacion serial
  ET.begin(details(mydata), &Serial);
  inicializarArrays();
  inicializarMotores();
  acercando = true;            // Simulacion del radar
  posicionUsuario[0][1] = 105; // Simulacion del radar
  //Serial.println("Iniciado");
}

void loop()
{
  if(globalCounter == 0)
  {
    //Serial.println("loop");
    obtenerPosicionUsuario();
    calcularPosiciones();
  }
  moverMotores();
  globalCounter++;
  globalCounter = globalCounter % stepsPorCiclo;
  delayMicroseconds(1500);
  
}

// Refresca la posicion del usuario

void obtenerPosicionUsuario()
{
  posicionUsuario[1][0] = posicionUsuario[0][0];
  posicionUsuario[1][1] = posicionUsuario[0][1];
  
  
  while(ET.receiveData())
  {
    //String anguloString = String(mydata.angulo);
    //String distanciaString = String(mydata.distanciaCM);
    //Serial.println("angulo = " + anguloString + " distancia = " + distanciaString);
    posicionUsuario[0][0] = mydata.angulo;
    posicionUsuario[0][1] = mydata.distanciaCM;
    //Serial.println("Data: " + String(posicionUsuario[0][0]) + " - " + String(posicionUsuario[0][1]));
    //Serial.println(".");
  } 
  
  ////////////////////////////////////////////////////////////
  // Simulador de radar
  /*
  posicionUsuario[0][0] = 0;
  if(acercando)
  {
    posicionUsuario[0][1] = posicionUsuario[0][1] + 17;
    if(posicionUsuario[0][1] >= 200)
    {
      acercando = false;
    }
  }
  else
  {
    posicionUsuario[0][1] = posicionUsuario[0][1] - 17;
    if(posicionUsuario[0][1] <= 100)
    {
      acercando = true;
    }
  }
  */
  ////////////////////////////////////////////////////////////
  
  for(int i=1; i<=13; i++)
  {
    contadorCiclo[nMotor(i)] = 0;
  }
}

// Calcula las nuevas posiciones de los motores

void calcularPosiciones()
{
  int recorrido = distanciaRecorrida()*sensibilidadMovimiento;
  for(int i=1;i<=13;i++)
  {
    int distanciaFinal = distanciaPuntoFinal(i);
    double variacionAngular = traslacionAngular(i);
    int affectedStep = 5; // distanciaFinal / pasosSize;
    //if(affectedStep < minPasosRadiales || affectedStep > maxPasosRadiales - 1)
    //{
    //  // fuera de rango re respuesta
    //} else {
      int movimiento = (variacionAngular > 0) ? -1 : 1; 
      movimiento *= recorrido;
      bufferActivacion[nMotor(i)][affectedStep-0] += movimiento;
      bufferActivacion[nMotor(i)][affectedStep-1] += movimiento;
      bufferActivacion[nMotor(i)][affectedStep-2] += movimiento;
    //}
    for(int j=1;j<maxPasosRadiales;j++)
    {
      bufferActivacion[nMotor(i)][j-1] = bufferActivacion[nMotor(i)][j];
    }
    bufferActivacion[nMotor(i)][maxPasosRadiales-1] = bufferActivacion[nMotor(i)][maxPasosRadiales-2]/1.5;
  }
  for(int i=1;i<=13;i++)
  {
    for(int j=2;j>0;j--)
    {
      registroPosicion[nMotor(i)][j] = registroPosicion[nMotor(i)][j-1];
    }
    int toIntegrate = 0;
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-0];
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-1];
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-2];
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-3];
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-4];
    toIntegrate += bufferActivacion[nMotor(i)][stepIntegracion-5];
    while(abs(toIntegrate) > stepsPorCiclo)
    {
      toIntegrate /= 2;
    }
    registroPosicion[nMotor(i)][0] += toIntegrate;
    if(registroPosicion[nMotor(i)][0] > 2047)
    {
      registroPosicion[nMotor(i)][0] -= 2048;
    }
    if(registroPosicion[nMotor(i)][0] < 0)
    {
      registroPosicion[nMotor(i)][0] += 2048;
    }
    int nStep = calcularSteps(registroPosicion[nMotor(i)][0], registroPosicion[nMotor(i)][1]);
    //if(i==1){
    //  Serial.println("corr: " + String(registroPosicion[nMotor(i)][0]));
    //}
    if(nStep != 0)
    {
      velocidad[nMotor(i)] = (double)stepsPorCiclo;
      velocidad[nMotor(i)] /= (double) abs(nStep);
    }
    else
    {
      velocidad[nMotor(i)] = stepsPorCiclo;
    }
  }
}

// Calcula la distancia en cm a la posicion final

int distanciaPuntoFinal(int idMotor)
{
  int xComp = cos(posicionUsuario[0][0])*posicionUsuario[0][1] - posicionRelativaMotores[nMotor(idMotor)][0];
  int yComp = sin(posicionUsuario[0][0])*posicionUsuario[0][1] - posicionRelativaMotores[nMotor(idMotor)][1];
  return sqrt(xComp*xComp + yComp*yComp);
}

// Calcula la distancia recorrida entre los 2 ultimos instantes

int distanciaRecorrida()
{
  int distancia = 0;
  distancia += posicionUsuario[0][1]*posicionUsuario[0][1];
  distancia += posicionUsuario[1][1]*posicionUsuario[1][1];   
  distancia -= 2*posicionUsuario[0][1]*posicionUsuario[1][1]*cos(posicionUsuario[0][0])*cos(posicionUsuario[1][0]);
  distancia -= 2*posicionUsuario[0][1]*posicionUsuario[1][1]*sin(posicionUsuario[0][0])*sin(posicionUsuario[1][0]);
  return sqrt(distancia);
}

// Calcula la traslacion angular del ultimo movimiento

double traslacionAngular(int idMotor)
{
  double num = sin(posicionUsuario[0][0]) * posicionUsuario[0][1] - posicionRelativaMotores[nMotor(idMotor)][1];
  double den = cos(posicionUsuario[0][0]) * posicionUsuario[0][1] - posicionRelativaMotores[nMotor(idMotor)][0];
  double theta = atan(num/den) * 180 / 3.14159;
  num = sin(posicionUsuario[1][0]) * posicionUsuario[1][1] - posicionRelativaMotores[nMotor(idMotor)][1];
  den = cos(posicionUsuario[1][0]) * posicionUsuario[1][1] - posicionRelativaMotores[nMotor(idMotor)][0];
  theta -= atan(num/den) * 180 / 3.14159;
  return theta;
}

// Mueve los motores a la ultima ubicacion calculada en el ciclo
void moverMotores()
{
  for(int i=1;i<=13;i++)
  {
    if((int)contadorCiclo[nMotor(i)] == globalCounter)
    {
       _motor[nMotor(i)].moverMotorHaciaPosicion(registroPosicion[nMotor(i)][0],esSentidoHorario(i));
       contadorCiclo[nMotor(i)] += velocidad[nMotor(i)];
    }
  }
}

// Inicializa los arrays para comunicacion de estados

void inicializarArrays()
{
  // Carga las posiciones relativas de los motores respecto al radar
  for(int i=1;i<=13;i++)
  {
    posicionRelativaMotores[nMotor(i)][0] = 0;
    posicionRelativaMotores[nMotor(i)][1] = 0;
  }
  // Arduino I
  if(arduinoID == 1)
  {
    posicionRelativaMotores[nMotor(1)][0]  = -27;
    posicionRelativaMotores[nMotor(1)][1]  = -31;
    posicionRelativaMotores[nMotor(2)][0]  = -27;
    posicionRelativaMotores[nMotor(2)][1]  = -18;
    posicionRelativaMotores[nMotor(3)][0]  = -27;
    posicionRelativaMotores[nMotor(3)][1]  =  -5;
    posicionRelativaMotores[nMotor(4)][0]  = -26;
    posicionRelativaMotores[nMotor(4)][1]  =   9;
    posicionRelativaMotores[nMotor(5)][0]  = -27;
    posicionRelativaMotores[nMotor(5)][1]  =  20;
    posicionRelativaMotores[nMotor(6)][0]  = -39;
    posicionRelativaMotores[nMotor(6)][1]  = -38;
    posicionRelativaMotores[nMotor(7)][0]  = -39;
    posicionRelativaMotores[nMotor(7)][1]  = -25;
    posicionRelativaMotores[nMotor(8)][0]  = -39;
    posicionRelativaMotores[nMotor(8)][1]  = -13;
    posicionRelativaMotores[nMotor(9)][0]  = -39;
    posicionRelativaMotores[nMotor(9)][1]  =   1;
    posicionRelativaMotores[nMotor(10)][0] = -39;
    posicionRelativaMotores[nMotor(10)][1] =  14;
    posicionRelativaMotores[nMotor(11)][0] = -38;
    posicionRelativaMotores[nMotor(11)][1] =  28;
    posicionRelativaMotores[nMotor(12)][0] = -39;
    posicionRelativaMotores[nMotor(12)][1] =  40;
  }
  // Arduino II
  if(arduinoID == 2)
  {
    posicionRelativaMotores[nMotor(1)][0]  =  21;
    posicionRelativaMotores[nMotor(1)][1]  = -18;
    posicionRelativaMotores[nMotor(2)][0]  =   9;
    posicionRelativaMotores[nMotor(2)][1]  = -24;
    posicionRelativaMotores[nMotor(3)][0]  =  -2;
    posicionRelativaMotores[nMotor(3)][1]  = -30;
    posicionRelativaMotores[nMotor(4)][0]  = -13;
    posicionRelativaMotores[nMotor(4)][1]  = -37;
    posicionRelativaMotores[nMotor(5)][0]  = -24;
    posicionRelativaMotores[nMotor(5)][1]  = -44;
    posicionRelativaMotores[nMotor(6)][0]  = -39;
    posicionRelativaMotores[nMotor(6)][1]  = -51;
    posicionRelativaMotores[nMotor(7)][0]  =  22;
    posicionRelativaMotores[nMotor(7)][1]  = -31;
    posicionRelativaMotores[nMotor(8)][0]  =  10;
    posicionRelativaMotores[nMotor(8)][1]  = -38;
    posicionRelativaMotores[nMotor(9)][0]  =  -2;
    posicionRelativaMotores[nMotor(9)][1]  = -44;
    posicionRelativaMotores[nMotor(10)][0] = -14;
    posicionRelativaMotores[nMotor(10)][1] = -50;
    posicionRelativaMotores[nMotor(11)][0] = -23;
    posicionRelativaMotores[nMotor(11)][1] = -57;
    posicionRelativaMotores[nMotor(12)][0] = -33;
    posicionRelativaMotores[nMotor(12)][1] = -65;
  }
  // Arduino III
  if(arduinoID == 3)
  {
    posicionRelativaMotores[nMotor(1)][0]  =  21;
    posicionRelativaMotores[nMotor(1)][1]  =  18;
    posicionRelativaMotores[nMotor(2)][0]  =  33;
    posicionRelativaMotores[nMotor(2)][1]  =  12;
    posicionRelativaMotores[nMotor(4)][0]  =  43;
    posicionRelativaMotores[nMotor(4)][1]  =   5;
    posicionRelativaMotores[nMotor(5)][0]  =  32;
    posicionRelativaMotores[nMotor(5)][1]  = -12;
    posicionRelativaMotores[nMotor(6)][0]  =  32;
    posicionRelativaMotores[nMotor(6)][1]  =  26;
    posicionRelativaMotores[nMotor(7)][0]  =  44;
    posicionRelativaMotores[nMotor(7)][1]  =  19;
    posicionRelativaMotores[nMotor(8)][0]  =  55;
    posicionRelativaMotores[nMotor(8)][1]  =  11;
    posicionRelativaMotores[nMotor(9)][0]  =  66;
    posicionRelativaMotores[nMotor(9)][1]  =   5;
    posicionRelativaMotores[nMotor(10)][0] =  66;
    posicionRelativaMotores[nMotor(10)][1] =  -8;
    posicionRelativaMotores[nMotor(11)][0] =  55;
    posicionRelativaMotores[nMotor(11)][1] = -13;
    posicionRelativaMotores[nMotor(12)][0] =  45;
    posicionRelativaMotores[nMotor(12)][1] = -18;
    posicionRelativaMotores[nMotor(13)][0] =  33;
    posicionRelativaMotores[nMotor(13)][1] = -25;
  }
  // Arduino IV
  if(arduinoID == 4)
  {
    posicionRelativaMotores[nMotor(1)][0]  = -38;
    posicionRelativaMotores[nMotor(1)][1]  =  54;
    posicionRelativaMotores[nMotor(2)][0]  = -23;
    posicionRelativaMotores[nMotor(2)][1]  =  44;
    posicionRelativaMotores[nMotor(3)][0]  = -27;
    posicionRelativaMotores[nMotor(3)][1]  =  34;
    posicionRelativaMotores[nMotor(4)][0]  = -12;
    posicionRelativaMotores[nMotor(4)][1]  =  37;
    posicionRelativaMotores[nMotor(5)][0]  =  -1;
    posicionRelativaMotores[nMotor(5)][1]  =  31;
    posicionRelativaMotores[nMotor(6)][0]  =  15;
    posicionRelativaMotores[nMotor(6)][1]  =  22;
    posicionRelativaMotores[nMotor(7)][0]  = -34;
    posicionRelativaMotores[nMotor(7)][1]  =  64;
    posicionRelativaMotores[nMotor(8)][0]  = -23;
    posicionRelativaMotores[nMotor(8)][1]  =  58;
    posicionRelativaMotores[nMotor(9)][0]  = -11;
    posicionRelativaMotores[nMotor(9)][1]  =  51;
    posicionRelativaMotores[nMotor(10)][0] =  -1;
    posicionRelativaMotores[nMotor(10)][1] =  45;
    posicionRelativaMotores[nMotor(11)][0] =   9;
    posicionRelativaMotores[nMotor(11)][1] =  39;
    posicionRelativaMotores[nMotor(12)][0] =  21;
    posicionRelativaMotores[nMotor(12)][1] =  32;
  }
  // Inicializa la posicion del usuario
  posicionUsuario[0][0] = 0;
  posicionUsuario[0][1] = 0;
  posicionUsuario[1][0] = 0;
  posicionUsuario[1][1] = 0;
  // Inicializa el bufer de activacion
  for(int i=1;i<=13;i++)
  {
    for(int j=0; j<maxPasosRadiales; j++)
    {
      bufferActivacion[nMotor(i)][j] = 0;
    }
  }
}

// Inicializa los motores para su uso

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
  //for(int i=1;i<=13;i++)
  //{
  //  _motor[nMotor(i)].EncontrarCeroMotor();
  //}
}

//Verifica que el movimiento que va a realizar sea un movimiento valido
//Si el movimiento de t-1 con respecto a t-2 es en una direccion, el movimiento de t-1 y t-0
// debe ser en la misma direccion, de lo contrario retorna falso
// - idMotor motor para el cual se realiza el calculo

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

//Determina el sentido de giro para un motor
// - idMotor id del motor para el cual se realiza el calculo

bool esSentidoHorario(int idMotor)
{
  int sentidoStep = calcularSteps(registroPosicion[nMotor(idMotor)][0], registroPosicion[nMotor(idMotor)][1]);
  if(sentidoStep < 0)
  {
    return false;
  } 
  else 
  {
    return true;
  }
}

// Calcula la diferencia de steps entre dos posiciones
// - Posicion de donde termina el movimiento
// - Posicion de donde parte el movimiento

int calcularSteps(int posicionFinal, int posicionInicial)
{
  int sentidoStep = posicionFinal - posicionInicial;
  if(sentidoStep > 1000)
  {
    sentidoStep-=2048;
  }
  else if (sentidoStep < -1000)
  {
    sentidoStep+=2048;
  }
  return sentidoStep;
}

