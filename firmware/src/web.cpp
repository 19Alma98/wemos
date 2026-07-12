#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "sensor.h"

ESP8266WebServer server(80);

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>DHT22 Monitor</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
  :root{
    --bg:#151922;
    --surface:#1d2330;
    --surface-2:#242b3a;
    --text:#EDEBE4;
    --muted:#8890A0;
    --temp:#E8A755;
    --hum:#5BC0BE;
    --line:#2A3040;
  }
  *{box-sizing:border-box;}
  html,body{
    margin:0;padding:0;
    background:
      radial-gradient(circle at 15% -10%, rgba(232,167,85,0.10), transparent 45%),
      radial-gradient(circle at 90% 0%, rgba(91,192,190,0.10), transparent 45%),
      var(--bg);
    color:var(--text);
    font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
    min-height:100vh;
  }
  .wrap{max-width:820px;margin:0 auto;padding:32px 20px 56px;}

  .eyebrow{
    font-size:11px;letter-spacing:.18em;text-transform:uppercase;
    color:var(--muted);margin:0 0 4px;
  }
  h1{
    font-size:22px;margin:0 0 28px;font-weight:600;letter-spacing:-.01em;
    display:flex;align-items:center;gap:10px;
  }
  .dot{
    width:8px;height:8px;border-radius:50%;background:var(--hum);
    box-shadow:0 0 0 0 rgba(91,192,190,.6);
    animation:pulse 2s infinite;
  }
  @keyframes pulse{
    0%{box-shadow:0 0 0 0 rgba(91,192,190,.55);}
    70%{box-shadow:0 0 0 8px rgba(91,192,190,0);}
    100%{box-shadow:0 0 0 0 rgba(91,192,190,0);}
  }

  .stats{
    display:grid;grid-template-columns:1fr 1fr;gap:16px;margin-bottom:16px;
  }
  .stat{
    background:var(--surface);
    border:1px solid var(--line);
    border-radius:16px;
    padding:22px 22px 18px;
    position:relative;overflow:hidden;
  }
  .stat .label{
    font-size:11px;letter-spacing:.14em;text-transform:uppercase;
    color:var(--muted);margin-bottom:10px;
  }
  .stat .value{
    font-size:44px;font-weight:700;letter-spacing:-.02em;
    font-variant-numeric:tabular-nums;line-height:1;
  }
  .stat .value sup{font-size:20px;font-weight:500;margin-left:2px;color:var(--muted);}
  .stat.temp .value{color:var(--temp);}
  .stat.hum .value{color:var(--hum);}
  .bar{
    margin-top:16px;height:3px;border-radius:2px;background:var(--surface-2);overflow:hidden;
  }
  .bar i{
    display:block;height:100%;border-radius:2px;
    transition:width .6s ease;
  }
  .stat.temp .bar i{background:linear-gradient(90deg,#E8A755,#f2c98a);}
  .stat.hum .bar i{background:linear-gradient(90deg,#5BC0BE,#8fdbd9);}

  .panel{
    background:var(--surface);
    border:1px solid var(--line);
    border-radius:16px;
    padding:20px 20px 8px;
  }
  .panel .label{
    font-size:11px;letter-spacing:.14em;text-transform:uppercase;
    color:var(--muted);margin-bottom:12px;
  }
  canvas{width:100%!important;}

  footer{
    margin-top:20px;text-align:center;font-size:11px;color:var(--muted);
    letter-spacing:.04em;
  }
  footer span{color:var(--text);}

  @media (max-width:480px){
    .stats{grid-template-columns:1fr;}
    .stat .value{font-size:38px;}
  }
</style>
</head>
<body>
<div class="wrap">
  <p class="eyebrow">Wemos D1 Mini &middot; DHT22</p>
  <h1><span class="dot"></span>Sensor Monitor</h1>

  <div class="stats">
    <div class="stat temp">
      <div class="label">Temperature</div>
      <div class="value" id="t">--<sup>&deg;C</sup></div>
      <div class="bar"><i id="tbar" style="width:0%"></i></div>
    </div>
    <div class="stat hum">
      <div class="label">Humidity</div>
      <div class="value" id="h">--<sup>%</sup></div>
      <div class="bar"><i id="hbar" style="width:0%"></i></div>
    </div>
  </div>

  <div class="panel">
    <div class="label">Last 20 readings</div>
    <canvas id="c" height="130"></canvas>
  </div>

  <footer>Updates every 2s &middot; <span id="status">connecting&hellip;</span></footer>
</div>

<script>
let tdata=[], hdata=[], labels=[];

const ctx=document.getElementById('c');
const chart=new Chart(ctx,{
  type:'line',
  data:{
    labels:labels,
    datasets:[
      {
        label:'Temp (°C)',
        data:tdata,
        borderColor:'#E8A755',
        backgroundColor:'rgba(232,167,85,0.12)',
        tension:0.35,
        pointRadius:3,
        pointHoverRadius:5,
        pointBackgroundColor:'#151922',
        pointBorderColor:'#E8A755',
        pointBorderWidth:2,
        borderWidth:2,
        fill:true,
        yAxisID:'y'
      },
      {
        label:'Humidity (%)',
        data:hdata,
        borderColor:'#5BC0BE',
        backgroundColor:'rgba(91,192,190,0.10)',
        tension:0.35,
        pointRadius:3,
        pointHoverRadius:5,
        pointBackgroundColor:'#151922',
        pointBorderColor:'#5BC0BE',
        pointBorderWidth:2,
        borderWidth:2,
        fill:true,
        yAxisID:'y1'
      }
    ]
  },
  options:{
    responsive:true,
    animation:{duration:300},
    interaction:{intersect:false,mode:'index'},
    plugins:{
      legend:{
        labels:{color:'#8890A0',boxWidth:10,boxHeight:10,usePointStyle:true,pointStyle:'circle'}
      }
    },
    scales:{
      x:{display:false},
      y:{
        position:'left',
        grid:{color:'#242b3a'},
        ticks:{color:'#8890A0'}
      },
      y1:{
        position:'right',
        grid:{display:false},
        ticks:{color:'#8890A0'}
      }
    }
  }
});

function pad(arr,minPad){
  let lo=Math.min(...arr), hi=Math.max(...arr);
  if(hi-lo<minPad){
    let mid=(hi+lo)/2;
    lo=mid-minPad/2; hi=mid+minPad/2;
  }else{
    let p=(hi-lo)*0.15;
    lo-=p; hi+=p;
  }
  return {lo,hi};
}

async function u(){
  try{
    let r=await fetch('/data');
    let d=await r.json();

    document.getElementById('t').innerHTML=d.temp+'<sup>&deg;C</sup>';
    document.getElementById('h').innerHTML=d.hum+'<sup>%</sup>';
    document.getElementById('tbar').style.width=Math.min(100,Math.max(0,(d.temp/40)*100))+'%';
    document.getElementById('hbar').style.width=Math.min(100,Math.max(0,d.hum))+'%';
    document.getElementById('status').textContent='live';

    labels.push('');
    tdata.push(d.temp);
    hdata.push(d.hum);
    if(tdata.length>20){tdata.shift();hdata.shift();labels.shift();}

    let tr=pad(tdata,1);
    let hr=pad(hdata,2);
    chart.options.scales.y.min=tr.lo; chart.options.scales.y.max=tr.hi;
    chart.options.scales.y1.min=hr.lo; chart.options.scales.y1.max=hr.hi;

    chart.update();
  }catch(e){
    document.getElementById('status').textContent='connection lost';
  }
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
