

#include <WiFi.h>                                     // needed to connect to WiFi
#include <WebServer.h>                                // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <WebSocketsServer.h>                         // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>                              // needed for JSON encapsulation (send multiple variables with one string)
#include <HTTPClient.h>

#include <Wire.h>
#include "SparkFun_SHTC3.h" // Click here to get the library: http://librarymanager/All#SparkFun_SHTC3
#include "AGS02MA.h" //https://github.com/RobTillaart/AGS02MA

#include "driver/adc.h" //ESP32 ADC channels configuration
#include "FIRFilter.h" // Moving Average filter (by Phil's Lab)


int tempSHTC3_scaled = 0;
int humSHTC3_scaled = 0;
int AGS10_scaled = 0;
int Ev_scaled = 0;

// SSID and password of Wifi connection:
const char* ssid =   "ESP32_CUSTOM_BOARD";
const char* password =  "123456789";  

//Your Domain name with URL path or IP address with path
String serverName = "YOUR_DOMAIN"; //See https://www.youtube.com/watch?v=SUFxL1awjO8 on how to form the URL of your own Google Sheet. Example: "https://docs.google.com/forms/d/xxxxxxxxxxxxxxxxxxxxx/formResponse"

// Configure IP addresses of the local access point
//// ACCESS POINT////
//IPAddress local_IP(192,168,1,22);
//IPAddress gateway(192,168,1,5);
//IPAddress subnet(255,255,255,0);
/////// AP/////////

