
#include <SPI.h>
#include <Ethernet.h>

// VERSION NUMBER
float VersionNumber = 2.0;

// DEVICE ID - MUST BE UNIQUE
char deviceID[] = "100001";

// NETWORK SETTINGS
int port = 84;
char ipAddress[] = "192.168.0.100";
char macaddress[] = "0xDE:0xAD:0xBE:0xEF:0xFE:0xEE";
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; //physical mac address
byte ip[] = { 
  192, 168, 0, 100 }; // DEVICE IP ADDRESS
IPAddress controllerserver(192,168,0,1); // CONTROL SERVER IP ADDRESS
EthernetServer server(80); //CONTROL SERVER PORT
EthernetClient client;

// CONTROL SERVER PARAMETERS
char updateurl1[] = "/projects/arduino/master_controller.php?command=checkin&deviceid=";
char updateurl2[] = "&ip=";
char updateurl3[] = "&mac=";

// DIGITAL OUTPUTS 
String readString; 
int output1 = 2; 
int output2 = 3;
int output3 = 5;
int output4 = 6;
int output5 = 7;
int output6 = 8;
int output7 = 9;

// **************************** START CODE ****************************

void setup(){

  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output5, OUTPUT);
  pinMode(output6, OUTPUT);
  pinMode(output7, OUTPUT);

  // ETHERNET CONNECTION
  Ethernet.begin(mac, ip); // , ip, gateway, subnet
  server.begin();

  //PRINT TO SERIAL 
  Serial.begin(38400); 
  Serial.println("messer Device Controller Loading..."); // so I can keep track of what is loaded
  Serial.print("IP Address: ");
  Serial.print(Ethernet.localIP());
  Serial.print(":");
  Serial.println(port);

  // CONNECT TO CONTROL SERVER
  // Control server keeps track of all devices on the network
  if (client.connect(controllerserver, 8888)) {
    Serial.println("Connected to device server");

    Serial.println("Updating System");
    client.print("GET ");
    client.print(updateurl1); 
    client.print(deviceID);
    client.print(updateurl2);
    client.print(ipAddress);
    client.print(updateurl3);
    client.print(macaddress);
    client.println(" HTTP/1.0");
    client.println();
    client.stop();
    Serial.println("Server updated");
  } 
  else {
    Serial.println("Connection to server failed.");


    client.stop();
    delay(1000);
  }

  // EVERYTHING IS LOADED - WAIT FOR REQUESTS
  Serial.println("Ready for requests...");
  Serial.println();
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.println("String:");
          Serial.println(readString); //print to serial monitor for debuging 
          Serial.begin(38400);

            client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();

          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Garage Control</TITLE>");
          //client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<CENTER>");

          client.println("<H1>Control</H1>");


          // READ SENSOR 
	  int sensor = digitalRead(4);

         //  PRINT STATUS OF SENSOR 
         if (sensor == HIGH) {
	 client.println("CLOSED");
	 } else {
	 client.println("OPEN");
	 }

          client.println("<br><br><a href='/?flashoutput5'>Open/Close Garage</a>");

          client.println("<br><br><a href='/'>Refresh</a>");
          
          client.println("<br><br><a href='/?flashoutput4'>Start Truck</a>");
          client.println("<br><br><a href='/?flashoutput3'>Stop Truck</a>");
          
          client.println("</CENTER>");

         
          client.println("</BODY>");
          client.println("</HTML>");

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin


          //******* OUTPUT 5 GARAGE *************
          //This one is Special for Garage. 

          if(readString.indexOf("flashoutput5") >0)
          {
            digitalWrite(output5, HIGH);
            delay(1000);
            digitalWrite(output5, LOW);
            Serial.println("Output 5 Flashed");
          }    
          if(readString.indexOf("flashoutput4") >0)
          {
            digitalWrite(output1, LOW);
            delay(1000);
            digitalWrite(output1, HIGH);
            delay(1000);
            digitalWrite(output1, LOW);
            delay(1000);
            digitalWrite(output1, HIGH);
            delay(2000);
            digitalWrite(output2, LOW);
            delay(3000);
            digitalWrite(output2, HIGH);
            Serial.println("Output 4 Flashed");
          }   
     if(readString.indexOf("flashoutput3") >0)
          {
            digitalWrite(output2, LOW);
            delay(3000);
            digitalWrite(output2, HIGH);
            Serial.println("Output 3 Flashed");
          }      



          Serial.println("--------------------------------------------------");
          //clearing string for next read
          readString="";

        }
      }
    }
  }
}

