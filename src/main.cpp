#include "Arduino.h"
#include "WiFi.h" 
#include "ESPAsyncWebServer.h" 
#include "SPIFFS.h"
#include "Keypad.h"

//const char* ssid = "SitePark.cz-Demel_AP";
//const char* password = "26894065";

const char* ssid = "SSPUOpava";
const char* password = "";

//////////////klavesnice////////////////////
const byte radky = 4;
const byte sloupce = 4;

char keys[radky][sloupce] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinyRadku[radky] = {13, 12, 14, 27};
byte pinySloupcu[sloupce] = {26, 25, 33, 32};

Keypad klavesnice = Keypad( makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
//////////////////////////////////////////////////////

// Set LED GPIO - tohle by mela byt ledka na esp32
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  /*pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();*/
}
 
void loop(){

  char klavesa = klavesnice.getKey();
  if (klavesa){
    Serial.print(klavesa);
  }
}