// The String below "webpage" contains the complete HTML code that is sent to the client whenever someone connects to the webserver
String webpage = "<!DOCTYPE html><html><head><title>ESP32-C3-MINI-1 SENSOR NODE</title><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'><link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'><link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/@fortawesome/fontawesome-free@5.15.4/css/fontawesome.min.css' integrity='sha384-jLKHWM3JRmfMU0A5x5AkjWkw/EYfGUAGagvnfryNV3F9VqM98XiIH7VBGVoxVSc7' crossorigin='anonymous'><link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta2/css/all.min.css' integrity='sha512-YWzhKL2whUzgiheMoBFwW8CKV4qpHQAEuvilg9FAn5VJUDwKZZxkJNuGM4XkWuk94WCrrwslk8yWNGmY1EduTA==' crossorigin='anonymous' referrerpolicy='no-referrer' /><title> ESP32-C3-MINI-1Sensor Node</title><style>html,body,h1,h2,h3,h4,h5 {font-family: 'Raleway', sans-serif}</style></head><body class='w3-light-grey':><!-- !PAGE CONTENT! --><div class='w3-main' style='margin-left:10px;margin-top:43px;'> <!-- Header --> <header class='w3-container' style='padding-top:22px;'> <h5><b><i class='fa fa-dashboard'></i> Sensor Node Measurements</b></h5> </header> <div class='w3-row-padding w3-margin-bottom'> <div class='w3-quarter'> <div class='w3-container w3-red w3-padding-16'> <div class='w3-left'><i class='fa fa-temperature-low' style='font-size:36px;'></i></div> <div class='w3-right'> <h3><span id='SHTC3_t'>-</span></h3> </div> <div class='w3-clear'></div> <h4>Temperature&emsp; °C</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-blue w3-padding-16'> <div class='w3-left'><i class='fas fa-tint w3-xxxlarge'></i></div> <div class='w3-right'> <h3><span id='SHTC3_h'>-</span></h3> </div> <div class='w3-clear'></div> <h4>Humidity &emsp;RH%</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-teal w3-padding-16'> <div class='w3-left'><i class='fas fa-fire w3-xxxlarge'></i></div> <div class='w3-right'> <h3><span id='AGS10'>-</span></h3> </div> <div class='w3-clear'></div> <h4>TVOC &emsp; PPM</h4> </div> </div> <div class='w3-quarter'> <div class='w3-container w3-orange w3-text-white w3-padding-16'> <div class='w3-left'><i class='fas fa-sun w3-xxxlarge'></i></div> <div class='w3-right'> <h3><span id='Ev'>-</span></h3> </div> <div class='w3-clear'></div> <h4>Sun Light &emsp; LUX</h4> </div> </div> </div><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p> <hr> <div class='w3-container'> <h5>Total Volatile Organic Compounds</h5> <p>Unhealthy</p> <div class='w3-grey'> <div class='w3-container w3-center w3-padding w3-red' style='width:100%'>2.2 - 5.5 PPM</div> </div> <p>Poor</p> <div class='w3-grey'> <div class='w3-container w3-center w3-padding w3-orange' style='width:100%'>0.66 - 2.2 PPM</div> </div> <p>Moderate</p> <div class='w3-grey'> <div class='w3-container w3-center w3-padding w3-yellow' style='width:100%'>0.25 - 0.66 PPM</div> </div> <p>Good</p> <div class='w3-grey'> <div class='w3-container w3-center w3-padding w3-green' style='width:100%'>0.065 - 0.25 PPM</div> </div><p>Excellent</p> <div class='w3-grey'> <div class='w3-container w3-center w3-padding w3-blue' style='width:100%'>0 - 0.065 PPM</div> </div> </div> </div> <h5>Suggested Conditions</h5> <table class='w3-table w3-striped w3-bordered w3-border w3-hoverable w3-light-gray'> <tr> <td>Strawberries</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> <tr> <td>Blueberries</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> <tr> <td>Lettuce</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> <tr> <td>Bell Pepers</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> <tr> <td>Basil</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> <tr> <td>Saffron</td> <td>Temperature</td><td> - °C</td><td>Humidity</td><td> - RH%</td><td>Sun Light</td><td> - LUX </td> </tr> </table><br> <hr> </div> <br> <div class='w3-container w3-dark-grey w3-padding-32'> <div class='w3-row'> <div class='w3-container w3-third'> <h5 class='w3-bottombar w3-border-green'>MCU</h5><h5 class=''>Espressif ESP32-C3-MINI-1</h5> <p>Small­sized 2.4 GHz Wi­Fi (802.11 b/g/n) and Bluetooth® 5 module15 GPIOsOn­board PCB antenna or external antenna connector</p> <p>Built around ESP32­C3 series of SoCs, RISC­V single­core microprocessor4 MB flash in chip package</p> <p>15 GPIOs</p><p>On­board PCB antenna or external antenna connector</p> <h16 class=''>https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_en.pdf</h16> </div> <div class='w3-container w3-third'> <h5 class='w3-bottombar w3-border-red'>On Board Sensors</h5><h6 class='w3-border-red'>→SHTC3Humidity and Temperature Sensor IC</h6> <p>Ultra-low power consumption, Full battery supply voltage range (1.62 - 3.6 V), Small DFN package: 2 × 2 × 0.75 mm3, I2C interface.</p><h6 class='w3-border-red'>→AGS10 TVOC Sensor</h6> <p>Long-term stability, low power consumption, high sensitivity, I2C interface.</p><h6 class='w3-border-red'>→BH1621FVC Analog Current Output Type Ambient Light Sensor IC</h6> <p>Compact surface mount package 1.6 × 1.6 mm, Output current in proportion to brightness, 2 steps controllable output current gain, Supply voltage operates from 2.4V to 5.5V</p> </div> <div class='w3-container w3-third'> <h5 class='w3-bottombar w3-border-orange'>Communication Protocols</h5> <p>WiFi</p> <p>Bluetooth</p> <p>RS232</p> <p>UART</p> </div> </div> </div></div></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {button_status: 'Pressed'};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);var SHTC3_t = obj.SHTC3_t;var SHTC3_h = obj.SHTC3_h;var AGS10 = obj.AGS10;var Ev = obj.Ev;SHTC3_t = SHTC3_t.toFixed(2);document.getElementById('SHTC3_t').innerHTML = SHTC3_t;SHTC3_h = SHTC3_h.toFixed(2);document.getElementById('SHTC3_h').innerHTML = SHTC3_h;AGS10 = AGS10.toFixed(3);document.getElementById('AGS10').innerHTML = AGS10;Ev = Ev.toFixed(2);document.getElementById('Ev').innerHTML = Ev; console.log(obj.SHTC3_t);console.log(obj.SHTC3_h); } window.onload = function(event) { init(); }</script></html>";

int interval = 1000;                                  // send data to the client every 1000ms -> 1s
unsigned long previousMillis = 0;                     // we use the "millis()" command for time reference and this will output an unsigned long
unsigned long previousMillis2 = 0;                     // we use the "millis()" command for time reference and this will output an unsigned long
// Initialization of webserver and websocket
WebServer server(80);                                 // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81);    // the websocket uses port 81 (standard port for websockets



/// Definitions for Analog Ambient Light Sensor (BH1621FVC) ///
#define Vmax 2.5 //Maximum measurable input analog voltage (ADC_ATTEN_DB_11 :  0 mV ~ 2500 mV)
#define Dmax 4095 //Maximum of the output ADC raw digital reading result, which is 4095 under Single Read mode, 4095 under Continuous Read mode.

