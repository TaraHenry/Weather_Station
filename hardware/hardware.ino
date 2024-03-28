//##################################################################################################################
//##                                      ELET2415 DATA ACQUISITION SYSTEM CODE                                   ##
//##                                                                                                              ##
//##################################################################################################################

// LIBRARY IMPORTS
#include <rom/rtc.h> 
#include <math.h>  // https://www.tutorialspoint.com/c_standard_library/math_h.htm 
#include <ctype.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"

//############### IMPORT HEADER FILES ##################

#ifndef FORECAST_H
#include "foreCast.h"
#endif

#ifndef _WIFI_H 
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

 
// DEFINE AND INITIALISE VARIABLES
#define ARDUINOJSON_USE_DOUBLE      1 

// DEFINE THE CONTROL PINS FOR SOIL MOISTURE SENSOR
#define soil_m    33

// DEFINE THE CONTROL PINS FOR TFT DISPLAY
#define TFT_DC    17
#define TFT_CS    5
#define TFT_RST   16
#define TFT_CLK   18
#define TFT_MOSI  23
#define TFT_MISO  19

// IMPORT FONTS FOR TFT DISPLAY
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h> 

// DEFINE COLOURS FOR TFT
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// DEFINE THE CONTROL PINS FOR THE DHT22 
#define DHTPIN 25
#define DHTTYPE DHT22

// DEFINE THE CONTROL PINS FOR THE BMP280
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)


// MQTT CLIENT CONFIG  
static const char* pubtopic      = "620154033";                    // Add your ID number here
static const char* subtopic[]    = {"620154033_sub","/elet2415"};  // Array of Topics(Strings) to subscribe to
static const char* mqtt_server   = "www.yanacreations.com";         // Broker IP address or Domain name as a String

static uint16_t mqtt_port        = 1883;

// WIFI CREDENTIALS
const char* ssid       = "MonaConnect";     // Add your Wi-Fi ssid
const char* password   = "";    // Add your Wi-Fi password 

// TASK HANDLES 
TaskHandle_t xMQTT_Connect          = NULL; 
TaskHandle_t xNTPHandle             = NULL;  
TaskHandle_t xLOOPHandle            = NULL;  
TaskHandle_t xUpdateHandle          = NULL;
TaskHandle_t xButtonCheckeHandle    = NULL;   

// FUNCTION DECLARATION   
void checkHEAP(const char* Name);   // RETURN REMAINING HEAP SIZE FOR A TASK
void initMQTT(void);                // CONFIG AND INITIALIZE MQTT PROTOCOL
unsigned long getTimeStamp(void);   // GET 10 DIGIT TIMESTAMP FOR CURRENT TIME
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload); // PUBLISH MQTT MESSAGE(PAYLOAD) TO A TOPIC
void vButtonCheck( void * pvParameters );
void vUpdate( void * pvParameters );
 

/* Declare your functions below */ 
void display();
double calcHeatIndex(double t, double h);
double calcSoilValue();

#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

/* Init class Instances for the DHT22 etcc */
DHT dht(DHTPIN, DHTTYPE);  

double h;   //humidity read by the DHT sensor
double t;   //temperature read by the DHT sensor
double bt;  //temperature read by the BMP sensor
int conVal; //value of the soil moisture sensor readings after conversion
int soilVal;//value of soil moisture before conversion
double hI;  //heat index calculated using temperature and humidity

/* Initialize class objects*/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);  // INIT SERIAL   

  // INITIALIZE ALL SENSORS AND DEVICES
  /* TFT DISPLAY SET UP */
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);

  /* Add all other necessary sensor Initializations and Configurations here */
  dht.begin();
  bmp.begin(0x76);
  Wire.begin(21, 22); 
  pinMode(DHTPIN, INPUT);
  pinMode(soil_m, INPUT);
  initialize();     // INIT WIFI, MQTT & NTP 
  // vButtonCheckFunction(); // UNCOMMENT IF USING BUTTONS INT THIS LAB, THEN ADD LOGIC FOR INTERFACING WITH BUTTONS IN THE vButtonCheck FUNCTION
}
  


