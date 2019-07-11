#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// SSID e Password para o modo AP
const char *ssid = "SWS_Module";
const char *password = "12345678";

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
  <input type="text" name="SSID" value="Flavio">
  <br>
  Password:<br>
  <input type="text" name="Password" value="Bergamini">
  <br><br>
  <input type="submit" value="Submit">
</form> 
 
</body>
</html>
)=====";

void handleRoot() {
 String s = MAIN_page;                                           //lendo HTML
 server.send(200, "text/html", s);                               //enviando HTML para o client
}

void handleForm() {
 String ssid_Station = server.arg("SSID"); 
 String pass_Station = server.arg("Password"); 
 
 Serial.print("SSID: ");
 Serial.println(ssid_Station);
 
 Serial.print("Password: ");
 Serial.println(pass_Station);
 
 String s = "<a href='/'> Go Back </a>";
 server.send(200, "text/html", s); //Send web page
}

void setup() {
  delay(1000);
  Serial.begin(115200);
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

void loop() {
  server.handleClient();
}
