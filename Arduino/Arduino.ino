// Load libraries
#include <Nextion.h>
#include <WiFi.h>

#define INTEGER 0
#define STRING 1


// Network credentials
const char *ssid = ""; //Put here your SSID
const char *password = ""; //Put here your password
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Wifi configuration
WiFiClient client;

//Server information
byte server_ip[] = {192, 168, 1, 30};

void setup() {
  //Serial.begin(115200);
  //Wifi connection
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin((char*)ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi Connected");
  //IPAddress ip = WiFi.localIP();
  //Serial.println(ip);


  //Serial.println("Connection to server...");
  while (!client.connect(server_ip, 22005)) {
    //Serial.println("Connection failed");
    delay(1000);
  }
  //Serial.println("Connected");
  nexInit();
}

char c;
char cmd[64] = "";

void loop()
{
  if (client.available())
  {
    while((c= client.read())!=10 && client.connected())
    {
      switch(c)
      {
        case '1': //RPM
          sprintf(cmd,"v.rpm");
          read_socket_data(INTEGER);
          break;
        case '2': //SPEED
          sprintf(cmd,"v.speed");
          read_socket_data(STRING);
          break;
        case '3': //GEAR
          sprintf(cmd,"v.gear");
          read_socket_data(STRING);
          break;
        case '4': //TOTAL_FUEL
          sprintf(cmd,"v.fuel");
          read_socket_data(STRING);
          break;
        case '5': //FUEL_LAP
          sprintf(cmd,"v.fuel_lap");
          read_socket_data(STRING);
          break;
          //A MODIFIER
        case '6': //LAP
          sprintf(cmd,"v.lap");
          read_socket_data(STRING);
          break;
        case '7': //T_OIL
          sprintf(cmd,"v.toil");
          read_socket_data(INTEGER);
          break;
        case '8': //T_MOT
          sprintf(cmd,"v.tmot");
          read_socket_data(INTEGER);
          break;
        case '9': //PIT_LIMITER
          sprintf(cmd,"v.pit_limit");
          read_socket_data(INTEGER);
          break;
        case 'a': //BALANCE
          sprintf(cmd,"v.balance_tmp");
          read_socket_data(STRING);
          break;
        case 'b': //LOW FUEL
          sprintf(cmd,"v.fuel_low");
          read_socket_data(INTEGER);
          break;
          //A MODIFIER
        case 'c': //DELTA TIME
          sprintf(cmd,"v.delta");
          read_socket_data(STRING);
          break;
        case 'd': //TC
          sprintf(cmd,"v.tc");
          read_socket_data(INTEGER);
          break;
        case 'e': //MAP
          sprintf(cmd,"v.map");
          read_socket_data(INTEGER);
          break;
        case 'f': //ABS
          sprintf(cmd,"v.abs");
          read_socket_data(INTEGER);
          break;
      }
    }
    //Serial.println(cmd);
  }
  Serial.println(client.connected());
  if (!client.connected())
  {
    Serial.println("Disconnecting");
    client.stop();
    while (!client.connect(server_ip, 22005)) {
      delay(1000);
    //Serial.println("Connection failed");
    }
  }
}

/*
 * Param 'int type' -> tell if the variable is an integer (type==0) or a string (type==1)
 *
 *
 */
void read_socket_data(int type)
{
  if(type==STRING)
    sprintf(cmd,"%s.txt=\"",cmd);
  else
    sprintf(cmd,"%s.val=",cmd);
  c = client.read();
  while(c!=10)
    {
      //Serial.print(c);
      if(c!=13)
      {
        sprintf(cmd,"%s%c",cmd,c);
        //cmd = cmd + c;
        //Serial.println((int)c);
      }
      c = client.read();
    }
    if(type==STRING)
      sprintf(cmd,"%s\"",cmd);
    sendCommand(cmd);
    //Serial.println(cmd);
}
