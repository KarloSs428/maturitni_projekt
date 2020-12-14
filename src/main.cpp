#include "Arduino.h"
#include "WiFi.h" 
#include "ESPAsyncWebServer.h" 
#include "SPIFFS.h"
#include "Keypad.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"

//const char* ssid = "SitePark.cz-Demel_AP";
//const char* password = "26894065";

const char* ssid = "SSPUOpava";
const char* password = "";

const char* PARAM_INPUT_1 = "state";

const int cidloPin = 13;
const int ledPin = 2;

long int DebounceTimer;
int detekovano = false;
/////////////displej///////////

LiquidCrystal_I2C lcd(0x27, 20, 4);


//////////////klavesnice////////////////////
const byte radky = 4;
const byte sloupce = 4;

char keys[radky][sloupce] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinyRadku[radky] = {12, 14, 27, 26};
byte pinySloupcu[sloupce] = {25, 33, 32, 35};

Keypad klavesnice = Keypad( makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
//////////////////////////////////////////////////////

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

//fuknce detekce, ktera se spusti pomoci preruseni
void IRAM_ATTR detekce() {
  detekovano = true;
  digitalWrite(ledPin, HIGH);
}


void setup(){
  Serial.begin(115200);
  lcd.begin(16,2);
  Wire.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(cidloPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);

  attachInterrupt(cidloPin, detekce, RISING);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  delay(2000);
  
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

  //javascript
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/css");
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

  server.begin();
}

void mereniCasuOdSpusteni(){
  Serial.print("Cas od zapnuti: ");
  Serial.print(millis()/1000);
  Serial.println(" vterin.");
  delay(1000);
}


int kdyzDetekce(int ledka){
  digitalWrite(ledka, HIGH);
  delay(1000);
  digitalWrite(ledka, LOW);
}


void loop(){
  //mereniCasuOdSpusteni();
  if(detekovano == true){
    kdyzDetekce(ledPin);
    Serial.println("Ble");
    detekovano = false;
  }
  char klavesa = klavesnice.getKey();
  if (klavesa){
    Serial.print("Stisknuta klavesa: ");
    Serial.println(klavesa);
  }
}