/*

   Based on:
   Example using the SparkFun HX711 breakout board with a scale
   By: Nathan Seidle SparkFun Electronics
   Date: November 19th, 2014
   and Norman Hendrich XXX FOLGT
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
char ssid[] = "WG-Internet";                // your network SSID (name)
char pass[] = "Abcd1234";                // your network password (use for WPA, or use as key for WEP)
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

    //long  scale_offset = 604949;
    //float scale_gain = 1.234567;

    //long tara_temp = 0;
    //int  tara_in_progress = 0;
    //long data[ DATASIZE ];

    long value = 0;

    //long av_count = 0;
    //long seq_num = 0;
    long t_last_read = 0;
    //long av_index = 0;


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

void usage() {
  String msg = "Force Sensing Book for Bachelorthesi \ns";
  String msg2 = "commands are: \n ";
  String msg3 = "?           help: print this message \n";
  String msg4 = "A <count>   aver: average output over <count> (max 80) samples \n";
  String msg5 = "T           tara: use current readings as zero offset \n";
  String msg6 = "C <count>   cali: calibration, current readings are <count> grams \n";
  String msg7 = "R <hz>      rate: send samplerate, default is 80 Hz \n";
  String msg8 = "S           save: copy offset+gain to EERCOM \n";
  String msg9 = "ok.\n";
  msg = msg + msg2 + msg3 + msg4 + msg5 + msg6 + msg7 + msg8 + msg9;
  send_data(msg);
  Serial.println( "Digital scale for ROS (80 Hz, 10 kg max), output in grams" );
  Serial.println( "For code and documentation see github.com/TAMS-Group/tams_hx711" );
  Serial.println( "" );
  Serial.println( "Serial commands are:" );
  Serial.println( "?           help: print this message" );
  Serial.println( "A <count>   aver: average output over <count> (max 80) samples" );
  Serial.println( "T           tara: use current readings as zero offset" );
  Serial.println( "C <count>   cali: calibration, current readings are <count> grams" );
  Serial.println( "R <hz>      rate: send samplerate, default is 80 Hz" );
  Serial.println( "S           save: copy offset+gain to EERCOM" );
  Serial.println( "ok.\n" );
}


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

  usage();

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
  int x = Udp.beginPacket("192.168.178.25", 12345);
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
      //sensor_list[i].data[ av_index ] = sensor_list[i].value;
    }
    else {
      Serial.println("HX711 not found.");
    }
    //Serial.print( "h3 \n" );
    //sensor_list[i].av_index ++;
    //sensor_list[i].seq_num ++;

  }

  long t_now = millis();
  seq_num ++;
  //av_index ++;

  for ( int i = 0; i < sensor_list.size(); i++ ) {
    //Serial.print( "h4 \n" );

    //if (sensor_list[i].av_index >= sensor_list[i].av_count)
    //sensor_list[i].av_index = 0;



//    // update tara, if requested
//    if (sensor_list[i].tara_in_progress > 0) {
//
//      sensor_list[i].tara_temp += sensor_list[i].value;
//      sensor_list[i].tara_in_progress ++;
//
//      if (sensor_list[i].tara_in_progress == 33) { // hardcoded number
//        sensor_list[i].scale_offset = sensor_list[i].tara_temp / 32L;
//        sensor_list[i].tara_in_progress = 0;
//
//        sprintf( pbuffer, "New tara offset %ld\n", sensor_list[i].scale_offset );
//        Serial.print( pbuffer );
//      }
//
//    }





    // return latest data sample or average data, as requested
//    if (sensor_list[i].av_count > 0) {
//      sensor_list[i].value = sensor_list[i].data[ 0 ];
//      for ( int i = 1; i < sensor_list[i].av_count; i++ ) {
//        sensor_list[i].value += sensor_list[i].data[ i ];
//      }
//      sensor_list[i].value = sensor_list[i].value / sensor_list[i].av_count;
//    }

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
    case 'H': // help
    case 'h': // help
    case '?':
      usage();
      command = "";
      break;
//    case 'A': // average <count>
//      n_tokens = sscanf( command.c_str(), "A %ld %ld ", &id, &test);
//      if (n_tokens == 2) {
//        sensor_list[id].av_count = min( test, DATASIZE );
//        sprintf( pbuffer, "# New averaging count %ld\n", sensor_list[id].av_count );
//        Serial.print( pbuffer );
//        send_data((String) pbuffer);
//      }
//      else {
//        sprintf( pbuffer, "# Invalid averaging command '%s'\n", command.c_str() );
//        Serial.print( pbuffer );
//        send_data((String) pbuffer);
//
//      }
//      command = "";
//      break;
//    case 'C': // calibration <value>
//    case 'c': // calibration <value>
//      long calib;
//      n_tokens = sscanf( command.c_str(), "C %ld %ld ", &id, &calib );
//      if (n_tokens == 2) {
//        sensor_list[id].scale_gain = 1.0 * calib / (sensor_list[id].value - sensor_list[id].scale_offset);
//        sprintf( pbuffer, "# New calibration value %ld  divder %f\n", calib, sensor_list[id].scale_gain );
//        Serial.print( pbuffer );
//        send_data((String) pbuffer);
//      }
//      else {
//        sprintf( pbuffer, "# Invalid calibration command '%s'\n", command.c_str() );
//        Serial.print( pbuffer );
//        send_data((String) pbuffer);
//      }
//      command = "";
//
//      break;
    case 'R': // rate <hz>
    case 'r': // rate <hz>
      n_tokens = sscanf( command.c_str(), "R %ld %ld", &id,  &rate );
      if (n_tokens == 2) {
        sprintf( pbuffer, "# New sample rate %ld\n", rate );
        Serial.print( pbuffer );
        Serial.println( "# Rate: Command NOT IMPLEMENTED, ignored!" );
        send_data("Arduino: Rate: Command NOT IMPLEMENTED, ignored!");
      }
      else {
        sprintf( pbuffer, "# Invalid rate command '%s'\n", command.c_str() );
        Serial.print( pbuffer );
        send_data(pbuffer);

      }
      command = "";
      break;
//    case 'T': // tara
//    case 't': // tara
//      n_tokens = sscanf( command.c_str(), "T %ld ", &id);
//      if (n_tokens == 1) {
//        if (sensor_list[id].tara_in_progress > 0) {
//          Serial.println( "Arduino: Already sampling for new tara, request ignored..." );
//          send_data("# Already sampling for new tara, request ignored...");
//          command = ""; break;
//        }
//        else {
//          Serial.println( "# Starting tara sampling..." );
//          send_data("Arduino: Starting tara sampling...");
//          sensor_list[id].tara_temp = 0;
//          sensor_list[id].tara_in_progress = 1;
//        }
//        command = "";
//      }
//      break;
//    case 'P': // accelerator position
//    case 'p': // accelerator position
//      x_offset = x;
//      y_offset = y;
//      z_offset = z;
//      send_data("Position is set as 0");
//
//      Serial.println("Position is set as 0");
//      break;
    default:  // reset position as 0 0
      sprintf( pbuffer, "# Unknown command '%c', ignored\n", cmd );
      Serial.print( pbuffer );
      send_data(pbuffer);
      command = "";
      break;
  } // switch
}
//
