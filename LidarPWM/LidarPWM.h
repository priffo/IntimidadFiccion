/*
  Lidar_PWM.h - Librería para usar LIDAR-Lite usando PWM.
  Creado por Pedro Riffo. 13/09/2015 Santiago, Chile.
  Para el proyecto Fondart: La intimidad de la ficción(LIFI).
*/
#ifndef LidarPWM_h
#define LidarPWM_h

#include "Arduino.h"

class LidarPWM
{
  public:
    
    long calcularDistanciaBaja();
    long calcularDistanciaAlta();
    void habilitarLecturaAlta();
    void habilitarLecturaBaja();
    void init(int MONITOR_PIN_ALTA,int INTERRUPT_ALTA,int MONITOR_PIN_BAJA,int INTERRUPT_BAJA);
    static LidarPWM& instance();
    static void handle_isrAlta();
    static void handle_isrBaja();

  private:
    LidarPWM();
    LidarPWM(LidarPWM const&);// Don't Implement
    LidarPWM operator=(LidarPWM const&); // Don't implement
    int _monitorPinAlta;
    int _interruptAlta;
    int _nLecturasAlta;
    bool _nuevaLecturaAlta;
    long _distanciaCMAlta;
    volatile unsigned long _tiempoInicialAlta;
    volatile unsigned long _anchoPulsoAlta;

    int _monitorPinBaja;
    int _interruptBaja;
    int _nLecturasBaja;
    bool _nuevaLecturaBaja;
    long _distanciaCMBaja;
    volatile unsigned long _tiempoInicialBaja;
    volatile unsigned long _anchoPulsoBaja;
    
    
  protected:
    void interrupcionAlta();
    void interrupcionBaja();
    

};

#endif