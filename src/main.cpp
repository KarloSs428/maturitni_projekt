#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Keypad.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "Tone32.h" //knihovna na bzucak, ktera z nejakeho duvodu prestala fungovat

#define trigger 13
#define echo 2

//udaje pro wifi ve skole
/*const char* ssid = "SSPUOpava";
const char* password = "";*/

//udaje pro wifi u me doma
const char *wifiSsid = "vasek pasek";
const char *wifiPassword = "jsemborec123";

long duration;

int distance, initialDistance, currentDistance, i;
int screenOffMsg = 0;

String hesloAlarm = "2369";
String docasneHeslo;

boolean aktivovano = false; //stav alarmu
boolean isActivated;
boolean activateAlarm = false;
boolean alarmActivated = false;
boolean enteredPassword;
boolean passChangeMode = false;
boolean passChanged = false;

/////////////DISPLEJ///////////

LiquidCrystal_I2C lcd(0x27, 16, 2);

//////////////////////////////

//////////KLAVESNICE///////////

const byte radky = 4;
const byte sloupce = 4;

char klavesa;

char keys[radky][sloupce] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pinyRadku[radky] = {12, 14, 27, 26};
byte pinySloupcu[sloupce] = {25, 33, 32, 4};

Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

//////////////////////////////

//spusti se web server na portu 80
AsyncWebServer server(80);

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

void zadejHeslo()
{
  int k = 7;
  docasneHeslo = "";
  aktivovano = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0, 1);
  lcd.print("Heslo:");
  while (aktivovano)
  {
    klavesa = klavesnice.getKey();
    if (klavesa != NO_KEY)
    {
      if (klavesa == '0' || klavesa == '1' || klavesa == '2' || klavesa == '3' ||
          klavesa == '4' || klavesa == '5' || klavesa == '6' || klavesa == '7' ||
          klavesa == '8' || klavesa == '9')
      {
        docasneHeslo += klavesa;
        lcd.setCursor(k, 1);
        lcd.print("*");
        k++;
      }
    }
    if (k > 11 || klavesa == '#')
    {
      docasneHeslo = "";
      k = 5;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" *** ALARM *** ");
      lcd.setCursor(0, 1);
      lcd.print("Heslo:");
    }
    if (klavesa == '*')
    {
      if (docasneHeslo == hesloAlarm)
      {
        aktivovano = false;
        alarmActivated = false;
        screenOffMsg = 0;
      }
      else if (docasneHeslo != hesloAlarm)
      {
        lcd.setCursor(0, 1);
        lcd.print("zkus to znovu");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" *** ALARM *** ");
        lcd.setCursor(0, 1);
        lcd.print("Heslo:");
      }
    }
  }
}

//funkce pro ziskani vzdalenosti pomoci ultrasonickeho senzoru
long getDistance()
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  //SPIFFS
  spifsOvereni();
  spifs();

  //pripojeni k wifi
  WiFi.begin(wifiSsid, wifiPassword);
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

  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(trigger, OUTPUT); 
  pinMode(echo, INPUT); 
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  if (activateAlarm)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("alarm se ");
    lcd.setCursor(0, 1);
    lcd.print("spusti za ");

    //odpocitavani do zapnuti alarmu
    int countdown = 9;
    while (countdown != 0)
    {
      lcd.setCursor(13, 1);
      lcd.print(countdown);
      countdown--;
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("alarm aktivovan");
    initialDistance = getDistance();
    activateAlarm = false;
    alarmActivated = true;
  }

  if (alarmActivated == true)
  {
    currentDistance = getDistance() + 10;
    if (currentDistance < initialDistance)
    {
      lcd.clear();
      zadejHeslo();
    }
  }

  if (!alarmActivated)
  {
    if (screenOffMsg == 0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("A - aktivace");
      lcd.setCursor(0, 1);
      lcd.print("B - zmena hesla");
      screenOffMsg = 1;
    }
    klavesa = klavesnice.getKey();
    //aktivace alarmu
    if (klavesa == 'A')
    {
      activateAlarm = true;
    }
    //zmena hesla
    else if (klavesa == 'B')
    {
      lcd.clear();
      int i = 1;
      docasneHeslo = "";
      lcd.setCursor(0, 0);
      lcd.print("soucasne heslo");
      lcd.setCursor(0, 1);
      lcd.print(">");
      passChangeMode = true;
      passChanged = true;
      while (passChanged)
      {
        klavesa = klavesnice.getKey();
        if (klavesa != NO_KEY)
        {
          if (klavesa == '0' || klavesa == '1' || klavesa == '2' || klavesa == '3' ||
              klavesa == '4' || klavesa == '5' || klavesa == '6' || klavesa == '7' ||
              klavesa == '8' || klavesa == '9')
          {
            docasneHeslo += klavesa;
            lcd.setCursor(i, 1);
            lcd.print("*");
            i++;
          }
        }
        if (i > 5 || klavesa == '#')
        {
          docasneHeslo = "";
          i = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("soucasne heslo");
          lcd.setCursor(0, 1);
          lcd.print(">");
        }
        if (klavesa == '*')
        {
          i = 1;
          if (hesloAlarm == docasneHeslo)
          {
            docasneHeslo = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("nove heslo");
            lcd.setCursor(0, 1);
            lcd.print(">");
            while (passChangeMode)
            {
              klavesa = klavesnice.getKey();
              if (klavesa != NO_KEY)
              {
                if (klavesa == '0' || klavesa == '1' || klavesa == '2' || klavesa == '3' ||
                    klavesa == '4' || klavesa == '5' || klavesa == '6' || klavesa == '7' ||
                    klavesa == '8' || klavesa == '9')
                {
                  docasneHeslo += klavesa;
                  lcd.setCursor(i, 1);
                  lcd.print("*");
                  i++;
                }
              }
              if (klavesa == '*')
              {
                i = 1;
                hesloAlarm = docasneHeslo;
                passChangeMode = false;
                passChanged = false;
                screenOffMsg = 0;
              }
              if (i > 5 || klavesa == '#')
              {
                docasneHeslo = "";
                i = 1;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("nove heslo");
                lcd.setCursor(0, 1);
                lcd.print(">");
              }
                        }
          }
        }
      }
    }
  }
}
