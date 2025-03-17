#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define RELAY_0 16
#define RELAY_1 5

#define ssid "SmartHome"
#define password "sae668128"

#define homePage "<script>\
  let buttonClicked = async () => {\
    await fetch(`/buttonClicked`).then(() => {console.log('triggered')});\
  }\
</script>\
<html>\
  <head><meta charset=\"UTF-8\" /></head>\
  <body>\
    <button onclick=\"buttonClicked()\">Botão</button>\
    <a href=\"config\">Configurações</a>\
  </body>\
</html>"

#define configPage "<script>\
</script>\
<html>\
  <head><meta charset=\"UTF-8\"></head>\
  <body>\
    <form action=\"/configSubmit\">\
      <div>\
        <label for=\"capacitorTime\">Tempo do capacitor</label>\
        <input type=\"number\" id=\"capacitorTime\" name=\"capacitorTime\" min=\"2\" max=\"6\" />\
      </div>\
      <input type=\"submit\" value=\"Salvar\" />\
    </form>\
  </body>\
</html>"

// Variáveis

ESP8266WebServer server(80);
bool pump_state = false;
unsigned long triggerMillis = 0;
unsigned relayPeriod = 3000;

// Protótipos

void handleHomePage(void);
void handleNotFound(void);
void handleConfigPage(void);
void handleButtonChange(void);
void handleConfigSubmit(void);
void handleConfigRead(void);

void (*MaquinaStep)(void);
void StepInicial(void);
void StepUm(void);

// Funções Padrão

void setup()
{
  Serial.begin(9600);

  pinMode(RELAY_0, OUTPUT);
  pinMode(RELAY_1, OUTPUT);

  digitalWrite(RELAY_0, LOW);
  digitalWrite(RELAY_1, LOW);

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
  server.on("/config", handleConfigPage);
  server.on("/buttonClicked", handleButtonChange);
  server.on("/configSubmit", handleConfigSubmit);
  server.on("/configRead", handleConfigRead);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop()
{
  //(*MaquinaStep)();
  server.handleClient();

  if (triggerMillis > 0 && pump_state && millis() - triggerMillis >= relayPeriod)
  {
    digitalWrite(RELAY_1, LOW);
    triggerMillis = 0;
  }
}

// Funções Máquina de Estado

void StepInicial(void)
{
  MaquinaStep = StepUm;
}

void StepUm(void)
{
  MaquinaStep = StepInicial;
}

// Funções Página

void handleHomePage(void)
{  
  server.send(200, "text/html", homePage);
}


void handleConfigPage(void)
{
  server.send(200, "text/html", configPage);
}

void handleNotFound(void)
{
  server.send(200, "text/html", "<h1>Não encontrado</h1>");
}

void handleButtonChange(void)
{
  if (!pump_state)
  {
    digitalWrite(RELAY_0, HIGH);
    digitalWrite(RELAY_1, HIGH);

    triggerMillis = millis();
  }
  else
  {
    digitalWrite(RELAY_0, LOW);
    digitalWrite(RELAY_1, LOW);
    triggerMillis = 0;
  }

  pump_state = !pump_state;    
  server.send(200, "text/html", "done");
}

void handleConfigSubmit(void)
{
  String capTime = server.arg("capacitorTime");
  relayPeriod = capTime.toInt() * 1000;
  
  server.send(200, "text/html", "done");
}

void handleConfigRead(void)
{  
  server.send(200, "application/json", "{\"capacitorTime\": " + String(relayPeriod) + "}");
}
