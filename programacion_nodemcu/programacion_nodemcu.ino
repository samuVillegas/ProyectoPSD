#define timeSeconds 1

// Set GPIOs for LED and PIR Motion Sensor
const int motionSensor = 14;
double tiempo=0;
double ultimo=0;
float factor_conversion=7.11;
float caudal=0;
int tiempoPrSeg = 0;
int tiempoPrMin = 0;
int tiempoFnMin = 0;
int tiempoFnSeg = 0;
// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int pulsos=0;
int guardar=0;
float volumen=0;
// Checks if motion was detected, sets LED HIGH and starts a timer
ICACHE_RAM_ATTR void detectsMovement() {
  pulsos=pulsos+1;
  tiempo = tiempo + 0.001;
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}
void loop() {
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
      Serial.print("Caudal: ");
      Serial.print(caudal);
      Serial.println(" L/m");
      Serial.print("Volumen: ");
      Serial.print(volumen);
      Serial.println(" L");
      /*Serial.printf(tiempoPrMin);
      Serial.print(":");
      Serial.printf(tiempoPrSeg);
      Serial.println(" Min");*/
      char tpM[10];
      sprintf(tpM,"%02d%c%02d%s", tiempoPrMin,':',tiempoPrSeg," Min");
      Serial.println(tpM);
    }
   }
  }
  void tiempoD(){
    tiempoPrSeg++;
    if(tiempoPrSeg == 60){
       tiempoPrSeg = 0;
       tiempoPrMin++;
    }
    delay(1000);
  }
