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

//############### IMPORT HEADER FILES ##################
// #ifndef NTP_H
// #include "NTP.h"
// #endif

// #ifndef MQTT_H
// #include "mqtt.h"
// #endif

// ADD YOUR IMPORTS HERE
#include "DHT.h"

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

#define BOX_WIDTH 50
#define BOX_HEIGHT 50
#define SPACING 5
#define MARGIN 10

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
// #define BMP_SCK  (13)
// #define BMP_MISO (12)
// #define BMP_MOSI (11)
// #define BMP_CS   (10)


// MQTT CLIENT CONFIG  
// static const char* pubtopic      = "620154033";                    // Add your ID number here
// static const char* subtopic[]    = {"620154033_sub","/elet2415"};  // Array of Topics(Strings) to subscribe to
// static const char* mqtt_server   = "www.yanacreations.com";         // Broker IP address or Domain name as a String

// static uint16_t mqtt_port        = 1883;

// // WIFI CREDENTIALS
// const char* ssid       = "MonaConnect";     // Add your Wi-Fi ssid
// const char* password   = "";    // Add your Wi-Fi password 


// // TASK HANDLES 
// TaskHandle_t xMQTT_Connect          = NULL; 
// TaskHandle_t xNTPHandle             = NULL;  
// TaskHandle_t xLOOPHandle            = NULL;  
// TaskHandle_t xUpdateHandle          = NULL;
// TaskHandle_t xButtonCheckeHandle    = NULL;  

// FUNCTION DECLARATION   
void checkHEAP(const char* Name);   // RETURN REMAINING HEAP SIZE FOR A TASK
void initMQTT(void);                // CONFIG AND INITIALIZE MQTT PROTOCOL
unsigned long getTimeStamp(void);   // GET 10 DIGIT TIMESTAMP FOR CURRENT TIME
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload); // PUBLISH MQTT MESSAGE(PAYLOAD) TO A TOPIC
void vButtonCheck( void * pvParameters );
void vUpdate( void * pvParameters );  
bool isNumber(double number);
 

/* Declare your functions below */ 
double convert_Celsius_to_fahrenheit(double c);
double convert_fahrenheit_to_Celsius(double f);
double calcHeatIndex(double Temp, double Humid);


/* Init class Instances for the DHT22 etcc */
DHT dht(DHTPIN, DHTTYPE);  

double h;   //humidity read by the DHT sensor
double t;   //temperature read by the DHT sensor
double bt;  //temperature read by the BMP sensor
int conVal; //value of the soil moisture sensor readings after conversion
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
  pinMode(DHTPIN, INPUT);
  pinMode(soil_m, INPUT);
  // initialize();     // INIT WIFI, MQTT & NTP 
  // vButtonCheckFunction(); // UNCOMMENT IF USING BUTTONS INT THIS LAB, THEN ADD LOGIC FOR INTERFACING WITH BUTTONS IN THE vButtonCheck FUNCTION
  display();
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
  Serial.print("Humidity: ");
  Serial.println(h);
          
  double ft = (t * 9.0/5.0) + 32;
  double HI = -42.379 + (2.04901523 * ft) + (10.14333127 * h) + (-0.22475541 * ft * h) + (-0.00683783 * pow(ft, 2))  + (-0.05481717 * pow(h,2)) + (0.00122874 * pow(ft, 2) * h)  + (0.00085282 * ft * pow(h,2)) + (-0.00000199 * pow(ft, 2) * pow(h,2));
  hI =  (5.0/9.0) * (HI - 32);
  Serial.print("Heat Index: ");
  Serial.print(hI);
  Serial.println(" *C");

  int soilVal = analogRead(soil_m);
  Serial.println(soilVal);
  conVal = map(soilVal, 1450, 3900, 100, 0);

  if (conVal < 0)
  {
    conVal = 0;
  }
  else if (conVal >= 100)
  {
    conVal = 100;
  }
  Serial.print(F("Soil Moisture = "));
  Serial.print(conVal);
  Serial.println("%");
  delay(2000);

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");

  Serial.println("************************************************");
  display();

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}


//####################################################################
//#                          UTIL FUNCTIONS                          #       
//####################################################################
void vButtonCheck( void * pvParameters )  {
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );     
      
    for( ;; ) {
        // Add code here to check if a button(S) is pressed
        // then execute appropriate function if a button is pressed 
        vTaskDelay(200 / portTICK_PERIOD_MS);  
    }
}

// void vUpdate( void * pvParameters )  {
//     configASSERT( ( ( uint32_t ) pvParameters ) == 1 );    
           
