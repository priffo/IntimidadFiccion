//
// LIFI
//
// FONDART: La intimidad de la ficción


#include "Arduino.h"
#include <Stepper_28BYJ48.h>
#include <LidarPWM.h>
#include <SensorEfectoHall.h>
#include <EasyTransfer.h>

#define N_INTENTOS_MAXIMO               20
#define ERROR                           -1
#define STEPS_POR_POSICION              8
#define N_POSICIONES                    2048/STEPS_POR_POSICION

#define LIDAR_INT_PWM_BAJA_RES          0 //INT 0 -> PIN2 (LIDAR_PIN_MONITOR_PWM_BAJA_RES)
#define LIDAR_INT_PWM_ALTA_RES          1 //INT 1 -> PIN3 (LIDAR_PIN_MONITOR_PWM_ALTA_RES)

#define LIDAR_PIN_MONITOR_PWM_BAJA_RES  2
#define LIDAR_PIN_MONITOR_PWM_ALTA_RES  3

#define SENSOR_MAGNETICO_PIN_BAJA_RES   8
#define SENSOR_MAGNETICO_PIN_ALTA_RES   9

//TODO DEFINIR PINES CORRECTOS
#define PIN_FASE1_MOTOR_ALTA            4
#define PIN_FASE2_MOTOR_ALTA            5
#define PIN_FASE3_MOTOR_ALTA            6
#define PIN_FASE4_MOTOR_ALTA            7

//TODO DEFINIR PINES CORRECTOS
#define PIN_FASE1_MOTOR_BAJA            10
#define PIN_FASE2_MOTOR_BAJA            11
#define PIN_FASE3_MOTOR_BAJA            12
#define PIN_FASE4_MOTOR_BAJA            13
#define ANGULO_POR_STEP                 0.18
#define posicion2Angulo(a)             a*ANGULO_POR_STEP
//#define posicion2ArrayPos(b)           b+MOTOR_POSICION_MAXIMA-1

//create object
EasyTransfer ET;



struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  float angulo;
  int distanciaCM;
};

