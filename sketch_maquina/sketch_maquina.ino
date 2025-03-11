#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define ssid "SmartHome"
#define password "sae668128"

ESP8266WebServer server(80);

// Protótipos

void handleHomePage(void);
void handleNotFound(void);

void (*MaquinaStep)(void);
void StepInicial(void);
void StepUm(void);

// Funções

void StepInicial(void)
{
  MaquinaStep = StepUm;
}

void StepUm(void)
{
  MaquinaStep = StepInicial;
}

void setup()
{
  Serial.begin(9600);
  MaquinaStep = StepInicial;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleHomePage);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop()
{
  (*MaquinaStep)();
  server.handleClient();
}

void handleHomePage(void)
{
  String html = "<h1>Teste</h1>";
  server.send(200, "text/html", html);
}

void handleNotFound(void)
{
  String html = "<h1>Não</h1>";
  server.send(200, "text/html", html);
}
