//Library/Dependencies
#include <DHT.h>
#include <SoftwareSerial.h>
#include <LowPower.h>
#include <string.h>

//User Define Variable
#define DHTPIN 8
#define DHTTYPE DHT22
#define RXPIN 5
#define TXPIN 4
#define RDSPIN A2
#define BAUDRATE 9600
#define SLEEP_TRESHOLD_MINUTE 1

const char FIREBASE_HOST[]="patech-16fcc-default-rtdb.asia-southeast1.firebasedatabase.app";
const char FIREBASE_AUTH[]="oWKPQnzXoqyK6oXZLPbV9srxJ3Cl34mnfTHYOxmZ";
const char FIREBASE_PATH[]="/";
const int SSL_PORT = 443;

const char APN[]="Internet";
const char usr[]="";
const char pwd[]="";


//Object Initialization
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial sim800l(RXPIN, TXPIN);
byte counter = 0, rainStat = 0, lastRainStatus = 0, clock_counter = 0;
enum rain_status {
  not_rain,
  gerimis,
  sedang,
  sangat_deras,
  deras
};

//Fuction Declarations
void sendDataToServer(){

}

byte rainDurationProcess(byte rainStatus){
  if(lastRainStatus != rainStatus){
    //send to server when rainstatus change
    
    //reset clock counter when rainstatus change after send to server
    clock_counter = 0;
  }
  else{
    lastRainStatus = rainStatus;
    ++clock_counter;
  }
  return clock_counter;
}

byte rainCondition(byte analogSignal){  
  enum rain_status status;
  if(analogSignal >= 120 && analogSignal <= 127){
    status = not_rain; 
  }
  else if(analogSignal >= 116 && analogSignal <= 119){
    status = gerimis;
  }
  else if(analogSignal >= 71 && analogSignal <= 115){
    status = sedang;
  }
  else if(analogSignal >= 36 && analogSignal <= 70){
    status = deras;
  }
  else if(analogSignal >= 0 && analogSignal <= 35){
    status = sangat_deras;
  } 
  return status;
}

void sleepInMinute(){
  float minute = SLEEP_TRESHOLD_MINUTE;
  while(counter < ceil((minute * 60)/8)){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
    ++counter;
  }
  counter = 0;
}

void readDHT(){
  float h = dht.readHumidity();
  float c = dht.readTemperature();
  float f = dht.readTemperature(true);

  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F("Temperature(C): "));
  Serial.println(c);
  Serial.print(F("Temperature(F): "));
  Serial.println(f);
}

void readRDS(){
  byte analogRDS = analogRead(RDSPIN) / 8;
  Serial.print(F("Analog RDS Value: "));
  Serial.println(analogRDS);
  Serial.println(rainCondition(analogRDS));
  rainDurationProcess(rainCondition(analogRDS));
}

void sim800lSetup(){
  bool ser = 1, sim = 1, status = false;
  if(sim800l.available()){
    Serial.write(sim800l.read());
    sim = 0;
  }
  if(Serial.available()){
    sim800l.write(Serial.read());
    ser = 0;
  }
  // (!(sim && ser)) ? status = true : status = false;
  // return status;
}

void setup(){
  Serial.begin(BAUDRATE);
  sim800l.begin(BAUDRATE);
  pinMode(RDSPIN, INPUT);
  dht.begin();  
}

void loop(){
  // sleepInMinute();
  sim800lSetup();
  delay(1000);
  readDHT();
  readRDS();
  delay(1500);
}
