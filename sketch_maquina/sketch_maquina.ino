#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define RELAY_0 16
#define RELAY_1 5

#define ssid "SmartHome"
#define password "sae668128"

#define homePage "<script>\
  const switchButtonAppearance = () => {\
    let button = document.querySelector('button');\
    if (button.textContent === 'LIGAR') {\
      button.textContent = 'DESLIGAR';\
      button.style.backgroundColor = '#D25';\
    }\
    else {\
      button.textContent = 'LIGAR';\
      button.style.backgroundColor = '#ffcb0f';\
    }\
  };\
  \
  const buttonClicked = async () => {\
    await fetch('/buttonClicked').then(() => {switchButtonAppearance()});\
  }\
</script>\
<style>\
  body {\
    background-color: #000;\
    margin: 0;\
  }\
  \
  button {\
    position: absolute;\
    top: 50%;\
    left: 50%;\
    transform: translate(-50%, -50%);\
    width: 80%;\
    font-size: 8vw;\
    font-weight: bold;\
    background-color: #ffcb0f;\
    color: #000;\
    border: none;\
    border-radius: 20px;\
    transition: 200ms;\
  }\
  \
  svg {\
    width: 100px;\
    position: absolute;\
    top: 10px;\
    right: 10px;\
  }\
  \
  .stats {\
    position: absolute;\
    top: 15%;\
    left: 50%;\
    transform: translateX(-50%);\
    color: #FFF;\
    font-size: 5vw;\
    font-family: sans-serif;\
  }\
  .aen {\
    display: flex;\
    align-items: center;\
    justify-content: center;\
    width: 100%;\
    height: 10vh;\
    position: absolute;\
    bottom: 0;\
    background-color: #ffcb0f;\
    color: #000;\
    font-weight: 700;\
    font-family: sans-serif;\
    font-size: 5vw;\
  }\
</style>\
<html>\
  <head><meta charset=\"UTF-8\" /></head>\
  <body>\
    <button onclick=\"buttonClicked()\">LIGAR</button>\
    <a href=\"config\">\
      <svg fill=\"#ffffff\" version=\"1.1\" id=\"Capa_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 45.973 45.973\" xml:space=\"preserve\" stroke=\"#ffffff\" transform=\"rotate(0)\"><g id=\"SVGRepo_bgCarrier\" stroke-width=\"0\"></g><g id=\"SVGRepo_tracerCarrier\" stroke-linecap=\"round\" stroke-linejoin=\"round\"></g><g id=\"SVGRepo_iconCarrier\"> <g> <g> <path d=\"M43.454,18.443h-2.437c-0.453-1.766-1.16-3.42-2.082-4.933l1.752-1.756c0.473-0.473,0.733-1.104,0.733-1.774 c0-0.669-0.262-1.301-0.733-1.773l-2.92-2.917c-0.947-0.948-2.602-0.947-3.545-0.001l-1.826,1.815 C30.9,6.232,29.296,5.56,27.529,5.128V2.52c0-1.383-1.105-2.52-2.488-2.52h-4.128c-1.383,0-2.471,1.137-2.471,2.52v2.607 c-1.766,0.431-3.38,1.104-4.878,1.977l-1.825-1.815c-0.946-0.948-2.602-0.947-3.551-0.001L5.27,8.205 C4.802,8.672,4.535,9.318,4.535,9.978c0,0.669,0.259,1.299,0.733,1.772l1.752,1.76c-0.921,1.513-1.629,3.167-2.081,4.933H2.501 C1.117,18.443,0,19.555,0,20.935v4.125c0,1.384,1.117,2.471,2.501,2.471h2.438c0.452,1.766,1.159,3.43,2.079,4.943l-1.752,1.763 c-0.474,0.473-0.734,1.106-0.734,1.776s0.261,1.303,0.734,1.776l2.92,2.919c0.474,0.473,1.103,0.733,1.772,0.733 s1.299-0.261,1.773-0.733l1.833-1.816c1.498,0.873,3.112,1.545,4.878,1.978v2.604c0,1.383,1.088,2.498,2.471,2.498h4.128 c1.383,0,2.488-1.115,2.488-2.498v-2.605c1.767-0.432,3.371-1.104,4.869-1.977l1.817,1.812c0.474,0.475,1.104,0.735,1.775,0.735 c0.67,0,1.301-0.261,1.774-0.733l2.92-2.917c0.473-0.472,0.732-1.103,0.734-1.772c0-0.67-0.262-1.299-0.734-1.773l-1.75-1.77 c0.92-1.514,1.627-3.179,2.08-4.943h2.438c1.383,0,2.52-1.087,2.52-2.471v-4.125C45.973,19.555,44.837,18.443,43.454,18.443z M22.976,30.85c-4.378,0-7.928-3.517-7.928-7.852c0-4.338,3.55-7.85,7.928-7.85c4.379,0,7.931,3.512,7.931,7.85 C30.906,27.334,27.355,30.85,22.976,30.85z\"></path> </g> </g> </g></svg>\
    </a>\
    <div class=\"stats\">\
      <p>Tensão: </p>\
      <p>Corrente: </p>\
    </div>\
    <div class=\"aen\">AEnSolar</div>\
  </body>\
</html>"

