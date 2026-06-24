
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);//D1 WITH SCL AND D2 WITH SDA

// Buttons
#define WATER_BTN D5
#define FOOD_BTN D6
#define EMERGENCY_BTN D7

// Buzzer
#define BUZZER D3

ESP8266WebServer server(80);
DNSServer dnsServer;

String webStatus = "All is Fine";
unsigned long statusTime = 0;

const byte DNS_PORT = 53;
String lastMessage = "All is Fine";

// ---------- LCD ----------
void showMessage(String msg) {
  if (msg != lastMessage) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lastMessage = msg;
  }
}

// ---------- Buzzer ----------
void waterBeep() {
  for (int i = 0; i < 2; i++) {
    tone(BUZZER, 2000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}

void foodBeep() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 2000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}

void emergencyBeep() {
  while (digitalRead(EMERGENCY_BTN) == LOW) {
    tone(BUZZER, 2500);
    delay(150);
    noTone(BUZZER);
    delay(150);

    tone(BUZZER, 1500);
    delay(2000);
    noTone(BUZZER);
    delay(150);

    dnsServer.processNextRequest();
    server.handleClient();
  }
}

// ---------- Web Page ----------
String webpage() {
  String html = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Glove</title>
<style>
body{
font-family:Arial;
background:#0f172a;
color:white;
text-align:center;
margin:0;
padding:20px;
}
.card{
max-width:500px;
margin:auto;
padding:25px;
border-radius:20px;
background:#1e293b;
box-shadow:0 0 20px rgba(0,0,0,.4);
}
.status{
font-size:32px;
font-weight:bold;
padding:30px;
border-radius:15px;
margin-top:20px;
}
.green{background:#16a34a;}
.blue{background:#2563eb;}
.orange{background:#ea580c;}
.red{
background:#dc2626;
animation:blink 1s infinite;
}
@keyframes blink{
50%{opacity:.5;}
}
</style>
</head>
<body>
<div class="card">
<h1> Smart Glove</h1>
<div id="status" class="status">Loading...</div>
</div>

<script>
function updateStatus(){
fetch('/status')
.then(r=>r.text())
.then(t=>{
let s=document.getElementById('status');
s.innerHTML=t;

if(t.includes('Fine')) s.className='status green';
else if(t.includes('Water')) s.className='status blue';
else if(t.includes('Food')) s.className='status orange';
else s.className='status red';
});
}
setInterval(updateStatus,20);
updateStatus();
</script>
</body>
</html>
)====";
  return html;
}

// ---------- Handlers ----------
void handleRoot() {
  server.send(200, "text/html", webpage());
}

void handleStatus() {
 server.send(200, "text/plain", webStatus);
}

void captivePortal() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/plain", "");
}

// ---------- Setup ----------
void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(WATER_BTN, INPUT_PULLUP);
  pinMode(FOOD_BTN, INPUT_PULLUP);
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  lcd.clear();
  lcd.print("Starting...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Smart Glove");

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  server.on("/generate_204", captivePortal); // Android
  server.on("/hotspot-detect.html", captivePortal); // Apple
  server.on("/fwlink", captivePortal); // Windows
  server.onNotFound(captivePortal);

  server.begin();

  lastMessage = "All is Fine";
  lcd.clear();
  lcd.print("All is Fine");
}

// ---------- Loop ----------
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

 if (digitalRead(WATER_BTN) == LOW)
{
    webStatus = "Need Water";
    statusTime = millis();

    showMessage("Need Water");

    server.handleClient();
    dnsServer.processNextRequest();

    waterBeep();
}
  else if (digitalRead(FOOD_BTN) == LOW)
{
    webStatus = "Need Food";
    statusTime = millis();

    showMessage("Need Food");

    server.handleClient();
    dnsServer.processNextRequest();

    foodBeep();
}
  else if (digitalRead(EMERGENCY_BTN) == LOW)
{
    webStatus = "Emergency";
    statusTime = millis();

    showMessage("Emergency");

    server.handleClient();
    dnsServer.processNextRequest();

    emergencyBeep();
}
  else {
     webStatus = "All is Fine";
    showMessage("All is Fine");
  }

  delay(50);
}
