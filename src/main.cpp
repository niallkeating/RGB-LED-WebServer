/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

//Refactoring Arduino code to be proper C++
String outputState(void);

// Replace with your network credentials
const char* ssid     = "BTHub6-M9XC";
const char* password = "ALMn94YgiEmb";

const char* PARAM_INPUT_1 = "RValue";
const char* PARAM_INPUT_2 = "GValue";
const char* PARAM_INPUT_3 = "BValue";

const int RedGPIO = 5;
const int GreenGPIO = 4;
const int BlueGPIO = 14;

String RedSliderValue = "100";
String GreenSliderValue = "100";
String BlueSliderValue = "100";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RGB LED Web Server</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.4rem;}
    p {font-size: 2.2rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .slider2 { -webkit-appearance: none; margin: 14px; width: 300px; height: 20px; background: #ccc;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider2::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 30px; height: 30px; background: #2f4468; cursor: pointer;}
    .slider2::-moz-range-thumb { width: 30px; height: 30px; background: #2f4468; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>LED Web Server</h2>
  <p>Red: <span id="RedValue">%REDVALUE%</span></p>
  <p><input type="range" onchange="updateRedSlider(this)" id="RedSlider" min="0" max="100" value="%REDVALUE%" step="1" class="slider2"></p>
  <p>Green: <span id="GreenValue">%GREENVALUE%</span></p>
  <p><input type="range" onchange="updateGreenSlider(this)" id="GreenSlider" min="0" max="100" value="%GREENVALUE%" step="1" class="slider2"></p>
  <p>Blue: <span id="BlueValue">%BLUEVALUE%</span></p>
  <p><input type="range" onchange="updateBlueSlider(this)" id="BlueSlider" min="0" max="100" value="%BLUEVALUE%" step="1" class="slider2"></p>
  
<script>
function updateRedSlider(element) {
  var sliderValue = document.getElementById("RedSlider").value;
  document.getElementById("RedValue").innerHTML = sliderValue;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?RValue="+sliderValue, true);
  xhr.send();
}
function updateGreenSlider(element) {
  var sliderValue = document.getElementById("GreenSlider").value;
  document.getElementById("GreenValue").innerHTML = sliderValue;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?GValue="+sliderValue, true);
  xhr.send();
}
function updateBlueSlider(element) {
  var sliderValue = document.getElementById("BlueSlider").value;
  document.getElementById("BlueValue").innerHTML = sliderValue;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?BValue="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String processor(const String& var){
  //Serial.println(var);
  if(var == "REDVALUE"){
    return RedSliderValue;
  }
  else if(var == "GREENVALUE"){
    return GreenSliderValue;
  }
  else if(var == "BLUEVALUE"){
    return BlueSliderValue;
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(RedGPIO, OUTPUT);
  analogWrite(RedGPIO, 0);

  pinMode(GreenGPIO, OUTPUT);
  analogWrite(GreenGPIO, 0);

  pinMode(BlueGPIO, OUTPUT);
  analogWrite(BlueGPIO, 0);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      RedSliderValue = inputMessage;
      analogWrite(RedGPIO, static_cast<int>((1.0 - RedSliderValue.toInt()/100.0)*1023));
    }
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      GreenSliderValue = inputMessage;
      analogWrite(GreenGPIO, static_cast<int>((1.0 - GreenSliderValue.toInt()/100.0)*1023));
    }
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      BlueSliderValue = inputMessage;
      analogWrite(BlueGPIO, static_cast<int>((1.0 - BlueSliderValue.toInt()/100.0)*1023));
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}
  
void loop() {

}