#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "fauxmoESP.h"
#include "credentials.h"
#include "afwebpage.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Wire.h>

//ESP8266WiFiMulti WiFiMulti;

AsyncWebServer server(HTTP_PORT);

using namespace fs;
// ws://ip/ws
AsyncWebSocket ws("/ws");
//envio de evento do servidor quando requisitado nesse local
AsyncEventSource events("/events");

bool shouldReboot = false;

fauxmoESP fauxmo;

void deleteFile(fs::FS &fs, char *filename);

/* Se utlizar a condição booleana para trocar o estado, trocar a linha inversora do bit pela linha comentada*/
void changeStateOf(uint8_t target, bool st){
    Serial.print("Valor a ser mudado: ");
    Serial.println(bits_value);
    bits_value = st < 1 ? bits_value|(1<<target) : bits_value&~(1<<target);   
    //bits_value = bits_value^(1<<target);
    Wire.beginTransmission(PCF_ADDR);
    Wire.write(bits_value);
    Wire.endTransmission();
    Serial.print("Valor atual: ");
    Serial.println(bits_value);
}

void setMultiDevices(){
  /*
  for (uint8_t i=0;i<7;i++){
    fauxmo.addDevice(ALEXA_COMMANDS[i]);
  }*/
  fauxmo.addDevice(ALEXA_COMMANDS[0]);
  fauxmo.addDevice(ALEXA_COMMANDS[1]);
  fauxmo.addDevice(ALEXA_COMMANDS[2]);
  //fauxmo.addDevice(ALEXA_COMMANDS[3]);
  //fauxmo.addDevice(ALEXA_COMMANDS[4]);
  //fauxmo.addDevice(ALEXA_COMMANDS[5]);
  //fauxmo.addDevice(ALEXA_COMMANDS[6]);
  Serial.println("device added");
}

void loadCredentials() {
  String recipe1 = readFile(LittleFS, "/ssid.txt");
  if (!recipe1.equals("none")){
    strcpy(WIFI_SSID,recipe1.c_str());
  }

  String recipe2 = readFile(LittleFS, "/passwd.txt");
  if (!recipe2.equals("none")){
    strcpy(WIFI_PASS,recipe2.c_str());
  }

  String recipe3 = readFile(LittleFS, "/apssid.txt");
  if (!recipe3.equals("none")){
    strcpy(AP_WIFI_SSID,recipe3.c_str());
  }

  String recipe4 = readFile(LittleFS, "/appasswd.txt");
  if (!recipe4.equals("none")){
    strcpy(AP_WIFI_PASS,recipe4.c_str());
  }

  String recipe5 = readFile(LittleFS, "/adminuser.txt");
  if (!recipe5.equals("none")){
    strcpy(LOGIN_USER,recipe5.c_str());
  }

  String recipe6 = readFile(LittleFS, "/adminpass.txt");
  if (!recipe6.equals("none")){
    strcpy(LOGIN_PASS,recipe6.c_str());
  }

  for (uint8_t i=0;i<7;i++){
    String recipe = readFile(LittleFS, filenames[i]);
    if (!recipe.equals("none")){
      strcpy(ALEXA_COMMANDS[i],recipe.c_str());
    }
  }

  Serial.println("---------------------------");
  Serial.println("L I D O   D O   A R Q U I V O");
  Serial.print("STA SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("STA password: ");
  Serial.println(WIFI_PASS);
  Serial.print("AP SSID: ");
  Serial.println(AP_WIFI_SSID);
  Serial.print("AP password: ");
  Serial.println(AP_WIFI_PASS);
  Serial.print("Administrador: ");
  Serial.println(LOGIN_USER);
  Serial.print("Senha admin: ");
  Serial.println(LOGIN_PASS);

  Serial.println(" ");
  Serial.println("--------------------");
  Serial.println("Consulte os comandos na pagina de credenciais.");
  Serial.println("Se for no modo AP: http://10.0.0.1/credenciais");
  Serial.println("Se for no modo STA, use o IP exibido na serial.");
  Serial.println("---------------------------");
}

void setValuesToVars(uint8_t target, char *content) {
  if (target == IS_SSID) {
    writeFile(LittleFS, "/ssid.txt", content);
  }
  else if (target == IS_PASSWD) {
    writeFile(LittleFS, "/passwd.txt", content);
  }
  else if (target == IS_APSSID) {
    writeFile(LittleFS, "/apssid.txt", content);
  }
  else if (target == IS_APPASSWD) {
    writeFile(LittleFS, "/appasswd.txt", content);
  }
  else if (target == IS_ADMIN_U) {
    writeFile(LittleFS, "/adminuser.txt", content);
  }
  else if (target == IS_ADMIN_P) {
    writeFile(LittleFS, "/adminpass.txt", content);
  }

  uint8_t id_to_file = 7; //7 é o primeiro relé em relação às variáveis IS_<alguma_coisa>
  for (uint8_t i=0;i<7;i++){
    if (target == id_to_file+i){
      writeFile(LittleFS, filenames[i], content);
    }
  }
}



//NAO ESQUECER DE ADICIONAR A BARRA ANTES DO NOME DO ARQUIVO
void writeFile(fs::FS &fs, char *filename, char *content) {
  Serial.println(content);
  File myFile = LittleFS.open(filename, "w");
  if (!myFile) {
    Serial.println("Problema ao tentar ler, sorry...");
    return;
  }

  if (myFile.print(content)) {
    Serial.println("conteudo salvo");
  }
  else {
    Serial.println("nao foi possivel salvar");
  }
  myFile.close();
}

String readFile(fs::FS &fs, char *filename) {
  const char *path = filename;
  Serial.printf("Reading file: %s\n", path);

  if (path[0] != '/') {
    Serial.println("File path needs start with /. Change it.");
    return "ouch! slash forgoten";
  }

  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open file for reading");
    /*infeliz condicao. Arrumar isso*/
    if (strcmp(filename,"/adminuser.txt") == 0 ||  strcmp(filename,"/adminpass.txt") == 0){
      return "admin";
    }
    if (strcmp(filename,"/apssid.txt") == 0 ||  strcmp(filename,"/appasswd.txt") == 0){
      return "afintelihome";
    }
    return "none";
  }

  Serial.println("Reading from file... ");
  char buf[50];
  memset(buf, 0, 50);

  while (file.available()) {
    file.readBytesUntil(0, buf, 49);
  }
  file.close();
  Serial.println(buf);

  return buf;
}

