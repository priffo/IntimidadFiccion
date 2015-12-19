#include "LidarPWM.h"
#include "Arduino.h"

LidarPWM& LidarPWM::instance()
{
    static LidarPWM instance; //this will call your constructor on the first time it is run (once and only once)
    return instance; //return a reference (the & sign) your object instance
}

void LidarPWM::handle_isrAlta()
{
    LidarPWM::instance().interrupcionAlta();// get the singleton instance and call the method.
}

void LidarPWM::handle_isrBaja()
{
    LidarPWM::instance().interrupcionBaja();// get the singleton instance and call the method.
}

LidarPWM::LidarPWM(){}

void LidarPWM::init(int MONITOR_PIN_ALTA,int INTERRUPT_ALTA,int MONITOR_PIN_BAJA,int INTERRUPT_BAJA)
{
	_monitorPinAlta = MONITOR_PIN_ALTA;
	_interruptAlta = INTERRUPT_ALTA;
	_nLecturasAlta = 0;
	_tiempoInicialAlta = 0;
	_anchoPulsoAlta = 0;
	_distanciaCMAlta = 0;
	_nuevaLecturaAlta= false;

	_monitorPinBaja = MONITOR_PIN_BAJA;
	_interruptBaja = INTERRUPT_BAJA;
	_nLecturasBaja = 0;
	_tiempoInicialBaja = 0;
	_anchoPulsoBaja = 0;
	_distanciaCMBaja = 0;
	_nuevaLecturaBaja = false;

	//configura pines Lidar baja res
    pinMode(_monitorPinAlta, INPUT);

	//configura pines Lidar baja res
    pinMode(_monitorPinBaja, INPUT);
    
}

void LidarPWM::interrupcionAlta()
{
	if (digitalRead(_monitorPinAlta) == HIGH) //Or i need to digitalRead it first?
	{
	     _tiempoInicialAlta = micros(); //get time of pulse going down
	}
	else if(_tiempoInicialAlta != 0)
	{
     	_anchoPulsoAlta = micros() - _tiempoInicialAlta;
     	_nLecturasAlta++;
     	if(_nLecturasAlta > 3)
 		{
			detachInterrupt(_interruptAlta);
			_tiempoInicialAlta = 0;
			_nuevaLecturaAlta = true;
		}
	}
}
void LidarPWM::interrupcionBaja()
{
	if (digitalRead(_monitorPinBaja) == HIGH) //Or i need to digitalRead it first?
	{
	     _tiempoInicialBaja = micros(); //get time of pulse going down
	}
	else if(_tiempoInicialBaja != 0)
	{
     	_anchoPulsoBaja = micros() - _tiempoInicialBaja;
     	_nLecturasBaja++;
     	if(_nLecturasBaja > 3)
 		{
			detachInterrupt(_interruptBaja);
			_tiempoInicialBaja = 0;
			_nuevaLecturaBaja = true;
		}
	}
}

long LidarPWM::calcularDistanciaAlta()
{
	//Serial.println("_nuevaLecturaAlta");
	if(_nuevaLecturaAlta)
	{
		if(_anchoPulsoAlta/10 > 30)
		{
            // 1ms/m medición de distancia LIDAR PWM
            // 10usec = 1 cm of distance for LIDAR-Lite
            // Se detectó de forma empírica que hay un offset de 30 us (aproximadamente)
            // en las mediciones, por eso la resta
            
            //se transforma la medición a centímetros
            _distanciaCMAlta = _anchoPulsoAlta/10 - 30;
        }

        //resetea para iniciar una nueva medición
        _nuevaLecturaAlta = false;
        _nLecturasAlta = 0;

        _tiempoInicialAlta = 0;
        _anchoPulsoAlta = 0;

        long distanciaReporte = _distanciaCMAlta;
        _distanciaCMAlta = 0;
               
        return distanciaReporte;
        
    }
    return 0;
}

long LidarPWM::calcularDistanciaBaja()
{
	static long lecturaAnterior = 0;
	if(_nuevaLecturaBaja)
	{
		if(_anchoPulsoBaja/10 > 30)
		{
            // 1ms/m medición de distancia LIDAR PWM
            // 10usec = 1 cm of distance for LIDAR-Lite
            // Se detectó de forma empírica que hay un offset de 30 us (aproximadamente)
            // en las mediciones, por eso la resta
            
            //se transforma la medición a centímetros
            _distanciaCMBaja = _anchoPulsoBaja/10 - 30;

            if(abs(lecturaAnterior - _distanciaCMBaja) > 10)
            {
            	lecturaAnterior = _distanciaCMBaja;
            	return 0;
            }
            
            lecturaAnterior = _distanciaCMBaja;
        }

        //resetea para iniciar una nueva medición
        _nuevaLecturaBaja = false;
        _nLecturasBaja = 0;

        long distanciaReporte = _distanciaCMBaja;
        _distanciaCMBaja = 0;
               
        return distanciaReporte;
        
    }
    return 0;
}

void LidarPWM::habilitarLecturaAlta()
{
	//habilita interrupcion deteccion de cambios en el pin
    attachInterrupt(_interruptAlta, handle_isrAlta, CHANGE);
}

void LidarPWM::habilitarLecturaBaja()
{
	//habilita interrupcion deteccion de cambios en el pin
    attachInterrupt(_interruptBaja, handle_isrBaja, CHANGE);
}