#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define RELAY_0 16
#define RELAY_1 5

#define ssid "SmartHome"
#define password "sae668128"

#define homePage "<script>\
  const buttonClicked = async () => {\
    await fetch('/buttonClicked');\
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
  async function getData() {\
    const res = await fetch('/configRead');\
    const json = await res.json();\
    \
    document.getElementById('capacitorTime').value = json.capacitorTime;\
    if (json.nominalVoltage === 220) {\
      document.getElementById('nominalVoltage').children[0].selected = true;\
      document.getElementById('nominalVoltage').children[1].selected = false;\
    }\
    else {\
      document.getElementById('nominalVoltage').children[0].selected = false;\
      document.getElementById('nominalVoltage').children[1].selected = true;\
    }\
    document.getElementById('nominalCurrent').value = json.nominalCurrent;\
  }\
  \
  getData();\
</script>\
<html>\
  <head><meta charset=\"UTF-8\"></head>\
  <body>\
    <form action=\"/configSubmit\">\
      <div>\
        <label for=\"capacitorTime\">Tempo do capacitor</label>\
        <input type=\"number\" id=\"capacitorTime\" name=\"capacitorTime\" min=\"2\" max=\"6\" value=\"capval\" />\
      </div>\
      <div>\
        <label for=\"capacitorTime\">Tensão nominal</label>\
        <select id=\"nominalVoltage\" name=\"nominalVoltage\">\
          <option value=\"220\">220V</option>\
          <option value=\"380\">380V</option>\
        </select>\
      </div>\
      <div>\
        <label for=\"capacitorTime\">Corrente nominal</label>\
        <input type=\"number\" id=\"nominalCurrent\" name=\"nominalCurrent\" min=\"3\" max=\"10\" value=\"nomCur\" />\
      </div>\
      <input type=\"submit\" value=\"Salvar\" />\
    </form>\
  </body>\
</html>"

#define submitPage "<script>\
  setTimeout(() => {\
    window.location.replace('/');\
  }, 3000);\
</script>\
<html>\
  <head><meta charset=\"UTF-8\"></head>\
  <body>\
    Configurações atualizadas com sucesso!<br>Espere para ser redirecionado\
  </body>\
</html>"

// Variáveis

ESP8266WebServer server(80);
bool pump_state = false;
unsigned long triggerMillis = 0;

unsigned relayPeriod = 3000,
  nomVoltage = 220,
  nomCurrent = 8;

// Protótipos

void handleHomePage(void);
void handleNotFound(void);
void handleConfigPage(void);
void handleButtonChange(void);
void handleConfigRead(void);
void handleConfigSubmit(void);

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

void handleConfigRead(void)
{
  server.send(200, "application/json", "{\"capacitorTime\": " + String(relayPeriod / 1000) + ", \"nominalVoltage\": " + String(nomVoltage) + ", \"nominalCurrent\": " + String(nomCurrent) + "}");
}

void handleConfigSubmit(void)
{
  relayPeriod = server.arg("capacitorTime").toInt() * 1000;
  nomVoltage = server.arg("nominalVoltage").toInt();
  nomCurrent = server.arg("nominalCurrent").toInt();
  
  server.send(200, "text/html", submitPage);
}