void loop() {
  // put your main code here, to run repeatedly:
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println("***************My Weather Station****************");
  t = dht.readTemperature();
  Serial.print(F("DHT Temperature: "));
  Serial.print(t);
  Serial.println(" *C");

  Serial.print(F("BMP Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  h = dht.readHumidity();
  Serial.print("Humidity =  ");
  Serial.print(h);
  Serial.println("%");
          
  hI = calcHeatIndex(t, h);
  Serial.print("Heat Index =  ");
  Serial.println(hI);

  conVal = calcSoilValue();
  Serial.print(F("Soil Moisture = "));
  Serial.print(conVal);
  Serial.println("%");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure()/100);
  Serial.println(" hPa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");

  Serial.println("************************************************");
  delay(2000);
  display();

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}


//####################################################################
//#                          UTIL FUNCTIONS                          #       
//####################################################################
// void vButtonCheck( void * pvParameters )  {
//     configASSERT( ( ( uint32_t ) pvParameters ) == 1 );     
      
//     for( ;; ) {
//         // Add code here to check if a button(S) is pressed
//         // then execute appropriate function if a button is pressed 
//         vTaskDelay(200 / portTICK_PERIOD_MS);  
//     }
// }

void vUpdate( void * pvParameters )  {
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );    
           
    for( ;; ) {
      // #######################################################
      // ## This function must PUBLISH to topic every second. ##
      // #######################################################
   
      double h = dht.readHumidity();
      double t = dht.readTemperature();
      double bt = bmp.readTemperature();
      double p = bmp.readPressure();
      double a = bmp.readAltitude(1013.25);

      // 1. Create JSon object
      StaticJsonDocument<1000> doc;
      // 2. Create message buffer/array to store serialized JSON object
      char message[1100]  = { 0 };
      // 3. Add key:value pairs to JSon object based on above schema
      doc["id"]                 = "620154033";
      doc["timestamp"]          = getTimeStamp();
      doc["celsTemperature"]    = t;
      doc["fahrTemperature"]    = (t * 9.0/5.0) + 32;
      doc["bmp_temp"]           = bt;
      doc["humidity"]           = h;
      doc["heatindex"]          = calcHeatIndex(t,h);
      doc["soilMoisture"]       = calcSoilValue();
      doc["pressure"]           = p;
      doc["altitude"]           = a;

      // 4. Seralize / Covert JSon object to JSon string and store in message array
      serializeJson(doc, message); 
      Serial.println(message);
      // 5. Publish message to a topic subscribed to by both backend and frontend                
      if(mqtt.connected()){
        publish(pubtopic, message);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS); 
    }  
     
}

unsigned long getTimeStamp(void) {
          // RETURNS 10 DIGIT TIMESTAMP REPRESENTING CURRENT TIME
          time_t now;         
          time(&now); // Retrieve time[Timestamp] from system and save to &now variable
          return now;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // ############## MQTT CALLBACK  ######################################
  // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO
  
  Serial.printf("\nMessage received : ( topic: %s ) \n",topic ); 
  char *received = new char[length + 1] {0}; 
  
  for (int i = 0; i < length; i++) { 
    received[i] = (char)payload[i];    
  }

  // PRINT RECEIVED MESSAGE
  Serial.printf("Payload : %s \n",received);

 
  // CONVERT MESSAGE TO JSON
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, received);  

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }


  // PROCESS MESSAGE
  const char* type = doc["type"]; 

  if (strcmp(type, "sensors") == 0){
    // 1. EXTRACT ALL PARAMETERS: NODES, RED,GREEN, BLUE, AND BRIGHTNESS FROM JSON OBJECT
    int ctemp       = doc["celsTemperature"];
    int ftemp       = doc["fahrTemperature"];
    int btemp       = doc["bmp_temp"];
    int humid       = doc["humidity"];
    int heatIndex   = doc["heatindex"];
    int soilmoist   = doc["soilMoisture"];
    int pressure    = doc["pressure"];
    int altitude    = doc["altitude"];
  }
}

bool publish(const char *topic, const char *payload){   
    bool res = false;
    try{
      res = mqtt.publish(topic,payload);
      // Serial.printf("\nres : %d\n",res);
      if(!res){
        res = false;
        throw false;
      }
    }
    catch(...){
      Serial.printf("\nError (%d) >> Unable to publish message\n", res);
    }
  return res;
}