#define configPage "<script>\
  function selectChanged() {\
    let val = document.getElementById('nominalVoltage').value, min = document.getElementById('minVoltage'), max = document.getElementById('maxVoltage');\
    if (val === '220') {\
      min.min = 180;\
      min.max = 200;\
      max.min = 240;\
      max.max = 260;\
    }\
    else {\
      min.min = 342;\
      min.max = 360;\
      max.min = 400;\
      max.max = 418;\
    }\
  }\
  \
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
    selectChanged();\
    document.getElementById('minVoltage').value = json.minVoltage;\
    document.getElementById('maxVoltage').value = json.maxVoltage;\
    document.getElementById('nominalCurrent').value = json.nominalCurrent;\
  }\
  \
  getData();\
</script>\
<style>\
  body {\
    background-color: #000;\
    margin: 0;\
    color: #FFF;\
    margin: 15vw;\
  }\
  \
  div {\
    margin-bottom: 60px;\
  }\
  \
  label {\
    font-size: 4vw;\
    font-family: sans-serif;\
  }\
  \
  input, select {\
    margin-top: 20px;\
    display: block;\
    font-size: 5vw;\
    color: #FFF;\
    background: transparent;\
    border: none;\
    border-bottom: solid 3px #FFF;\
  }\
  \
  option {\
    color: #000;\
    font-size: 3vw;\
  }\
  \
  input[type=\"submit\"] {\
    position: absolute;\
    bottom: 15vw;\
    right: 15vw;\
    padding: 20px;\
    border: none;\
    color: #000;\
    background-color: #ffcb0f;\
    width: 30vw;\
  }\
</style>\
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
        <select id=\"nominalVoltage\" onchange=\"selectChanged()\" name=\"nominalVoltage\">\
          <option value=\"220\">220V</option>\
          <option value=\"380\">380V</option>\
        </select>\
      </div>\
      <div>\
        <label for=\"minVoltage\">Tensão mínima</label>\
        <input type=\"number\" id=\"minVoltage\" name=\"minVoltage\" min=\"180\" max=\"200\" />\
      </div>\
      <div>\
        <label for=\"minVoltage\">Tensão máxima</label>\
        <input type=\"number\" id=\"maxVoltage\" name=\"maxVoltage\" min=\"240\" max=\"260\" />\
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
<style>\
  body {\
    background-color: #000;\
    margin: 0;\
    color: #FFF;\
    margin: 15vw;\
  }\
  \
  div {\
    font-size: 5vw;\
    font-family: sans-serif;\
  }\
</style>\
<html>\
  <head><meta charset=\"UTF-8\"></head>\
  <body>\
    <center><div>Configurações atualizadas com sucesso!<br>Espere para ser redirecionado</div></center>\
  </body>\
</html>"

// Variáveis

ESP8266WebServer server(80);
bool pump_state = false;
unsigned long triggerMillis = 0;

unsigned relayPeriod = 3000,
  nomVoltage = 220,
  minVoltage = 180,
  maxVoltage = 260,
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
  
  IPAddress local_IP(192,168,4,1);
  IPAddress gateway(192,168,4,6);
  IPAddress subnet(255,255,255,0);

  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.println(WiFi.softAP("SmartControl", "12345678") ? "Ready" : "Failed!");

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
  server.send(200, "application/json", "{\"capacitorTime\": " + String(relayPeriod / 1000) + ", \"nominalVoltage\": " + String(nomVoltage) + ", \"minVoltage\": " + String(minVoltage) + ", \"maxVoltage\": " + String(maxVoltage) + ", \"nominalCurrent\": " + String(nomCurrent) + "}");
}

void handleConfigSubmit(void)
{
  relayPeriod = server.arg("capacitorTime").toInt() * 1000;
  nomVoltage = server.arg("nominalVoltage").toInt();
  minVoltage = server.arg("minVoltage").toInt();
  maxVoltage = server.arg("maxVoltage").toInt();
  nomCurrent = server.arg("nominalCurrent").toInt();
  
  server.send(200, "text/html", submitPage);
}
