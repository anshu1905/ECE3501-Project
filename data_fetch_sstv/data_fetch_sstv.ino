//Temperature And Humidity Sensor 
#include <dht.h>
#define dht_apin 11 // DHT connected to D1
dht DHT;
float humidity;//humidity variable 
float temperature ;//temperature variable 
int gasVal;
int rainVal;

//Rain Sensor 
#define rain_digital 2

//Gas Sensor   
#define gas_digital 3

void setup() {
  Serial.begin(9600); 
  pinMode(rain_digital,INPUT);
  pinMode(gas_digital, INPUT);
}

void loop() 
{
  DHT.read11(dht_apin);
  
  float humidity = DHT.humidity;
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.print("\t");
  delay(100);
  
  float temperature = DHT.temperature;
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("\t");
  delay(100);
  
  int rainVal = digitalRead(rain_digital);
  Serial.print("Rain");
  Serial.print("0");
  Serial.print("\t");
  delay(100);
  
  int gasVal = digitalRead(gas_digital);
  Serial.print("Gas:");
  Serial.println(gasVal);
  delay(1000);
}