struct posicionPolar{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  float angulo;
  int distanciaCM;
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;


Stepper_28BYJ48 motorBaja(PIN_FASE1_MOTOR_BAJA,PIN_FASE2_MOTOR_BAJA,PIN_FASE3_MOTOR_BAJA,PIN_FASE4_MOTOR_BAJA,FULL_STEP,SENSOR_MAGNETICO_PIN_BAJA_RES);
Stepper_28BYJ48 motorAlta(PIN_FASE1_MOTOR_ALTA,PIN_FASE2_MOTOR_ALTA,PIN_FASE3_MOTOR_ALTA,PIN_FASE4_MOTOR_ALTA,FULL_STEP,SENSOR_MAGNETICO_PIN_ALTA_RES);

int _sentidoGiroMotorBajaRes = GIRO_SENTIDO_HORARIO;
int _sentidoGiroMotorAltaRes = GIRO_SENTIDO_HORARIO;

long _distanciaLidarAlta;
long _distanciaLidarBaja;

int _mapAnguloDistancia[N_POSICIONES];
int _mapDiferencias[N_POSICIONES];
int _mayorDiferenciaPorVuelta;
int _posicionMayorDiferencia;


bool sentidoGiro = false;

void setup()
{   
    Serial.begin(9600); // Start serial communication
    ET.begin(details(mydata), &Serial);
    motorBaja.EncontrarCeroMotor();
    motorAlta.EncontrarCeroMotor();
    
    LidarPWM::instance().init(LIDAR_PIN_MONITOR_PWM_ALTA_RES,LIDAR_INT_PWM_ALTA_RES,LIDAR_PIN_MONITOR_PWM_BAJA_RES,LIDAR_INT_PWM_BAJA_RES);
                              
    LidarPWM::instance().habilitarLecturaAlta();
    LidarPWM::instance().habilitarLecturaBaja();

    //mapearEntorno(motorBaja);
    motorAlta.moverMotor(1024,true);
      
    
    
}



void loop()
{ /*  
  motorBaja.moverUnStep(true,FULL_STEP);
        motorAlta.moverUnStep(true,FULL_STEP);
        motorBaja.delayPorStep();
        */
   
    _distanciaLidarAlta = LidarPWM::instance().calcularDistanciaAlta();
    if(_distanciaLidarAlta > 0)
    {
        //String distanciaAltaString = String(_distanciaLidarAlta);
        //Serial.println("alta = "+ distanciaAltaString);
        struct posicionPolar;
        mydata.angulo = 0;
        mydata.distanciaCM = _distanciaLidarAlta;
        //send the data
        ET.sendData();
        LidarPWM::instance().habilitarLecturaAlta();
    }
    
    _distanciaLidarBaja = LidarPWM::instance().calcularDistanciaBaja();
    if(_distanciaLidarBaja > 0)
    {
        //String distanciaBajaString = String(_distanciaLidarBaja);
        //Serial.println("baja = "+ distanciaBajaString);
        LidarPWM::instance().habilitarLecturaBaja();
    }
    
    delay(1000);
    
}

void mapearEntorno(Stepper_28BYJ48 stepper)
{
    stepper.irAPosicion(0);
    
    for(int i=0;i<N_POSICIONES;i++)
    {
        _distanciaLidarBaja= LidarPWM::instance().calcularDistanciaBaja();
        
        if(_distanciaLidarBaja > 0)
        {
            //_mapAnguloDistancia[posicion2ArrayPos(i)] = _distanciaLidarBaja;
            //stepper.irAPosicion(i*STEPS_POR_POSICION);
            //LidarPWM::instance().habilitarLecturaBaja();
        } 
    }    
}

void buscarDiferencias()
{
    _distanciaLidarBaja= LidarPWM::instance().calcularDistanciaBaja();
    if(_distanciaLidarBaja > 0)
    {
      
      _mapDiferencias[motorBaja.getPosicion()] = abs(_mapAnguloDistancia[motorBaja.getPosicion()] - _distanciaLidarBaja);
      _mapAnguloDistancia[motorBaja.getPosicion()] = _distanciaLidarBaja;
      if(_mayorDiferenciaPorVuelta < _mapDiferencias[motorBaja.getPosicion()])
      {
       _mayorDiferenciaPorVuelta = _mapDiferencias[motorBaja.getPosicion()];
       _posicionMayorDiferencia = motorBaja.getPosicion();
      }

      LidarPWM::instance().habilitarLecturaBaja();
    }
}

void irADiferencia()
{
    motorAlta.irAPosicion(_posicionMayorDiferencia);
}

int buscarBordeIzquierda(int distanciaInicial)
{
    int nIntentos = 0;
    while(true)
    {
        _distanciaLidarAlta= LidarPWM::instance().calcularDistanciaAlta();
        if(_distanciaLidarAlta > 0)
        {
            if(abs(distanciaInicial - _distanciaLidarAlta) > 20)
            {
                return motorBaja.getPosicion();
            }
            if(nIntentos > N_INTENTOS_MAXIMO)
            {
                return ERROR;
            }
            motorAlta.moverMotor(1,GIRO_SENTIDO_HORARIO);
            motorAlta.delayPorStep();
        }
    }
    return ERROR;
}

int buscarBordeDerecha(int distanciaInicial)
{
    LidarPWM::instance().habilitarLecturaBaja();
    int nIntentos = 0;
    while(true)
    {
        _distanciaLidarAlta= LidarPWM::instance().calcularDistanciaAlta();
        if(_distanciaLidarAlta > 0)
        {
            if(abs(distanciaInicial - _distanciaLidarAlta) > 20)
            {
                return motorBaja.getPosicion();
            }
            if(nIntentos > N_INTENTOS_MAXIMO)
            {
                return ERROR;
            }
            motorAlta.moverMotor(1,GIRO_SENTIDO_HORARIO);
            motorAlta.delayPorStep();
            LidarPWM::instance().habilitarLecturaBaja();
        }
    }
    return ERROR;
}

struct posicionPolar calcularCentroMasaPersona()
{

    int  posicionBordeDerecha = buscarBordeDerecha(0);
    if(posicionBordeDerecha >= 0)
    {
        int diferenciaBordeIzquierda = buscarBordeIzquierda(posicionBordeDerecha);
        
        if( diferenciaBordeIzquierda >= 0)
        {
            int posicionCentroMasa = (posicionBordeDerecha + diferenciaBordeIzquierda/2);
            posicionCentroMasa= posicionCentroMasa % 2048;
            motorAlta.irAPosicion(posicionCentroMasa);
            _distanciaLidarAlta = 0;
            LidarPWM::instance().habilitarLecturaBaja();
            while(_distanciaLidarAlta <= 0)
            {
                _distanciaLidarAlta= LidarPWM::instance().calcularDistanciaAlta();
            }
            struct posicionPolar posicion;
            posicion.angulo = posicionCentroMasa * 0,17578125;
            posicion.distanciaCM = _distanciaLidarAlta;
        }
    }
}

int enviarPosicion()
{
    struct posicionPolar pos = calcularCentroMasaPersona();
    //Serial.println("angulo = %f , distancia = %d",angulo,distanciaCM);
}
