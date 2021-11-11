/*
   Code on Arduino Nano for Bachelor Thesis Force-Sensing-Book
   Reads Sensors of the sensoring Book and sends the values to the host computer.
   Based on: tams_HX711 From Norman Hendrich 
*/

#include <stdio.h>
#include <list>
//#include <EEPROM.h>
//#include <SERCOM.h>
#include "HX711.h"
#include <algorithm>
#include <vector>
#include <WiFiNINA.h>

bool debugging = false;

char c = ' ';
//long av_index = 0;
long seq_num = 0;
int value = 0;
long  rate = 80;
#define DATASIZE 40

#define BUFSIZE 80
char pbuffer[ BUFSIZE];
String command_serial = "";
int id = -1;
int test = 0;

// Accelerometer
// https://docs.arduino.cc/tutorials/nano-33-iot/imu_accelerometer
#include <Arduino_LSM6DS3.h>
#include <WiFiUdp.h>

float xGyro, yGyro, zGyro;

float x, y, z;
int degreesX = 0;
int degreesY = 0;
int degreesZ = 0;
unsigned long currentMillisInfo = 0;

//WIFI
//please enter your sensitive data in the Secret tab
char ssid[] = "XXX";                // your network SSID (name)
char pass[] = "XXX";                // your network password (use for WPA, or use as key for WEP)
int status = 0;             // the Wi-Fi radio's status
int ledState = LOW;                       //ledState used to set the LED
unsigned long previousMillisInfo = 0;     //will store last time Wi-Fi information was updated
unsigned long previousMillisLED = 0;      // will store the last time LED was updated
const int intervalInfo = 5000;            // interval at which to update the board information

//https://www.arduino.cc/en/Tutorial/LibraryExamples/WiFiNINAWiFiUdpSendReceiveString
char packetBuffer[256]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back
WiFiUDP Udp;
unsigned int localPort = 2390;      // local port to listen on
int packetSize = 0;                 // check if data available

// Klasse mit allen Infos für ein Sensor
class hx711_sensor {
  public:
    String name_id;
    HX711 scale;


    long value = 0;

    long t_last_read = 0;


    //methods
    void init( int dout, int clk) {
      scale.begin( dout, clk );
      scale.set_scale( 1.234567 );
      scale.set_offset( 604949 );
      scale.tare(); // Reset the scale to 0

      scale.wait_ready( 1000 );
    };

};


std::vector<hx711_sensor> sensor_list(5);




void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);

  //Erst starten, wenn monitor gestartet ist.
  //while (!Serial);

  //WIFI
  // set the LED as output
  pinMode(LED_BUILTIN, OUTPUT);
  // attempt to connect to Wi-Fi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }


  //Accelerometer
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  imu();





  //Serial.print( "Default calibration  offset=" ); Serial.print( scale_offset );
  //Serial.print( "  gain=" ); Serial.print( scale_gain ); Serial.println();

  memset( pbuffer, 0, BUFSIZE );

  // HX711 hinzufügen
  hx711_sensor hx1;
  hx711_sensor hx2;
  hx711_sensor hx3;
  hx711_sensor hx4;
  hx711_sensor hx10kg;

  sensor_list[0] = hx1;
  sensor_list[1] = hx2;
  sensor_list[2] = hx3 ;
  sensor_list[3] = hx4;
  sensor_list[4] = hx10kg;

  sensor_list[0].init(3, 2);
  sensor_list[1].init(5, 4);
  sensor_list[2].init(7, 6);
  sensor_list[3].init(9, 8);
  sensor_list[4].init(11, 10);
  //
  //for( int i = 0; i < sensor_list.size(); i++ ) {
  // sensor_list[id].tara_temp = 0;
  // sensor_list[id].tara_in_progress = 1;
  //  }

  Udp.begin(localPort);
  wifi();
}




void loop() {

  //WIFI
  wifi();
  //test
  //send_data("");
  //IMU
  imu();
  // hx711 sensors
  hx711();

  //
  packetSize = Udp.parsePacket();
  //Serial.println(packetSize);
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);

    Serial.println("Received: " + (String) packetBuffer);
    read_command(packetBuffer);
    if (len > 0) {
      packetBuffer[len] = 0;
      send_data("received command: " + (String) packetBuffer);
    }

    //read_command();
  }

  if ( Serial.available())
  {
    while ( Serial.available() > 0) {
      c = Serial.read();
      command_serial += c;
    }


  } // Serial.available()
  delay(100);
}