//     for( ;; ) {
//           // #######################################################
//           // ## This function must PUBLISH to topic every second. ##
//           // #######################################################
   
//           // 1. Read Humidity and save in variable below
//           double h = dht.readHumidity();
           
//           // 2. Read temperature as Celsius   and save in variable below 
//           double t = dht.readTemperature();

//           if(isNumber(t)){
//               // ##Publish update according to ‘{"id": "student_id", "timestamp": 1702212234, "temperature": 30, "humidity":90, "heatindex": 30}’

//               // 1. Create JSon object
//               StaticJsonDocument<1000> doc;
//               // 2. Create message buffer/array to store serialized JSON object
//               char message[1100]  = { 0 };
//               // 3. Add key:value pairs to JSon object based on above schema
//               doc["id"]                 = "620154033";
//               doc["timestamp"]          = getTimeStamp();
//               doc["temperature"]        = t;
//               doc["humidity"]           = h;
//               doc["heatindex"]          = calcHeatIndex(t,h);
//               // 4. Seralize / Covert JSon object to JSon string and store in message array
//               serializeJson(doc, message); 
//               // 5. Publish message to a topic subscribed to by both backend and frontend                
//               if(mqtt.connected()){
//                 publish(pubtopic, message);
//               }
//           }  
//         vTaskDelay(1000 / portTICK_PERIOD_MS);  
//     }
// }

 

unsigned long getTimeStamp(void) {
          // RETURNS 10 DIGIT TIMESTAMP REPRESENTING CURRENT TIME
          time_t now;         
          time(&now); // Retrieve time[Timestamp] from system and save to &now variable
          return now;
}


// void callback(char* topic, byte* payload, unsigned int length) {
//   // ############## MQTT CALLBACK  ######################################
//   // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO
  
//   Serial.printf("\nMessage received : ( topic: %s ) \n",topic ); 
//   char *received = new char[length + 1] {0}; 
  
//   for (int i = 0; i < length; i++) { 
//     received[i] = (char)payload[i];    
//   }

//   // PRINT RECEIVED MESSAGE
//   Serial.printf("Payload : %s \n",received);

 
//   // CONVERT MESSAGE TO JSON
//   StaticJsonDocument<1000> doc;
//   DeserializationError error = deserializeJson(doc, received);  

//   if (error) {
//     Serial.print("deserializeJson() failed: ");
//     Serial.println(error.c_str());
//     return;
//   }


//   // PROCESS MESSAGE
//   const char* type = doc["type"]; 
//   // ‘{"type": "controls", "brightness": 255, "leds": 7, "color": { "r": 255, "g": 255, "b": 255, "a": 1 } }’
 

//   if (strcmp(type, "controls") == 0){
//     // 1. EXTRACT ALL PARAMETERS: NODES, RED,GREEN, BLUE, AND BRIGHTNESS FROM JSON OBJECT
//     int brightness  = doc["brightness"]; 
//     int nodes       = doc["leds"];
//     int red         = doc["color"]["r"];
//     int green       = doc["color"]["g"];
//     int blue        = doc["color"]["b"];
//     // 2. ITERATIVELY, TURN ON LED(s) BASED ON THE VALUE OF NODES. Ex IF NODES = 2, TURN ON 2 LED(s)
//     for (int i = 0; i < nodes; i++)
//     {
//       leds[i] = CRGB(red, green, blue);
//       FastLED.setBrightness(brightness);
//       FastLED.show();
//       delay(50);
//     }
//     // 3. ITERATIVELY, TURN OFF ALL REMAINING LED(s).
//     for(int j = nodes; j < NUM_LEDS; j++)
//     {
//       leds[j] = CRGB::Black;
//       FastLED.setBrightness( brightness );
//       FastLED.show();
//       delay(50);
//     }
//   }
// }

// bool publish(const char *topic, const char *payload){   
//     bool res = false;
//     try{
//       res = mqtt.publish(topic,payload);
//       // Serial.printf("\nres : %d\n",res);
//       if(!res){
//         res = false;
//         throw false;
//       }
//     }
//     catch(...){
//       Serial.printf("\nError (%d) >> Unable to publish message\n", res);
//     }
//   return res;
// }



// //***** Complete the util functions below ******

// double convert_Celsius_to_fahrenheit(double c){    
//   // CONVERTS INPUT FROM °C TO °F. RETURN RESULTS
//   return (c * 9.0/5.0) + 32;
// }

// double convert_fahrenheit_to_Celsius(double f){    
//     // CONVERTS INPUT FROM °F TO °C. RETURN RESULT 
//   return (5.0/9.0) * (f - 32);   
// }