void deleteFile(fs::FS &fs, char *filename) {
  if (filename[0] != '/') {
    Serial.println("File path needs start with /. Change it.");
  }
  if (fs.remove(filename)) {
    return;
  }
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  Serial.println("Implementar pagina aqui ou no 'login success?'");
}


void serverSetup() {

  // Custom entry point (not required by the library, here just as an example)
  //como nao eh utilizado pela lib, podemos jogar o index como config: (https://github.com/me-no-dev/ESPAsyncWebServer)
  //na pagina da lib tem exemplo de auth.

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hello, world");
    Serial.println("HOUVE REQUISICAO!!!!!!!");
  });

  // These two callbacks are required for gen1 and gen3 compatibility
  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), String((char *)data))) return;
    // Handle any other body request here...
  });
  server.onNotFound([](AsyncWebServerRequest * request) {
    String body = (request->hasParam("body", true)) ? request->getParam("body", true)->value() : String();
    if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), body)) return;
    // Handle not found request here...
  });



  /*  CONFIGURACAO DE PAGINA DE LOGIN*/
  //---------------------------------------------------------------------------------------
  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // attach AsyncEventSource
  server.addHandler(&events);

  server.on("/login",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LOGIN_USER, LOGIN_PASS))
      return request->requestAuthentication();
    request->send(200, "text/plain", "Login Ok");
  }
           );

  server.on("/config",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LOGIN_USER, LOGIN_PASS))
      return request->requestAuthentication();

    request->send_P(200, "text/html", login_html);
  }
           );

//-------------------------- credenciais --------------------------------------------------
  server.on("/credenciais",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LOGIN_USER, LOGIN_PASS))
      return request->requestAuthentication();

    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("STA SSID: ");
    response->print(String(WIFI_SSID));
    response->print("<br>");

    response->print("STA PASSWORD: ");
    response->print(String(WIFI_PASS));
    response->print("<br>");

    response->print("AP SSID: ");
    response->print(String(AP_WIFI_SSID));
    response->print("<br>");

    response->print("AP PASSWORD: ");
    response->print(String(AP_WIFI_PASS));
    response->print("<br>");

    response->print("ADMIN USER: ");
    response->print(String(LOGIN_USER));
    response->print("<br>");

    response->print("ADMIN PASS: ");
    response->print(String(LOGIN_PASS));
    response->print("<br>");

    char msg[18];
    memset(msg,0,18);
    for (uint8_t i=0;i<7;i++){
      sprintf(msg,"IDENTIFIER %d: ", i+1);
      response->print(msg);
      response->print(String(ALEXA_COMMANDS[i]));
      response->print("<br>");
      memset(msg,0,18);
    }
    
    request->send(response);
    //text/plain ou text/html

    request->send_P(200, "text/html", login_html);
  }
           );

