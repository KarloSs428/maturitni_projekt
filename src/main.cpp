#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Keypad.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"

/*const char* ssid = "SitePark.cz-Demel_AP";
const char* password = "26894065";*/

/*const char* ssid = "SSPUOpava";
const char* password = "";*/

//udaje pro wifi
const char *ssid = "vasek pasek";
const char *password = "jsemborec123";

const int cidloPin = 13;
const int ledPin = 2;

int detekovano = false;

bool aktivovano;
bool alarmZapnuty;

String hesloAlarm="1234";
String tempPassword;

///////////////////////////////
/////////////DISPLEJ///////////

LiquidCrystal_I2C lcd(0x27, 16, 2);

///////////////////////////////
//////////////////////////////

///////////////////////////////
//////////KLAVESNICE///////////

const byte radky = 4;
const byte sloupce = 4;

char keys[radky][sloupce] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pinyRadku[radky] = {12, 14, 27, 26};
byte pinySloupcu[sloupce] = {25, 33, 32, 35};

Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
///////////////////////////////
//////////////////////////////

//spusti se web server na portu 80
AsyncWebServer server(80);

//fuknce detekce, ktera se spusti pomoci preruseni
void IRAM_ATTR detekce()
{
  detekovano = true;
  digitalWrite(ledPin, HIGH);
}

void spifs()
{
  //index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html");
  });

  //css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  //client.js
  server.on("/client.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/client.js", "text/css");
  });
}

void spifsOvereni()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}


//funkce, ktera se spusti, kdyz se aktivuje alarm
void narusitel(int ledka)
{
  char klavesa = klavesnice.getKey();
  if (detekovano == true)
  {
    digitalWrite(ledka, HIGH);
    Serial.println("pohyb");
    if (klavesa == '0')
    {
      digitalWrite(ledka, LOW);
      detekovano = false;
      aktivovano = false;
    }
  }
}



/*void enterPassword() {
  int k=5;
  tempPassword = "";
  aktivovano = true;
      while(aktivovano) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY){
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
          tempPassword += keypressed;
          lcd.setCursor(k,1);
          lcd.print("*");
          k++;
        }
      }
      if (k > 9 || keypressed == '#') {
        tempPassword = "";
        k=5;
        Serial.println("Zadej heslo: ");
      }
      if ( keypressed == '*') {
        if ( tempPassword == hesloAlarm ) {
          aktivovano = false;
          detekovano = false;
        }
        else if (tempPassword != HesloAlarm) {
          Serial.println("Zkus to znovu: ");
          delay(2000);
        }
      }    
    }
}*/

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////



void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(cidloPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);

  attachInterrupt(cidloPin, detekce, RISING);

  //SPIFFS
  spifsOvereni();
  spifs();

  //pripojeni
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  //vypsani IP adresy a zapnuti serveru
  Serial.println(WiFi.localIP());
  server.begin();


  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("ALARM");
  lcd.setCursor(1, 1);
  lcd.print("Karel Demel");
  delay(3000);
}



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////



void loop()
{
  narusitel(ledPin);
}
