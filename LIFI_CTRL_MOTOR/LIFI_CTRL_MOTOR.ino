#include "Arduino.h"
#include <Stepper_28BYJ48.h>
#include <EasyTransfer.h>

#define nMotor(n)  (n-1)

// Define la cantidad de pasos a considerar radialmente
// para el buffer de un motor
#define maxPasosRadiales 20
// Define el minimo radio necesario para generar una respuesta
#define minPasosRadiales 10
// Distancia entre radios equidistantes de un motor
#define pasosSize 10
// Numero de repeticiones de la correccion de la posicion de los
// motores entre cada lectura de la posicion del usuario
#define correccionesMotor 1
// Define el step del buffer que es integrado para la posicion
// futura del motor
#define stepIntegracion 5
// Factor de la distacia recorrida por el usuario para acentuar
// o disminuir el efecto sobre el movimiento
#define sensibilidadMovimiento 10.0
// Tiempo de sincronizacion del ciclo de correccion de motores
#define tiempoCorreccion 1000000

#define stepsPorCiclo           512

bool acercando; // Dark: to delete

//Corresponde a todos los motores controlador por un cada 
//arduino mega 2560, pueden ser hasta 13.
Stepper_28BYJ48 _motor[13];

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

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  float angulo;
  float distancia;
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;

//create object
EasyTransfer ET;

void setup()
{
  Serial.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  //ET.begin(details(mydata), &Serial);
  ////Serial.println("inicio");
  inicializarArrays();
  ////Serial.println("arrays Inicializados");
  inicializarMotores();
  ////Serial.println("motores Inicializados");
  acercando = true;
  posicionUsuario[0][1] = 105;
  ////Serial.println("fin setup");
  delay(1000);
  Serial.println("init ready");
}

int globalCounter = 0;
void loop()
{
  
  // for(int i=1;i<=13;i++)
  // {  
  //  _motor[nMotor(i)].moverMotorHaciaPosicion(2047,true);
  // }
  // _motor[1].delayPorStep();
  
  //Serial.println("loop");
  if(globalCounter==0){
  obtenerPosicionUsuario();
  //Serial.println("loop 1");
  for(int i=0; i<correccionesMotor; i++)
  {
    unsigned long tiempoEspera = micros();
      //Serial.println("loop 2");
    calcularPosiciones();
      //Serial.println("loop 3");
    //moverMotores();
      //Serial.println("loop 4");
    tiempoEspera = tiempoCorreccion - (micros() - tiempoEspera);
      //Serial.println("loop 5");
    String tiempoEsperaString = String (tiempoEspera);
    //Serial.println("tiempoEspera = " + tiempoEsperaString);
    //delay(tiempoEspera/1000);
      //Serial.println("loop 6");
  }
  }
  globalCounter=globalCounter % stepsPorCiclo;
  moverMotores();
  delay(2);
  globalCounter++;
  
  /*
  if(z = 0)
  {
    
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa1FullStep();
  }
  _motor[0].delayPorStep();
  }
  
  if(z = 1)
  {
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa2FullStep();
  }
  _motor[0].delayPorStep();
  }
  
  if(z = 2)
  {
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa3FullStep();
  }
  _motor[0].delayPorStep();
  }
  if(z = 3)
  {
  for(int i=1;i<=13;i++)
  {
      _motor[nMotor(i)].etapa4FullStep();
  }
  _motor[0].delayPorStep();
  }
  z++;
  z = z % 4;
  */
}

// Refresca la posicion del usuario