// double calcHeatIndex(double Temp, double Humid){
//     // CALCULATE AND RETURN HEAT INDEX USING EQUATION FOUND AT https://byjus.com/heat-index-formula/#:~:text=The%20heat%20index%20formula%20is,an%20implied%20humidity%20of%2020%25

//   double ft = convert_Celsius_to_fahrenheit(Temp);
//   double hI = -42.379 + (2.04901523 * ft) + (10.14333127 * Humid) + (-0.22475541 * ft * Humid) + (-0.00683783 * pow(ft, 2))  + (-0.05481717 * pow(Humid,2)) + (0.00122874 * pow(ft, 2) * Humid)  + (0.00085282 * ft * pow(Humid,2)) + (-0.00000199 * pow(ft, 2) * pow(Humid,2));
//   return convert_fahrenheit_to_Celsius(hI);
// }
// //HI= c1+c2T+c3R+c4TR+c5T2+c6R2+c7T2R+c8TR2+c9T2R2
// bool isNumber(double number){       
//         char item[20];
//         snprintf(item, sizeof(item), "%f\n", number);
//         if( isdigit(item[0]) )
//           return true;
//         return false; 
// } 

void display(){
  //Section to display the temperature value, Humidity, Soil Moisture in Percentage,Pressure, and Altitude on the TFT Display      tft.fillRect(0, 0, tft.width(), tft.height(), ILI9341_MAGENTA);
  tft.setRotation(1);
  //tft.fillRect(0, 0, tft.width()-2, tft.height()-2, ILI9341_WHITE);

  tft.drawRoundRect(1, 6, tft.width()/2, 71, 5, ILI9341_MAGENTA);
  tft.drawRoundRect(160, 6, tft.width()/2, 71, 5, ILI9341_MAGENTA);

  tft.drawRoundRect(1, 76, tft.width()/2, 71, 5, ILI9341_MAGENTA);
  tft.drawRoundRect(160, 76, tft.width()/2, 71, 5, ILI9341_MAGENTA);

  tft.drawRoundRect(1, 150, tft.width(), 71, 5, ILI9341_MAGENTA);
  tft.drawRoundRect(1, 150, tft.width(), 71, 5, ILI9341_MAGENTA);

  tft.drawRoundRect(250, 90, 220, 225, 5, ILI9341_WHITE);
  tft.drawRoundRect(251, 91, 220, 225, 5, ILI9341_WHITE);

  tft.fillRect(26, 90, 180, 40, ILI9341_GREEN);
  tft.fillRect(270, 90, 180, 40, ILI9341_CYAN);

  tft.setCursor(45, 100);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.print("EXTERIOR");

  tft.setCursor(288, 100);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.print("INTERIOR");

  tft.setCursor(160, 155);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.print("%");

  tft.setCursor(412, 165);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(6);
  tft.print("%");

  tft.setCursor(175, 210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.print("C");

  tft.setCursor(160, 210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("o");

  tft.setCursor(412, 230);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(6);
  tft.print("C");

  tft.setCursor(395, 230);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.print("o");

  tft.setCursor(160, 265);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.print("mb");
  // tft.fillRect(MARGIN + 3 * (BOX_WIDTH + SPACING), tft.height() - BOX_HEIGHT - MARGIN, BOX_WIDTH, BOX_HEIGHT, 10, ILI9341_MAGENTA);
  //     tft.setCursor(2, 10);
  //     tft.print(F("DHT Temperature:"));
  //     tft.setTextSize(2);
  //     tft.print(t);
  //     tft.setTextSize(2);
  //     tft.print(" C");

  //     tft.setCursor(2, 30);
  //     tft.print(F("BMP Temperature= "));
  //     tft.print(bmp.readTemperature());
  //     tft.println(" C");
    
  //     tft.setCursor(2, 50);
  //     tft.print(F("Humidity:"));
  //     tft.setTextSize(2);
  //     tft.print(h);
  //     tft.setTextSize(2);

  //     tft.setCursor(2, 70);
  //     tft.print("Moisture Value:");
  //     tft.setTextSize(2);
  //     tft.print(conVal);
  //     tft.print("%");
  //     tft.setTextSize(2);

      

  //     tft.setCursor(2, 90);
  //     tft.print("Pressure= ");
  //     tft.print((bmp.readPressure()-23875.39)/1000);
  //     tft.println(" kPa");

  //     tft.setCursor(2, 110);
  //     tft.print("Approx. Altitude = ");
  //     tft.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  //     tft.println(" m");
}