// //***** Complete the util functions below ******

void display(){
  //Section to display the temperature value, Humidity, Soil Moisture in Percentage,Pressure, and Altitude on the TFT Display
  tft.setRotation(1);
  //tft.fillRect(0, 0, tft.width()-2, tft.height()-2, ILI9341_WHITE);

  //RECTANGLE 1 - displays the temperatures read from the DHT22 and BMP280 sensors
  tft.drawRect(1, 4, tft.width()-2, 72, ILI9341_GREEN);

  tft.drawBitmap(3, 10, sun, 50, 50, ILI9341_YELLOW, ILI9341_WHITE);  //sun picture
  tft.drawBitmap(260, 170, partlyCloudyDay, 50, 50, ILI9341_CYAN, ILI9341_WHITE); //partly cloudy picture

  tft.setCursor(55, 23);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("DHT TEMP.");
  tft.setCursor(55, 45);
  tft.print(t);
  tft.setTextSize(1);
  tft.print("o");
  tft.setTextSize(2);
  tft.print("C");

  tft.setCursor(195, 23);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("BMP TEMP.");
  tft.setCursor(195, 45);
  tft.print(bmp.readTemperature());
  tft.setTextSize(1);
  tft.print("o");
  tft.setTextSize(2);
  tft.print("C");

  //RECTANGLE 2 - displays the humidity reading from the DHT22
  //            - displays the heat index calculated from the DHT22 humidity and temperature
  //            - displays the pressure read from the BMP280 sensor
  tft.drawRect(1, 76, tft.width()-2, 62, ILI9341_GREEN);

  tft.setCursor(5, 85);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("HUMIDITY");
  tft.setCursor(5, 107);
  tft.print(h);
  tft.setTextSize(2);
  tft.print("%");

  tft.drawLine(105, 76, 105, 138, ILI9341_GREEN);

  tft.setCursor(115, 85);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("H.I.");
  tft.setCursor(115, 107);
  tft.print(hI);
  tft.setTextSize(1);
  tft.print("o");
  tft.setTextSize(2);
  tft.print("F");

  tft.drawLine(192, 76, 192, 138, ILI9341_GREEN);

  tft.setCursor(195, 85);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("PRESSURE");
  tft.setCursor(195, 107);
  tft.print(bmp.readPressure()/100);
  tft.setTextSize(2);
  tft.print(" hPa");

  //RECTANGLE 3 - displays the soil moisture reading converted to percentage
  //            - displays the approximated altitude
  tft.drawRect(1, 138, tft.width()-2, 100, ILI9341_GREEN);

  tft.setCursor(5, 150);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("SOIL MOISTURE: ");
  tft.setCursor(190, 150);
  tft.print(conVal);
  tft.setTextSize(2);
  tft.print("%");

  tft.setCursor(5, 180);
  tft.setTextColor(ILI9341_DARKGREY, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("APPROX.");
  tft.setCursor(5, 200);
  tft.print("ALTITUDE: ");
  tft.setCursor(140, 200);
  tft.print(bmp.readAltitude(1013.25));
  tft.setTextSize(2);
  tft.print(" m");
}

double calcHeatIndex(double t, double h){
    // CALCULATE AND RETURN HEAT INDEX USING EQUATION FOUND AT https://byjus.com/heat-index-formula/#:~:text=The%20heat%20index%20formula%20is,an%20implied%20humidity%20of%2020%25

  double ft = (t * 9.0/5.0) + 32;
  double HI = -42.379 + (2.04901523 * ft) + (10.14333127 * h) + (-0.22475541 * ft * h) + (-0.00683783 * pow(ft, 2))  + (-0.05481717 * pow(h,2)) + (0.00122874 * pow(ft, 2) * h)  + (0.00085282 * ft * pow(h,2)) + (-0.00000199 * pow(ft, 2) * pow(h,2));
  hI =  (5.0/9.0) * (HI - 32);
  return hI;
}

double calcSoilValue(){    
  soilVal = analogRead(soil_m);
  conVal = map(soilVal, 1500, 3900, 100, 0);
  if (conVal < 0)
  {
    conVal = 0;
  }
  else if (conVal >= 100)
  {
    conVal = 100;
  }  
  return conVal; 
}