#define R1 470  //Output resistor of Ambient Light Sensor (see schematic)
/*
 *  GC2 GC1 Mode (BH1621FVC Datasheet p. 4)
 *  0    0 Shutdown 
 *  0    1 H-Gain Mode 
 *  1    0 L-Gain Mode 
 *  1    1 Shutdown
 */
#define GC1 5 // ESP32 output pin for GC1 control
#define GC2 4 // ESP32 output pin for GC2 control

FIRFilter AmbientFilterMovingAverage;

float Viout = 0; // Voltage output of ambient light sensor.
float Dout = 0; // ADC raw digital reading result.
float Ev = 0; //Value of measured light in LUX

adc1_channel_t channel = ADC1_CHANNEL_3; 
adc_atten_t atten = ADC_ATTEN_DB_11; //Measurable input voltage range 0 mV ~ 2500 mV

///// Convert ADC readings to LUX  ////
void lux_calc()
{
FIRFilter_Update(&AmbientFilterMovingAverage, Dout);  
Dout = adc1_get_raw(ADC1_CHANNEL_3);
Viout = (AmbientFilterMovingAverage.out * Vmax) / Dmax;
Ev = Viout / (0.063 * 0.000001 * R1);
Ev_scaled = round(100 * Ev);
}

void BH1621FVC_setup()
{
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(channel, atten);
  FIRFilter_Init(&AmbientFilterMovingAverage);
  pinMode(GC2, OUTPUT);
  pinMode(GC1, OUTPUT);
}

void BH1621FVC_printInfo()
{
//Serial.print("EV = ");
//Serial.print(Ev);
//Serial.print("\t");
//Serial.print("    Viout = ");
//Serial.print(Viout);
//Serial.print("    AmbientFilterMovingAverage = ");
//Serial.print(AmbientFilterMovingAverage.out);
//Serial.print("    Dout = ");
//Serial.println(Dout);
}
/////////// End of Definitions for Analog Ambient Light Sensor  /////////

#define I2C_SDA 7 // ESP32 pin 7 as I2C_SDA
#define I2C_SCL 6 // ESP32 pin 6 as I2C_SCL
#define I2C_Freq 15000 // Set I2C bus frequency to 15kHz (AGS10 operates at speeds of 15kHz or lower)

#define USR_LED 10

///////////////////////
// Utility Functions //
///////////////////////

SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class

void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : break;//Serial.print("Nominal"); break;
    case SHTC3_Status_Error :   break; //Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail :  break; //Serial.print("CRC Fail"); break;
    default : break;  //Serial.print("Unknown return code"); break;
  }
}

void printInfo()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // You can also assess the status of the last command by checking the ".lastStatus" member of the object
  {
    //Serial.print("RH = "); 
    //Serial.print(mySHTC3.toPercent());                        // "toPercent" returns the percent humidity as a floating point number
    //Serial.print("%, T = "); 
    //Serial.print(mySHTC3.toDegC());                           // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively 
    //Serial.println(" deg C"); 
  }
  else
  {
   // Serial.print("Update failed, error: "); 
    errorDecoder(mySHTC3.lastStatus);
    //Serial.println();
  }
}



void SHTC3_setup() {
                                   // Begin Serial 
  //while(Serial == false){};                                   // Wait for the serial connection to start up
  //Serial.println("SHTC3 Example 1 - Basic Readings");    // Title
  //Serial.print("Beginning sensor. Result = ");           // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution 
  errorDecoder(mySHTC3.begin());                              // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  //Serial.println();
  //Serial.println("\n\n");
  //Serial.println("Waiting for 5 seconds so you can read this info ^^^");

  delay(5000);                                                // Give time to read the welcome message and device ID. 
}

void SHTC3_loop() {
  SHTC3_Status_TypeDef result = mySHTC3.update();             // Call "update()" to command a measurement, wait for measurement to complete, and update the RH and T members of the object
  printInfo();                                                // This function is used to print a nice little line of info to the serial port
                                                   // Delay for the data rate you want - note that measurements take ~10 ms so the fastest data rate is 100 Hz (when no delay is used)
  tempSHTC3_scaled = round( 100 * mySHTC3.toDegC());
  humSHTC3_scaled = round( 100 * mySHTC3.toPercent());
}



AGS02MA AGS(26);

