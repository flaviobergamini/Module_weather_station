#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <string.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>


// SSID e Password para o modo AP
const char *ssid = "SWS_Module";
const char *password = "12345678";
const char *ssid_new;
const char *pass_new;
String serverAddress_http = "http://10.0.0.104";

WiFiClient client;
ESP8266WebServer server(80);

//IP address padrão para o acesso no navegador http://192.168.4.1

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
 
<h2>Weather Station Module<h2>
<h3> Fetin - 2019</h3>
 
<form action="/action_page">
  SSID:<br>
  <input type="text" name="SSID" value="">
  <br>
  Password:<br>
  <input type="text" name="Password" value="">
  <br><br>
  <input type="submit" value="Submit">
</form> 
 
</body>
</html>
)=====";

void logica()
{
  post("Humidade", 31.22);
  delay(200);
  String  pay = gett();
  Serial.println(pay);
  Serial.print("Desenvolver a logica final aqui");
  while(1)
  {
    digitalWrite(D3, HIGH);
    delay(200);
    digitalWrite(D3, LOW);
    delay(200);
  }
}

void post(String nome, float valor)
{
  String Svalor = String(valor);
  HTTPClient http;
  http.begin(serverAddress_http); 
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.POST(nome);
  String payload = http.getString();
  http.end();
  delay(200);
  http.begin(serverAddress_http);
  http.addHeader("Content-Type", "text/plain");
  int httpCode1 = http.POST(Svalor);
  String payload1 = http.getString();  
  http.end();
}

String gett()
{
  String pay;
  HTTPClient http;
  http.begin(serverAddress_http);
  int httpReceive = http.GET();
  //Serial.println(httpReceive);
  if (httpReceive == HTTP_CODE_OK) 
  {
    pay = http.getString();
   // Serial.println(pay);
  }
  http.end();
  return pay;
}

void handleRoot() {
 String s = MAIN_page;                                           //lendo HTML
 server.send(200, "text/html", s);                               //enviando HTML para o client
}

void handleForm() {
 String ssid_Station = server.arg("SSID"); 
 String pass_Station = server.arg("Password"); 
 
 ssid_new = ssid_Station.c_str();
 pass_new = pass_Station.c_str();
 
 Serial.print("SSID: ");
 Serial.println(ssid_Station);
 
 Serial.print("Password: ");
 Serial.println(pass_Station);
 Serial.print("------------- \n");
 Serial.println(ssid_new);
 
 String s = "<a href='/'> Go Back </a>";
 server.send(200, "text/html", s); //Send web page
//-----------------------------------------------------------------------
 for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(800);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_new, pass_new);
  delay(500);
  int loop1 = 1;
  if ((WiFi.status() == WL_CONNECTED)) 
  {
    while(loop1)
    {
      if ((WiFi.status() == WL_CONNECTED)) 
      {
        digitalWrite(D0, HIGH);
        digitalWrite(D1, LOW);
        loop1 = 0;
      }
    }
    Serial.print("Conectado \n");
    WriteEEPROM(ssid_new, pass_new);
    logica();
  }
  else{
    Serial.print("Não conectado \n");
    digitalWrite(D0, LOW);
    digitalWrite(D1, HIGH);
    ESP.reset();
    modeAP();
  }
  
  
//-----------------------------------------------------------------------
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1024);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D7, INPUT);
  digitalWrite(D0,LOW);
  digitalWrite(D1,HIGH);
  digitalWrite(D3,LOW);
  byte value = EEPROM.read(0);
  EEPROM.end();
  if(digitalRead(D7) == LOW)
    eraseEEPROM();
  if(value == 0)
  {
    Serial.print("Value = 0");
    delay(500);
    modeAP();
   }
   else
   {
   ReadEEPROM(ssid, password);
   handleForm(); 
   }
}

void loop() {
  server.handleClient();
}

void modeAP()
{
  delay(1000);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/action_page", handleForm); //form action is handled here
  server.begin();
  Serial.println("HTTP server started");
}

void eraseEEPROM() {
  EEPROM.begin(1024);
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

void WriteEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();
 
  Serial.println("writing eeprom ssid:");
          for (int i = 0; i < ssidlen; ++i)
            {
              EEPROM.write(i, ssid[i]);
              Serial.print("Wrote: ");
              Serial.println(ssid[i]); 
            }

  Serial.println("writing eeprom password:");
          for (int i = 0; i < passlen; ++i)
            {
              EEPROM.write((i+ssidlen), password[i]);
              Serial.print("Wrote: ");
              Serial.println(password[i]); 
            }
  EEPROM.end();
}

void ReadEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();

  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < ssidlen; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    //esid.trim();
  Serial.println(esid.length());
  Serial.print("SSID saida: ");
  Serial.println(esid);
  
  Serial.println("\n");
  
  Serial.println("Lendo password EEPROM");
  String passq;
  for (int i = (ssidlen); i < (ssidlen+passlen); ++i)
  {
    passq +=char(EEPROM.read(i));
  }
  Serial.println("Tamanho da password: ");
  Serial.println(passq.length());
  Serial.println("Password saida: ");
  Serial.println(passq);
  
  ssid_new = esid.c_str();
  pass_new = passq.c_str();
  EEPROM.end();
}