//------------------ redirecionado do /config -----------------------------------------
  server.on("/setup",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(LOGIN_USER, LOGIN_PASS))
      return request->requestAuthentication();

    String converter;
    if (request->hasParam("input_ssid")) {
      memset(WIFI_SSID, 0, 51);
      converter = request->getParam("input_ssid")->value();
      strcpy(WIFI_SSID, converter.c_str());
      Serial.println(WIFI_SSID);
      setValuesToVars(IS_SSID, WIFI_SSID);
    }

    if (request->hasParam("input_passwd")) {
      memset(WIFI_PASS, 0, 51);
      converter = request->getParam("input_passwd")->value();
      strcpy(WIFI_PASS, converter.c_str());
      Serial.println(WIFI_PASS);
      setValuesToVars(IS_PASSWD, WIFI_PASS);
    }

    if (request->hasParam("input_apssid")) {
      memset(AP_WIFI_SSID, 0, 51);
      strcpy(AP_WIFI_SSID, request->getParam("input_apssid")->value().c_str());
      Serial.println(AP_WIFI_SSID);
      setValuesToVars(IS_APSSID, AP_WIFI_SSID);
    }

    if (request->hasParam("input_appasswd")) {
      memset(AP_WIFI_PASS, 0, 51);
      strcpy(AP_WIFI_PASS, request->getParam("input_appasswd")->value().c_str());
      Serial.println(AP_WIFI_PASS);
      setValuesToVars(IS_APPASSWD, AP_WIFI_PASS);
    }

    if (request->hasParam("input_admin_u")) {
      memset(LOGIN_USER, 0, 51);
      strcpy(LOGIN_USER, request->getParam("input_admin_u")->value().c_str());
      Serial.println(LOGIN_USER);
      setValuesToVars(IS_ADMIN_U, LOGIN_USER);
    }

    if (request->hasParam("input_admin_p")) {
      memset(LOGIN_PASS, 0, 51);
      strcpy(LOGIN_PASS, request->getParam("input_admin_p")->value().c_str());
      Serial.println(LOGIN_PASS);
      setValuesToVars(IS_ADMIN_P, LOGIN_PASS);
    }

    for (uint8_t i=0;i<7;i++){
      if (request->hasParam(FORM_INPUTS[i])) {
        memset(ALEXA_COMMANDS[i], 0, 50);
        strcpy(ALEXA_COMMANDS[i], request->getParam(FORM_INPUTS[i])->value().c_str());
        Serial.println(ALEXA_COMMANDS[i]);
        setValuesToVars(i+7, ALEXA_COMMANDS[i]);
      }
    }

    request->send(200, "text/plain", "Aplicado. Reiniciando...");
    ESP.restart();

  }
           );


  //----------------------------------------------------------------------------------------




  // Start the server
  server.begin();

}

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------


void wifiSetup() {

  startCredentials();
  loadCredentials();
  // Set WIFI module to AP+STA mode
  WiFi.softAPConfig(staticIP,gateway,subnet);
  WiFi.softAP(AP_WIFI_SSID,AP_WIFI_PASS);
  WiFi.mode(WIFI_AP_STA);
  //WiFiMulti.addAP(AP_WIFI_SSID, AP_WIFI_PASS);
  

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  unsigned long int timeout = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    if ((millis()-timeout) > 15000){
      break; 
    }
    
  }
  Serial.println();
  delay(2000);

  // Connected!
  if (WiFi.status() == WL_CONNECTED){
    Serial.printf("[WiFi] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    Serial.println(WiFi.macAddress());
    Serial.print("[WiFi] AP Mode, IP address: ");
    Serial.println(WiFi.softAPIP());
  }


}

void setup() {
  //inicializa todos os bits em HIGH (lógica invertida)
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.beginTransmission(PCF_ADDR);
  Wire.write(bits_value);
  Wire.endTransmission();
    
  
  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  if (!LittleFS.begin()) {
    Serial.println("Couldn't mount the filesystem.");
  }

  delay(2000); //pra dar tempo de abrir o monitor serial e ler as configs


  // Wifi
  //wifi_set_macaddr(STATION_IF, &newMACAddress[0]);
  wifiSetup();
  //wifi_set_macaddr(STATION_IF, &newMACAddress[0]);

  // Web server
  serverSetup();

  delay(2000);

  // Set fauxmoESP to not create an internal TCP server and redirect requests to the server on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(false);
  fauxmo.setPort(HTTP_PORT); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn kitchen on" ("kitchen" is the name of the first device below)
  // "Alexa, turn on kitchen"
  // "Alexa, set kitchen to fifty" (50 means 50% of brightness)

  // Add virtual devices
  setMultiDevices();

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {

    // Callback when a command from Alexa is received.
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.

    // if (0 == device_id) digitalWrite(RELAY1_PIN, state);
    // if (1 == device_id) digitalWrite(RELAY2_PIN, state);
    // if (2 == device_id) analogWrite(LED1_PIN, value);

    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    // For the example we are turning the same LED on and off regardless fo the device triggered or the value
    //digitalWrite(RELE0, !state); // we are nor-ing the state because our LED has inverse logic.


    changeStateOf(device_id, state); 

  });

}

void loop() {

  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  // This is a sample code to output free heap every 5 seconds
  // This is a cheap way to detect memory leaks
  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }

}
