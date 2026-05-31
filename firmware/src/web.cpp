#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "sensor.h"

ESP8266WebServer server(80);

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body style="background:#111;color:white;text-align:center;">
<h2>DHT22 Dashboard</h2>
<div id="t">--</div>
<div id="h">--</div>
<canvas id="c"></canvas>

<script>
let tdata=[], labels=[];
const ctx=document.getElementById('c');
const chart=new Chart(ctx,{type:'line',data:{labels:labels,datasets:[{data:tdata}]}});
async function u(){
let r=await fetch('/data');
let d=await r.json();
document.getElementById('t').innerText=d.temp;
document.getElementById('h').innerText=d.hum;
labels.push(Date.now());
tdata.push(d.temp);
if(tdata.length>20){tdata.shift();labels.shift();}
chart.update();
}
setInterval(u,2000);
u();
</script>
</body>
</html>
)rawliteral";

void handleRoot(){
  server.send_P(200,"text/html",PAGE);
}

void handleData(){
  float t=getTemp();
  float h=getHum();

  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
  server.send(200,"application/json",json);
}

void handleTest(){
  server.send(200,"text/plain","OK");
}

void webBegin(){
  server.on("/",handleRoot);
  server.on("/data",handleData);
  server.on("/test", handleTest);
  server.begin();
}

void webLoop(){
  server.handleClient();
}
