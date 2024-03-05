#include<WiFi.h>
#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include<DHT.h>

//  rgb led details
byte rpin = 25;
byte gpin = 26;
byte bpin = 27;
byte rchannel = 0;
byte gchannel = 1;
byte bchannel = 2;
byte resolution = 8;
int frequency = 5000;

//  dht details
byte dht_pin = 4;
#define dht_type DHT11
DHT dht(dht_pin , dht_type);

//  wifi credentials
const char ssid[] = "the Force";
const char password[] = "rohanog7";

//  io details
#define IO_USERNAME  "RohanOg"
#define IO_KEY       "aio_DuWU79T7gyreyHo16OEY42asrXSf"
#define IO_BROKER    "io.adafruit.com"
#define IO_PORT       1883

//  client details
WiFiClient wificlient;
Adafruit_MQTT_Client mqtt(&wificlient , IO_BROKER , IO_PORT , IO_USERNAME , IO_KEY);

/*
  SYNTAX FOR REFERENCE : 
  Adafruit_MQTT_Subscribe FEED OBJECT = Adafruit_MQTT_Subscribe(&mqtt , IO_USERNAME"/feeds/FEED NAME");
*/
Adafruit_MQTT_Subscribe red_color = Adafruit_MQTT_Subscribe(&mqtt , IO_USERNAME"/feeds/red");
Adafruit_MQTT_Subscribe green_color = Adafruit_MQTT_Subscribe(&mqtt , IO_USERNAME"/feeds/green");
Adafruit_MQTT_Subscribe blue_color = Adafruit_MQTT_Subscribe(&mqtt , IO_USERNAME"/feeds/blue");


/*  
  SYNTAX FOR REFERENCE : 
  Adafruit_MQTT_Publish feed object = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/FEED NAME");  
 */
Adafruit_MQTT_Publish Tempc = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/temperature(C*)");
Adafruit_MQTT_Publish Tempf = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/temperature(F)");
Adafruit_MQTT_Publish Tempk = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/temperature(K)");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/humidity(%RH)");


void setup()
{
  Serial.begin(115200);

  //  connecting with wifi
  Serial.print("Connecting with : ");
  Serial.println(ssid);
  WiFi.begin(ssid , password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected !");
  Serial.print("IP assigned by AP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //  RGB led setup
  ledcSetup(rchannel , frequency , resolution);
  ledcSetup(gchannel , frequency , resolution);
  ledcSetup(bchannel , frequency , resolution);

  //  attaching pins with channel
  ledcAttachPin(rpin , rchannel);
  ledcAttachPin(gpin , gchannel);
  ledcAttachPin(bpin , bchannel);

  //  dht setup
  dht.begin();

  //  feeds to be subscribed
mqtt.subscribe(&red_color);
mqtt.subscribe(&green_color);
mqtt.subscribe(%blue_color);

  
}

void loop()
{
  //  connecting with server
  mqttconnect();

  //  reading values from dht sensor
  float Tempc = dht.readTemperature();
  float Tempf = dht.readTemperature(true);
  float Tempk = tempc + 273.15;
  float Humidity = dht.readHumidity();

  if (isnan(Tempc)  ||  isnan(Tempf)  ||  isnan(Humidity))
  {
    Serial.println("Sensor not working!");
    delay(1000);
    return;
  }

  //  printing these values on serial monitor
  String val = String(Tempc) + " *C" + "\t" + String(Tempf) + " *F" + "\t" + String(Tempk) + " *K" + "\t" + 
               String(humidity) + " %RH"
  Serial.println(val);
  

Adafruit_MQTT_Subscribe *subscription;
while(true)
{
   subscription = mqtt.readSubscription(5000);  //  wait for 5000ms to read subscription
    if (subscription  ==  0)  //  after timeout or 5sec , either feed is returned or 0
    {
      Serial.println("Can't catch feed");
      break;
    }
    else  //  got something
    {
      if (subscription  ==  &red)
      {
        String temp = (char *)red.lastread;
        r = temp.toInt();
        makecolor(r , g , b);
      }
      else if (subscription  ==  &green)
      {
        String temp = (char *)green.lastread;
        g = temp.toInt();
        makecolor(r , g , b);
      }
      else if (subscription  ==  &blue)
      {
        String temp = (char *)blue.lastread;
        b = temp.toInt();
        makecolor(r , g , b);
      }
    }
  }

  delay(7000);
}
  
  

void mqttconnect()
{
  //  if already connected, return
  if (mqtt.connected())return;

  //  if not, connect
  else
  {
    while (true)
    {
      int connection = mqtt.connect();  //  mqq client has all details of client, port , username, key
      if (connection  ==  0)
      {
        Serial.println("Connected to IO");
        break;  //  connected
      }
      else
      {
        Serial.println("Can't Connect");
        mqtt.disconnect();
        Serial.println(mqtt.connectErrorString(connection));  //  printing error message
        delay(5000);  //  wait for 5 seconds
      }
    }
  }

  //  wait for some time
  delay(5000);
}

void makecolor(byte r , byte g , byte b)
{
  //  printing values
  Serial.print("RED : ");
  Serial.print(r);
  Serial.print('\t');
  Serial.print("GREEN : ");
  Serial.print(g);
  Serial.print('\t');
  Serial.print("BLUE : ");
  Serial.println(b);

  //  writing values
  ledcWrite(rchannel , r);
  ledcWrite(gchannel , g);
  ledcWrite(bchannel , b);
}