void AGS10_setup()
{
  
  //Serial.println(__FILE__);
 // Serial.print("AGS02MA_LIB_VERSION: ");
  //Serial.println(AGS02MA_LIB_VERSION);
  //Serial.println();

  bool b = AGS.begin();
  //Serial.print("BEGIN:\t");
  //Serial.println(b);

  //Serial.print("VERSION:\t");
  //Serial.println(AGS.getSensorVersion());


  b = AGS.setPPBMode();
  uint8_t m = AGS.getMode();
  //Serial.print("MODE:\t");
  //Serial.print(b);
  //Serial.print("\t");
  //Serial.println(m);
}


void AGS10_loop()
{
    delay(1000);
  //Serial.print("PPM:\t");
  //Serial.print(AGS.readPPM(), 3);
  //Serial.print("\t");
  AGS10_scaled = AGS.readPPM();
  AGS10_scaled = 1000 * AGS10_scaled;
  
//Serial.print(AGS.dataReady(), HEX);
//Serial.print("\t");
//Serial.print(AGS.lastStatus(), HEX);
//Serial.print("\t");
//Serial.print(AGS.lastError(), HEX);
//Serial.println();
}


void setup() {
  Serial.begin(115200);                               // init serial port for debugging
 /*
  Serial.print("Setting up Access Point ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Starting Access Point ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");

  Serial.print("IP address = ");
  Serial.println(WiFi.softAPIP());
  */
  
    WiFi.begin(ssid, password);                         // start WiFi interface
  //Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));     // print SSID to the serial interface for debugging
 
  while (WiFi.status() != WL_CONNECTED) {             // wait until WiFi is connected
    delay(1000);
    //Serial.print(".");
  }
 // Serial.print("Connected to network with IP address: ");
 // Serial.println(WiFi.localIP());                     // show IP address that the ESP32 has received from router
 
  server.on("/", []() {                               // define here wat the webserver needs to do
    server.send(200, "text/html", webpage);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  
  server.begin();                                     // start server
  
  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);                  // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_Freq);
  AGS10_setup();
  SHTC3_setup();
  BH1621FVC_setup();
  //pinMode(USR_LED, OUTPUT);
}

void loop() {
  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop();                                   // Update function for the webSockets 
  
  unsigned long now = millis();                       // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) > interval) { // check if "interval" ms has passed since last time the clients were updated
    
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["SHTC3_t"] = mySHTC3.toDegC();                    // write data into the JSON object -> I used "SHTC3_t" and "SHTC3_h" here, but you can use anything else
    object["SHTC3_h"] = mySHTC3.toPercent();
    object["AGS10"] = AGS.readPPM();
    object["Ev"] = Ev;
    serializeJson(doc, jsonString);                   // convert JSON object to string
  //  Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
    webSocket.broadcastTXT(jsonString);               // send JSON string to clients

    /////---////
  SHTC3_loop();
  AGS10_loop();
  digitalWrite(GC2, HIGH);
  digitalWrite(GC1, LOW);
  lux_calc();
  BH1621FVC_printInfo();
    ////----///


    previousMillis = now;                             // reset previousMillis
  }

   if ((unsigned long)(now - previousMillis2) > 600000) { // check if "interval" ms has passed since last time the clients were updated

      //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

  String serverPath = serverName + "?entry.xxxxxxxx=" + String(tempSHTC3_scaled) + "&entry.xxxxxxxx=" + String(humSHTC3_scaled) + "&entry.xxxxxxxx=" + String(AGS.readPPM(),3) + "&entry.xxxxxxxx=" + String(Ev_scaled); // See "https://docs.google.com/forms/d/1Zobf7Q9O97v2UY1hLgVSHluPp4rJjfVSQzWjaI8qJsc/formResponse" on how to form the URL for Each entry you want to send.
  //Serial.println(serverPath);

  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());

  // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        String payload1 = http.getString();
        //Serial.println(payload1);
      }
      else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      //Serial.println("WiFi Disconnected");
    }

  previousMillis2 = now;
    }
}
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type) {                                     // switch on the type of information sent
    case WStype_DISCONNECTED:                         // if a client is disconnected, then type == WStype_DISCONNECTED
      //Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:                            // if a client is connected, then type == WStype_CONNECTED
      //Serial.println("Client " + String(num) + " connected");
      // optionally you can add code here what to do when connected
      break;
    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      // try to decipher the JSON string received
      StaticJsonDocument<200> doc;                    // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        //Serial.print(F("deserializeJson() failed: "));
        //Serial.println(error.f_str());
        return;
      }
      else {
        // JSON string was received correctly, so information can be retrieved:
        const char* button_status = doc["button_status"];
        //Serial.println("button status: " + String(num));
        //Serial.println("button status: " + String(button_status));

      }
     // Serial.println("");
      break;
  }
}
