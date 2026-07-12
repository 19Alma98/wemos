#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "sensor.h"

ESP8266WebServer server(80);

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Monitor DHT22</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
  :root{
    --bg:#12151c;
    --surface:#1a1f2a;
    --surface-2:#222939;
    --text:#EDEBE4;
    --muted:#818a9c;
    --temp:#E8A755;
    --hum:#5BC0BE;
    --line:#272e3f;
    --good:#6FCF97;
    --bad:#E8687A;
  }
  *{box-sizing:border-box;}
  html,body{
    margin:0;padding:0;
    background:
      radial-gradient(circle at 12% -10%, rgba(232,167,85,0.09), transparent 45%),
      radial-gradient(circle at 92% 0%, rgba(91,192,190,0.09), transparent 45%),
      var(--bg);
    color:var(--text);
    font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
    min-height:100vh;
  }
  .wrap{max-width:860px;margin:0 auto;padding:32px 20px 56px;}

  .topbar{
    display:flex;align-items:flex-start;justify-content:space-between;
    margin-bottom:30px;gap:12px;
  }
  .eyebrow{
    font-size:11px;letter-spacing:.18em;text-transform:uppercase;
    color:var(--muted);margin:0 0 6px;
  }
  h1{
    font-size:22px;margin:0;font-weight:600;letter-spacing:-.01em;
  }
  .chip{
    display:flex;align-items:center;gap:7px;
    font-size:11px;letter-spacing:.06em;text-transform:uppercase;
    color:var(--muted);
    background:var(--surface);border:1px solid var(--line);
    border-radius:100px;padding:6px 12px 6px 10px;
    white-space:nowrap;margin-top:2px;
  }
  .dot{
    width:7px;height:7px;border-radius:50%;background:var(--good);
    box-shadow:0 0 0 0 rgba(111,207,151,.6);
    animation:pulse 2s infinite;
    flex-shrink:0;
  }
  .dot.off{background:var(--bad);animation:none;box-shadow:none;}
  @keyframes pulse{
    0%{box-shadow:0 0 0 0 rgba(111,207,151,.55);}
    70%{box-shadow:0 0 0 7px rgba(111,207,151,0);}
    100%{box-shadow:0 0 0 0 rgba(111,207,151,0);}
  }

  .dials{
    display:grid;grid-template-columns:1fr 1fr;gap:16px;margin-bottom:16px;
  }
  .dial-card{
    background:var(--surface);
    border:1px solid var(--line);
    border-radius:18px;
    padding:22px;
    display:flex;align-items:center;gap:18px;
  }
  .dial{
    position:relative;
    width:104px;height:104px;flex-shrink:0;
    border-radius:50%;
    background:conic-gradient(var(--ring-color,var(--temp)) calc(var(--pct,0)*1%), var(--surface-2) 0);
    display:flex;align-items:center;justify-content:center;
    transition:background 0.6s ease;
  }
  .dial::before{
    content:"";
    position:absolute;inset:9px;
    background:var(--surface);
    border-radius:50%;
  }
  .dial .reading{
    position:relative;z-index:1;
    font-family:'SFMono-Regular',Consolas,'Liberation Mono',Menlo,monospace;
    font-size:19px;font-weight:600;font-variant-numeric:tabular-nums;
    text-align:center;line-height:1.1;
  }
  .dial .reading sup{font-size:11px;font-weight:500;color:var(--muted);}
  .dial-card.temp .dial{--ring-color:var(--temp);}
  .dial-card.hum .dial{--ring-color:var(--hum);}
  .dial-card.temp .reading{color:var(--temp);}
  .dial-card.hum .reading{color:var(--hum);}

  .dial-meta .label{
    font-size:11px;letter-spacing:.14em;text-transform:uppercase;
    color:var(--muted);margin-bottom:6px;
  }
  .dial-meta .range{
    font-size:12px;color:var(--muted);
    font-variant-numeric:tabular-nums;
  }
  .dial-meta .trend{
    font-size:12px;margin-top:4px;font-weight:600;
  }
  .trend.up{color:var(--temp);}
  .trend.down{color:var(--hum);}
  .trend.flat{color:var(--muted);font-weight:400;}

  .panel{
    background:var(--surface);
    border:1px solid var(--line);
    border-radius:18px;
    padding:20px 20px 8px;
  }
  .panel-head{
    display:flex;align-items:baseline;justify-content:space-between;
    margin-bottom:14px;
  }
  .panel .label{
    font-size:11px;letter-spacing:.14em;text-transform:uppercase;
    color:var(--muted);
  }
  .panel .sub{font-size:11px;color:var(--muted);}
  canvas{width:100%!important;}

  footer{
    margin-top:22px;text-align:center;font-size:11px;color:var(--muted);
    letter-spacing:.04em;
  }
  footer .ip{
    font-family:'SFMono-Regular',Consolas,'Liberation Mono',Menlo,monospace;
    color:var(--text);
  }

  @media (max-width:480px){
    .dials{grid-template-columns:1fr;}
    .dial-card{padding:18px;}
  }
