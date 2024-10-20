#include "DHT.h"
#include <Wire.h>
#include "Adafruit_SGP40.h"

Adafruit_SGP40 sgp;
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define POMP_MOSFET D0
#define WATER_SENSOR A0
#define WATER_LEVEL_THRESHOLD 500

struct AgroSystemState {
  int soilMoisture;
  float temperature;
  float humidity;
  uint16_t airQuality;
};

AgroSystemState agroSystemState;


/*
* To increase the autonomy of the device should be considered turning the power of the sensor on before reading from it.
*/
void setup() {
  Serial.begin(9600);

  while (!Serial) { delay(10); }

  pinMode(POMP_MOSFET, OUTPUT);
  
  dht.begin();

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
}

void loop() {
  delay(2000);
  readDHT();
  // readWaterLevel();
  // readAirQuality();
  displayMeasurements();

}


void readAirQuality(){
  uint16_t sraw;
  sraw = sgp.measureRaw(agroSystemState.temperature, agroSystemState.humidity);
  agroSystemState.airQuality = sgp.measureVocIndex(agroSystemState.temperature, agroSystemState.humidity);
}


void readWaterLevel(){
  //int value from 0 - 1024
  agroSystemState.soilMoisture = analogRead(WATER_SENSOR);

  if (agroSystemState.soilMoisture < WATER_LEVEL_THRESHOLD){
    digitalWrite(POMP_MOSFET, HIGH);
  }  else {
    digitalWrite(POMP_MOSFET, LOW);
  }
}

void displayMeasurements(){
  Serial.print(F("Humidity: "));
  Serial.println(agroSystemState.humidity);

  Serial.print(F("Temperature: "));
  Serial.println(agroSystemState.temperature);

  Serial.print(F("Soil moisture: "));
  Serial.println(agroSystemState.soilMoisture);

  Serial.print("Voc Index: ");
  Serial.println(agroSystemState.airQuality);

}

void readDHT() {
  agroSystemState.humidity = NAN;
  agroSystemState.temperature = NAN;

  agroSystemState.humidity = dht.readHumidity();
  agroSystemState.temperature = dht.readTemperature();
  if (isnan(agroSystemState.humidity) || isnan(agroSystemState.temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}
