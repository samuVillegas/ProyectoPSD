#include <ThingSpeak.h>
#include <ESP8266WiFi.h> // Incluye la librería ESP8266wifi

#define timeSeconds 1
//LCD 16*2 I2C Display libraries
#include <Wire.h>               
#include <LiquidCrystal_I2C.h>    
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//Variables de conexion al servidor
// ----- Cambiar XXXXX por el nombre de la red wifi o datos a la que se va a conectar
char ssid[] = "UNE_HFC_7BB0"; // Nombre del WiFi (nombre del router)
// ----- Cambiar YYYYY por la contraseña de la red wifi o datos a la que se va a conectar 
char pass[] = "AAA0BF87"; // WiFi router password
// ----- Cambiar 123456 por el ID de su canal en ThingSpeak
unsigned long myChannelNumber = 1004093; // Thingspeak número de canal
// ----- Cambiar ZZZZZZZZZZ por la API Key de su canal en ThingSpeak
const char * myWriteAPIKey = "9O4JAFFUIFE01Y8O"; // ThingSpeak write API Key

int status = WL_IDLE_STATUS; 
WiFiClient client;

// Set GPIOs for LED and PIR Motion Sensor
const int motionSensor = 14;
double tiempo=0;
double ultimo=0;
float factor_conversion=7.11;
float caudal=0;
int tiempoPrSeg = 0;
int tiempoPrMin = 0;
int tiempoPrH = 0;
int tiempoFnSeg = 0;
// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int pulsos=0;
int guardar=0;
float volumen=0;
//pines buzzer y sensor ir
int pinbuzer = 16;
int pinInfr = 2;
// Checks if motion was detected, sets LED HIGH and starts a timer
int  estado;
ICACHE_RAM_ATTR void detectsMovement() {
  pulsos=pulsos+1;
  tiempo = tiempo + 0.001;
}
void setup() {
    WiFi.begin(ssid, pass); // Inicia la conexion WiFi 
    ThingSpeak.begin(client); // Inicia ThingSpeak
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(pinbuzer,OUTPUT);
  pinMode(pinInfr,INPUT);
  lcd.begin(16,2);// Cols and rows of the I2C Dsp
  //lcd.init();
  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING); 
  //Conexion al servidor
   Serial.println();
   Serial.print("Conectando a ");
   Serial.print(ssid);

   while (WiFi.status() != WL_CONNECTED)
   {
     delay(500);
     Serial.print(".");
   }

   Serial.println();
   Serial.println("Conectado a WiFi");
   Serial.print("Dirección IP: ");
   Serial.println(WiFi.localIP());
}
  void loop() {
     estado = digitalRead(pinInfr);
    Serial.println(estado);
   if(estado == 1){
     delay(1500);
     do{
       delay(150);
       proceso();
       estado = digitalRead(pinInfr);
       Serial.println(estado);
      }while(estado == 0);
      enviar();
      reinicio();
     }
  }
  void proceso(){
      // Current time
   now = millis();
   // Turn off the LED after the number of seconds defined in the timeSeconds variable
   if(now - lastTrigger > (timeSeconds*1000)) {
     lastTrigger=now;
     guardar= pulsos;
     pulsos=0;
     caudal = guardar/factor_conversion;
     volumen=volumen+caudal/60;

    if(caudal != 0){
       tiempoD();
       char tpM[12];
       sprintf(tpM,"%02d%c%02d%c%02d",tiempoPrH,':', tiempoPrMin,':',tiempoPrSeg);
       Serial.println(tpM);
       //Use the display to print vol & time
       lcd.setCursor(0,0); // Inicio del cursor
       lcd.print("L. GASTO : ");
       lcd.print(volumen);
       lcd.setCursor(0,1); 
       lcd.print("TIEMPO:");
       lcd.print(tpM);
       if(volumen > 10){
         digitalWrite(pinbuzer,HIGH);
       }
       else{
         digitalWrite(pinbuzer,LOW);
       }
    }
   }
  }
  void tiempoD(){
    tiempoPrSeg++;
    tiempoFnSeg++;
    if(tiempoPrSeg == 60){
       tiempoPrSeg = 0;
       tiempoPrMin++;
    }
    if(tiempoPrMin == 60){
      tiempoPrMin = 0;
      tiempoPrH++;
    }
    delay(1000);
  }
  void reinicio(){
    tiempoPrSeg = 0;
    tiempoPrMin = 0;
    tiempoPrH = 0;
    tiempoFnSeg = 0;
    lcd.clear();
  }
  void enviar(){
    Serial.print("Ultimo dato enviado =");
     /* char tpM[10];
      sprintf(tpM,"%02d%c%02d%c%02d",tiempoPrH,':', tiempoPrMin,':',tiempoPrSeg);*/
        ThingSpeak.setField(1,volumen);
        ThingSpeak.setField(2,tiempoFnSeg);
       // Escribe todos los campos a la vez.
      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      Serial.println(tiempoFnSeg);
      Serial.println(volumen); 
      digitalWrite(pinbuzer,LOW);
      delay(2000);
  }









































































































    