void obtenerPosicionUsuario()
{
  posicionUsuario[1][0] = posicionUsuario[0][0];
  posicionUsuario[1][1] = posicionUsuario[0][1];
  
  //dummy data
  posicionUsuario[0][0] = 0; // Dark: completar con theta del radar
  if(acercando)
  {
    //dummy data
    posicionUsuario[0][1] = ((posicionUsuario[0][1] + 10));
    if(posicionUsuario[0][1] >= 155)
    {
      acercando = false;
    }
  }
  else
  {
    posicionUsuario[0][1] = ((posicionUsuario[0][1] - 10));
    if(posicionUsuario[0][1] <= 105)
    {
      acercando = true;
    }
  }
  
  String theta = String(posicionUsuario[0][0]);
  String distancia = String(posicionUsuario[0][1]);
  
  for(int i=0; i<13; i++)
  {
    contadorCiclo[i] = 0;
  }
  //Serial.println("Pos = (" + theta + "," + distancia + ")");

/*
  while(!ET.receiveData())
  {
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out. 
    posicionUsuario[0][0] = mydata.angulo; // Dark: completar con theta del radar
    posicionUsuario[0][1] = mydata.distancia; // Dark: completar con distancia del radar
  }
  */
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
      if (posicionUsuario[i][j] == 0)
      {
        //valid = false;
        i = 2;
        j = 2;
      }
    }
  }
  if(valid)
  {
    // procedimiento de calculo
    int recorrido = distanciaRecorrida()*sensibilidadMovimiento;
    String recorridoString = String(recorrido);
    //Serial.println("recorridoString = "+ recorridoString);
    for(int i=1; i<=13; i++)
    {
      int distanciaFinal = distanciaPuntoFinal(nMotor(i));
      String distanciaFinalString = String(distanciaFinal);
      double variacionAngular = traslacionAngular(nMotor(i));
      String variacionAngularString = String(variacionAngular);
      int affectedStep = distanciaFinal / pasosSize;
      String affectedStepString = String(affectedStep);
      if (nMotor(i) == 4) {
        //String iString = String (i);
        //Serial.println("distanciaFinal/variacionAngular " + iString + " = "+ distanciaFinalString + " - " + variacionAngularString + " - " + affectedStep);
      }
      if(affectedStep - 1 < minPasosRadiales || affectedStep + 1 > maxPasosRadiales - 1)
      {
        // fuera de rango re respuesta
      } else {
        int movimiento = (variacionAngular > 0) ? 1 : -1; 
        String movimientoString = String(movimiento);
        movimiento *= recorrido;
        String newMovimiento = String(movimiento);
        if (nMotor(i)==4){
          //Serial.println("movimientoString = "+ movimientoString + " - " + newMovimiento);
        }
        bufferActivacion[nMotor(i)][affectedStep-1] = (bufferActivacion[i][affectedStep-1]+movimiento)/2;
        bufferActivacion[nMotor(i)][affectedStep] += movimiento;
        bufferActivacion[nMotor(i)][affectedStep+1] = (bufferActivacion[i][affectedStep+1]+movimiento)/2;
      }
    }
  } else {
    // procedimiento de stand by
  }
  for(int i=1;i<=13;i++)
  {
    for(int j=1;j<maxPasosRadiales;j++)
    {
      bufferActivacion[nMotor(i)][j-1] = bufferActivacion[nMotor(i)][j];
    }
    bufferActivacion[nMotor(i)][maxPasosRadiales-1] = 0;
  }
  for(int i=1; i<=13;  i++)
  {
    for(int j=2; j>0; j--)
    {
      registroPosicion[nMotor(i)][j] = registroPosicion[i][j-1];
    }
    if (nMotor(i)==4){
      String integra = String(bufferActivacion[nMotor(i)][stepIntegracion]);
      //Serial.println("movimientoString = " +  integra);
    }
    registroPosicion[nMotor(i)][0] += bufferActivacion[nMotor(i)][stepIntegracion];
    if(registroPosicion[nMotor(i)][0] > 2047)
    {
      registroPosicion[nMotor(i)][0] -= 2048;
    }
    if(registroPosicion[nMotor(i)][0] < 0){
      registroPosicion[nMotor(i)][0] += 2048;
    }
    if (nMotor(i)==4){
      int lala = registroPosicion[nMotor(i)][1];
      String registroPosicion = String(lala);
      String integra = String(bufferActivacion[nMotor(i)][stepIntegracion]);
      //Serial.println("mov = " + integra + " - " + registroPosicion);
    }
    //Serial.println("registroPosicion " + iString +" = " + registroPosicion);
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
  
  int dummy = posicionUsuario[1][1];
  String dummyString = String(dummy);
  //Serial.println("posicionUsuario[1][1] = "+ dummyString);
    
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
  for(int i=4;i<=4;i++)
  {
    double velocidad;
    int nStep = calcularSteps(registroPosicion[nMotor(i)][0],registroPosicion[nMotor(i)][1]);
    String nStepString = String(nStep);
    
    Serial.println("nStepString = "+ nStepString);
    
    if(nStep != 0)
    {
      velocidad = stepsPorCiclo/abs(nStep);
    }
    else
    {
      velocidad = stepsPorCiclo;
    }
    /*
    String velocidadString = String(velocidad);
    Serial.println("velocidadString = "+ velocidadString);
    */
    /*
    String globalCounterString = String(globalCounter);
    Serial.println("globalCounterString = "+ globalCounterString);
    */
    String globalCounterString = String(globalCounter);
    String contadorCicloString = String(contadorCiclo[i]);
    
    Serial.println("globalCounterString = "+ globalCounterString + "  contadorCiclo = " +contadorCicloString);
    if((int)contadorCiclo[i] == globalCounter)
    {
        _motor[nMotor(i)].moverMotorHaciaPosicion(registroPosicion[nMotor(i)][0],esSentidoHorario(i));
        contadorCiclo[i]+= velocidad;
      /*
      if(esMovimientoValido(i))
      {
        
        _motor[nMotor(i)].moverMotorHaciaPosicion(registroPosicion[nMotor(i)][0],esSentidoHorario(i));
      }
      */
    }
  }
  _motor[nMotor(0)].delayPorStep();
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
  posicionRelativaMotores[nMotor(13)][0] =   0; // Legacy
  posicionRelativaMotores[nMotor(13)][1] =   0; // Legacy
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
  
  for(int i=1;i<=13;i++)
  {
    //_motor[nMotor(i)].EncontrarCeroMotor();
  }
}

//Verifica que el movimiento que va a realizar sea un movimiento válido,
//Si el movimiento de T-1 con respecto a T-2 es en una dirección, el movimiento de T-1 y T0
//debe ser en la misma dirección, de lo contrario retorna falso.
//- idMotor, motor para el cual se realiza el cálculo

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
// - idMotor, motor para el cual se realiza el cálculo

bool esSentidoHorario(int idMotor)
{
  //Lothar usar calcularSteps
  bool toReturn = false;
  int sentidoStep = registroPosicion[nMotor(idMotor)][0] - registroPosicion[nMotor(idMotor)][1];
  if(sentidoStep < 0)
  {
    toReturn = false;
  } else {
    toReturn = true;
  }
  if(abs(sentidoStep) > 1000){
    toReturn = !toReturn;
  }
  return toReturn;
}

int calcularSteps(int posicionFinal,int posicionInicial)
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