</style>
</head>
<body>
<div class="wrap">
  <div class="topbar">
    <div>
      <p class="eyebrow">Wemos D1 Mini &middot; DHT22</p>
      <h1>Monitor Sensore</h1>
    </div>
    <div class="chip"><span class="dot" id="statusDot"></span><span id="status">connessione&hellip;</span></div>
  </div>

  <div class="dials">
    <div class="dial-card temp">
      <div class="dial" id="tdial">
        <div class="reading" id="t">--<sup>&deg;C</sup></div>
      </div>
      <div class="dial-meta">
        <div class="label">Temperatura</div>
        <div class="range" id="trange">min -- &middot; max --</div>
        <div class="trend flat" id="ttrend">stabile</div>
      </div>
    </div>
    <div class="dial-card hum">
      <div class="dial" id="hdial">
        <div class="reading" id="h">--<sup>%</sup></div>
      </div>
      <div class="dial-meta">
        <div class="label">Umidit&agrave;</div>
        <div class="range" id="hrange">min -- &middot; max --</div>
        <div class="trend flat" id="htrend">stabile</div>
      </div>
    </div>
  </div>

  <div class="panel">
    <div class="panel-head">
      <div class="label">Ultime 20 letture</div>
      <div class="sub">ogni 2s</div>
    </div>
    <canvas id="c" height="130"></canvas>
  </div>

  <footer>Lettura ogni 2s da <span class="ip" id="host">device</span></footer>
</div>

<script>
let tdata=[], hdata=[], labels=[];
document.getElementById('host').textContent=location.host;

const ctx=document.getElementById('c');
const chart=new Chart(ctx,{
  type:'line',
  data:{
    labels:labels,
    datasets:[
      {
        label:'Temp. (°C)',
        data:tdata,
        borderColor:'#E8A755',
        backgroundColor:'rgba(232,167,85,0.12)',
        tension:0.35,
        pointRadius:3,
        pointHoverRadius:5,
        pointBackgroundColor:'#12151c',
        pointBorderColor:'#E8A755',
        pointBorderWidth:2,
        borderWidth:2,
        fill:true,
        yAxisID:'y'
      },
      {
        label:'Umidità (%)',
        data:hdata,
        borderColor:'#5BC0BE',
        backgroundColor:'rgba(91,192,190,0.10)',
        tension:0.35,
        pointRadius:3,
        pointHoverRadius:5,
        pointBackgroundColor:'#12151c',
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
        labels:{color:'#818a9c',boxWidth:10,boxHeight:10,usePointStyle:true,pointStyle:'circle'}
      }
    },
    scales:{
      x:{display:false},
      y:{
        position:'left',
        grid:{color:'#222939'},
        ticks:{color:'#818a9c'}
      },
      y1:{
        position:'right',
        grid:{display:false},
        ticks:{color:'#818a9c'}
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

function setTrend(el,delta,upWord,downWord){
  el.classList.remove('up','down','flat');
  if(Math.abs(delta)<0.05){
    el.classList.add('flat'); el.textContent='stabile';
  }else if(delta>0){
    el.classList.add('up'); el.textContent='in salita · +'+delta.toFixed(1);
  }else{
    el.classList.add('down'); el.textContent='in discesa · '+delta.toFixed(1);
  }
}

async function u(){
  try{
    let r=await fetch('/data');
    let d=await r.json();

    document.getElementById('t').innerHTML=d.temp+'<sup>&deg;C</sup>';
    document.getElementById('h').innerHTML=d.hum+'<sup>%</sup>';
    document.getElementById('tdial').style.setProperty('--pct',Math.min(100,Math.max(0,(d.temp/40)*100)));
    document.getElementById('hdial').style.setProperty('--pct',Math.min(100,Math.max(0,d.hum)));
    document.getElementById('status').textContent='live';
    document.getElementById('statusDot').classList.remove('off');

    let prevT=tdata.length?tdata[tdata.length-1]:d.temp;
    let prevH=hdata.length?hdata[hdata.length-1]:d.hum;

    labels.push('');
    tdata.push(d.temp);
    hdata.push(d.hum);
    if(tdata.length>20){tdata.shift();hdata.shift();labels.shift();}

    let tr=pad(tdata,1);
    let hr=pad(hdata,2);
    chart.options.scales.y.min=tr.lo; chart.options.scales.y.max=tr.hi;
    chart.options.scales.y1.min=hr.lo; chart.options.scales.y1.max=hr.hi;
    chart.update();

    let tmin=Math.min(...tdata).toFixed(1), tmax=Math.max(...tdata).toFixed(1);
    let hmin=Math.min(...hdata).toFixed(1), hmax=Math.max(...hdata).toFixed(1);
    document.getElementById('trange').innerHTML='min '+tmin+'&deg; &middot; max '+tmax+'&deg;';
    document.getElementById('hrange').innerHTML='min '+hmin+'% &middot; max '+hmax+'%';
    setTrend(document.getElementById('ttrend'),d.temp-prevT);
    setTrend(document.getElementById('htrend'),d.hum-prevH);
  }catch(e){
    document.getElementById('status').textContent='connessione persa';
    document.getElementById('statusDot').classList.add('off');
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