void imu() {
  // https://docs.arduino.cc/tutorials/nano-33-iot/imu_accelerometer
  // https://docs.arduino.cc/tutorials/nano-33-iot/imu_gyroscope
  //Serial.println("i1");

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
  }
  else return;
  //Serial.println("i2");
  if (IMU.gyroscopeAvailable()) {
    
    IMU.readGyroscope(xGyro, yGyro, zGyro);
  }
  else return;

  send_data("imu," + (String) millis() + "," + (String) (x) + "," + (y) + "," + (z)+ "," 
  + (String) (xGyro) + "," + (yGyro) + "," + (zGyro));

  if (debugging) {

    if (x > 0.1) {
      x = 100 * x;
      degreesX = map(x, 0, 97, 0, 90);
      Serial.print("Tilting up ");
      Serial.print(degreesX);
      Serial.println("  degrees");
      //send_data("Tilting up " + degreesX-degreesX_offset + "  degrees" );
    }
    if (x < -0.1) {
      x = 100 * x;
      degreesX = map(x, 0, -100, 0, 90);
      Serial.print("Tilting down ");
      Serial.print(degreesX);
      Serial.println("  degrees");
      //send_data("Tilting down " + degreesX-degreesX_offset + "  degrees" );

    }
    if (y > 0.1) {
      y = 100 * y;
      degreesY = map(y, 0, 97, 0, 90);
      Serial.print("Tilting left ");
      Serial.print(degreesY);
      Serial.println("  degrees");
      //send_data("Tilting left " + degreesY-degreesY_offset + "  degrees" );

    }
    if (y < -0.1) {
      y = 100 * y;
      degreesY = map(y, 0, -100, 0, 90);
      Serial.print("Tilting right ");
      Serial.print(degreesY);
      Serial.println("  degrees");
      //send_data("Tilting right " + degreesY-degreesY_offset + "  degrees" );

    }

    if (z > 0.1) {
      z = 100 * z;
      degreesZ = map(z, 0, 97, 0, 90);
      Serial.print("Tilting left ");
      Serial.print(degreesZ);
      Serial.println("  degrees");
      //send_data("Tilting left " + degreesY-degreesY_offset + "  degrees" );

    }
    if (z < -0.1) {
      z = 100 * z;
      degreesZ = map(z, 0, -100, 0, 90);
      Serial.print("Tilting right ");
      Serial.print(degreesZ);
      Serial.println("  degrees");
      //send_data("Tilting right " + degreesY-degreesY_offset + "  degrees" );

    }
  }

}

void wifi() {
  //https://docs.arduino.cc/tutorials/nano-33-iot/WiFi_connection
  currentMillisInfo = millis();
  //Serial.println("1");

  // check if the time after the last update is bigger the interval
  if (currentMillisInfo - previousMillisInfo >= intervalInfo) {
    previousMillisInfo = currentMillisInfo;
    //Serial.println("2");

    Serial.println("Board Information:");
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    //Serial.println("3");

    Serial.print("IP Address: ");
    Serial.println(ip);
    //Serial.println("4");

    // print your network's SSID:
    Serial.println();
    Serial.println("Network Information:");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    //Serial.println("5");

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
    Serial.println("---------------------------------------");
    //Serial.println("6");

  }

  unsigned long currentMillisLED = millis();

  // measure the signal strength and convert it into a time interval
  int intervalLED = WiFi.RSSI() * -10;

  // check if the time after the last blink is bigger the interval
  if (currentMillisLED - previousMillisLED >= intervalLED) {
    previousMillisLED = currentMillisLED;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void send_data(String msg) {
  int x = Udp.beginPacket("XXX.XXX.XXX.XX", 12345);
  if (msg == "") {
    Udp.write(ReplyBuffer);
  }
  else {

    Udp.write(msg.c_str());

  }

  Udp.endPacket();
}


void hx711() {
  // get sample from HX711 and store into data array
  for ( int i = 0; i < sensor_list.size(); i++ ) {
    //Serial.print( "h1 \n" );
    sensor_list[i].scale.wait_ready();
    //Serial.print( "h2 \n" );
    if (sensor_list[i].scale.is_ready()) {
      sensor_list[i].value = sensor_list[i].scale.read();
    }
    else {
      Serial.println("HX711 not found.");
    }

  }

  long t_now = millis();
  seq_num ++;
  //av_index ++;

  for ( int i = 0; i < sensor_list.size(); i++ ) {
  

    sprintf( pbuffer, "W %ld %ld %ld %ld %ld %ld  %ld\n",
             seq_num & 0xffff,
             t_now & 0xffff,
             sensor_list[i].value,
             i);
    Serial.print( pbuffer );
    send_data(pbuffer);
    Serial.print( "h5 \n" );

  }
}

void read_command(String command ) {
  char cmd = command[0];
  int n_tokens = 0;

  switch ( cmd ) {
    default:  // reset position as 0 0
      sprintf( pbuffer, "# Unknown command '%c', ignored\n", cmd );
      Serial.print( pbuffer );
      send_data(pbuffer);
      command = "";
      break;
  } // switch
}
//
