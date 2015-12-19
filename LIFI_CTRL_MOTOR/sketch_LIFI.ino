//
// LIFI
//
// FONDART: La intimidad de la ficción


#include "Arduino.h"
#include <Stepper_28BYJ48.h>
#include <LidarPWM.h>
#include <SensorEfectoHall.h>

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
#define posicion2ArrayPos(b)           b+MOTOR_POSICION_MAXIMA-1


Stepper_28BYJ48 motorBaja(PIN_FASE1_MOTOR_BAJA,PIN_FASE2_MOTOR_BAJA,PIN_FASE3_MOTOR_BAJA,PIN_FASE4_MOTOR_BAJA,FULL_STEP,SENSOR_MAGNETICO_PIN_BAJA_RES);
Stepper_28BYJ48 motorAlta(PIN_FASE1_MOTOR_ALTA,PIN_FASE2_MOTOR_ALTA,PIN_FASE3_MOTOR_ALTA,PIN_FASE4_MOTOR_ALTA,FULL_STEP,SENSOR_MAGNETICO_PIN_ALTA_RES);

int _sentidoGiroMotorBajaRes = GIRO_SENTIDO_HORARIO;
int _sentidoGiroMotorAltaRes = GIRO_SENTIDO_HORARIO;

long _distanciaLidarAlta;
long _distanciaLidarBaja;

int _mapAnguloDistancia[2048/8+1];

bool sentidoGiro = false;

void setup()
{   
    Serial.begin(9600); // Start serial communication
    motorBaja.EncontrarCeroMotor();
    motorAlta.EncontrarCeroMotor();
    
    LidarPWM::instance().init(LIDAR_PIN_MONITOR_PWM_ALTA_RES,LIDAR_INT_PWM_ALTA_RES,LIDAR_PIN_MONITOR_PWM_BAJA_RES,LIDAR_INT_PWM_BAJA_RES);
                              
    LidarPWM::instance().habilitarLecturaAlta();
    LidarPWM::instance().habilitarLecturaBaja();

    mapearEntorno(motorBaja);
    
}



void loop()
{   
    _distanciaLidarBaja= LidarPWM::instance().calcularDistanciaBaja();
    if(_distanciaLidarBaja > 0)
    {
      LidarPWM::instance().habilitarLecturaBaja();
    }
    
    _distanciaLidarAlta = LidarPWM::instance().calcularDistanciaAlta();
    if(_distanciaLidarAlta > 0)
    {
      LidarPWM::instance().habilitarLecturaAlta();
    }
    
    delay(100);    
}

void mapearEntorno(Stepper_28BYJ48 stepper)
{
    stepper.irAPosicion(0);
    
    for(int i=0;i<2048/8;i++)
    {
        _distanciaLidarBaja= LidarPWM::instance().calcularDistanciaBaja();
        
        if(_distanciaLidarBaja > 0)
        {
            _mapAnguloDistancia[posicion2ArrayPos(i)] = _distanciaLidarBaja;
            stepper.irAPosicion(i);
            LidarPWM::instance().habilitarLecturaBaja();
        } 
    }    
}

