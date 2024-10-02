#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

#define tiempo_muestreo   1s
#define TMP102_ADDRESS 0x90

//Pines y puertos 
BufferedSerial serial(USBTX, USBRX);
I2C i2c (D14,D15);
Adafruit_SSD1306_I2c oled (i2c, D0);
AnalogIn ain(A0);

// Variables globales 
float Vin=0.0;
int ent=0;
int dec=0;
char men[40];
char comando[3]= {0x01, 0x60,0xA0};
char data[2];

const char *mensaje_inicio = "Arranque del programa\n\r";

void Iniciar_OLED() {    
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
    oled.clearDisplay();
    oled.display();
    oled.printf("Test\r\n");
    oled.display();
 }

void Iniciar_Sensor() 
{
    //arranque del sensor de temperatura
     i2c.write(TMP102_ADDRESS, comando, 3);
}

void Saludo_Serial() 
{
    // saludo por el puerto serial
    serial.write(mensaje_inicio, strlen(mensaje_inicio));
}

void Leer_Potenciometro() 
{
   Vin = ain*3.3;
   ent = int(Vin); 
   dec = int((Vin-ent)*10000); 
   // Visualizacion
   sprintf(men, "El voltaje es:\n\r %01u.%04u volts \n\r", ent, dec);
   oled.clearDisplay();
   oled.display();
   // formateo de la cadena de caracteres
   oled.setTextCursor(0,2);
   oled.printf(men);
   oled.display();
   // impresion puerto serie
   serial.write(men, strlen(men));
}

void Leer_Temperatura()
{
   //Lectura sensor I2C
   // Leer el registro de temperatura
   comando[0] = 0; // Registro de temperatura
   i2c.write(TMP102_ADDRESS, comando, 1); // Enviar el comando para leer
   i2c.read(TMP102_ADDRESS, data, 2); // Leer 2 bytes
   int16_t temp = (data[0] << 4) | (data[1] >> 4);
   float Temperatura = temp *0.0625; 
   ent = int(Temperatura); 
   dec = int((Temperatura-ent)*10000); 
   sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
   oled.clearDisplay();
   oled.display();
   oled.setTextCursor(0,2);
   oled.printf(men);
   oled.display();
   // impresion puerto serie
   serial.write(men, strlen(men));
}

int main(){
   Iniciar_OLED();
   Iniciar_Sensor();
   Saludo_Serial();

   while (true) {
      Leer_Potenciometro();
      Leer_Temperatura();
        ThisThread::sleep_for(tiempo_muestreo);
    }
} 