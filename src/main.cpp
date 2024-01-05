#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Create servo object to control a servo
Servo myServo;

// GPIO the servo is attached to
static const int servoPin = 0;

// GPIO the motors are attached to
static const int M1forwardPin = 14;
static const int M1backwardPin = 12;
static const int M2forwardPin = 5;
static const int M2backwardPin = 4;
static const int sleepPin = 16;

// RPM values for the motor speed
#define M1SPEED 200
#define M2SPEED 200

// Replace with your network credentials
const char* ssid     = "DevOffice2G";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(M1forwardPin, OUTPUT);
  pinMode(M1backwardPin, OUTPUT);
  pinMode(M2forwardPin, OUTPUT);
  pinMode(M2backwardPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  myServo.attach(servoPin);

  digitalWrite(sleepPin, HIGH); 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void forward() {
  analogWrite(M1forwardPin, M1SPEED);
  analogWrite(M2forwardPin, M2SPEED);
  analogWrite(M1backwardPin, 0);
  analogWrite(M2backwardPin, 0);
}
void backward() {
  analogWrite(M1backwardPin, M1SPEED);
  analogWrite(M2backwardPin, M2SPEED);
  analogWrite(M1forwardPin, 0);
  analogWrite(M2forwardPin, 0);
}
void turnLeft() {
  analogWrite(M1backwardPin, M1SPEED);
  analogWrite(M2forwardPin, M2SPEED);
  analogWrite(M1forwardPin, 0);
  analogWrite(M2backwardPin, 0);
}
void turnRight() {
  analogWrite(M1forwardPin, M1SPEED);
  analogWrite(M2backwardPin, M2SPEED);
  analogWrite(M1backwardPin, 0);
  analogWrite(M2forwardPin, 0);
}
void stopMotors() {
  analogWrite(M1forwardPin, 0);
  analogWrite(M2forwardPin, 0);
  analogWrite(M1backwardPin, 0);
  analogWrite(M2backwardPin, 0);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Display the HTML web page
            client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // Style section for buttons and slider. You can change customizations such as color and so on.
            client.println("<style>");
            client.println(".container {font: caption; display: block; flex-direction: column; align-items: center;}");
            client.println(".slider {width: 275px; accent-color: #00ffea; cursor: pointer;}");
            client.println(".button-container {display: flex; flex-direction: column; align-items: center;}");
            client.println(".row-container {display: flex; flex-direction: row; justify-content: center; align-items: center;}");
            client.println(".button {width: 135px; border: 0; background-color: #ede100; box-shadow: inset 2px 2px 3px #b2b2b2, inset -2px -2px 3px #000; padding: 10px 25px; text-align: center; display: inline-block; border-radius: 5px; font-size: 16px; margin: 10px 25px; cursor: pointer;}");
            client.println(".button:hover {background-color: #fff200;}");
            client.println(".button:active{box-shadow: inset 2px 2px 3px #000, inset -2px -2px 3px #b2b2b2;}");
            client.println("footer {text-align: center; margin: 50px; width: 400px; background-color: #ede100;}");
            client.println("</style><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script></head>");
            // Body section. Contains functions of buttons and slider.
            client.println("<body>"); 
            client.println("<div class=\"container\">");
            client.println("<div class=\"row-container\"><h2>ESP8266 Servo Control Panel</h2></div>");
            client.println("<div><div class=\"button-container\">");
            client.println("<div class=\"row-container\"><button class=\"button\" id=\"forward-btn\" onmousedown=\"btnVal('forward')\" onmouseup=\"btnVal('stop')\">Forward</button></div>");
            client.println("<div class=\"row-container\"><button class=\"button\" id=\"left-btn\" onmousedown=\"btnVal('left')\" onmouseup=\"btnVal('stop')\">Left</button>");
            client.println("<button class=\"button\" id=\"right-btn\" onmousedown=\"btnVal('right')\" onmouseup=\"btnVal('stop')\">Right</button></div>");
            client.println("<div class=\"row-container\"><button class=\"button\" id=\"backward-btn\" onmousedown=\"btnVal('backward')\" onmouseup=\"btnVal('stop')\">Backward</button></div></div>");
            client.println("</div>");
            client.println("<div class=\"row-container\"><p>Servo Position: <span id=\"servoPos\"></span></p></div>");
            client.println("<div class=\"row-container\"><input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value="+valueString+"/></div>");
            client.println("<div class=\"row-container\"><footer><p>TryMe</p></footer></div></div>");
            // Script section. Get the button and slider values and executes functions.
            client.println("<script>");
            client.println("var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000});");
            client.println("function btnVal(dir) { $.get(\"/?btnVal=\" + dir, true);}");
            client.println("function servo(pos) { $.get(\"/?value=\" + pos + \"&\");}");
            client.println("{Connection: close};</script>");
            client.println("</body></html>");
            
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              myServo.write(valueString.toInt()); // Rotate the servo
              Serial.println(valueString); 
            } else if (header.indexOf("GET /?btnVal=forward")!= -1) {
              Serial.println("FORWARD");
              forward();  // Turn forward
            } else if (header.indexOf("GET /?btnVal=left")!= -1) {
              Serial.println("LEFT");
              turnLeft();   // Turn left
            } else if (header.indexOf("GET /?btnVal=right")!= -1) {
              Serial.println("RIGHT");
              turnRight();  // Turn right
            } else if (header.indexOf("GET /?btnVal=backward")!= -1) {
              Serial.println("BACK");
              backward();   // Turn backward
            } else if (header.indexOf("GET /?btnVal=stop")!= -1) {
              Serial.println("STOP");
              stopMotors();  // Stopped
            }     
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}