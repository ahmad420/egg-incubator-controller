#include <DHT.h>
#include <WiFi.h>
#include "time.h"
DHT dht(26,DHT12);

int heatBulbBin = 17;
int humidityFanBin = 16;

#include <LiquidCrystal_I2C.h>
// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

const char* ssid     = "";
const char* password = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;

int dayNum = 0;
int hatchingPeriodDay = 19;
float minTemp = 37.5;
float maxTemp = 38.5;
int minHumidity = 50;
int maxHumidity = 55;
int minHumidityHatching = 70;
int maxHumidityHatching = 75;

typedef enum {
    my_WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    my_WL_IDLE_STATUS      = 0,
    my_WL_NO_SSID_AVAIL    = 1,
    my_WL_SCAN_COMPLETED   = 2,
    my_WL_CONNECTED        = 3,
    my_WL_CONNECT_FAILED   = 4,
    my_WL_CONNECTION_LOST  = 5,
    my_WL_DISCONNECTED     = 6
} my_internet_status;


void setup() {
  pinMode(heatBulbBin, OUTPUT);
  pinMode(humidityFanBin, OUTPUT);
  
  dht.begin();
  delay(2000);
  Serial.begin(115200);
  
  lcd.init();                   
  lcd.backlight();

    // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println(WiFi.status());
  }
  Serial.println("");
  Serial.println("WiFi connected.");

   // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

String getDate(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Failed to obtain time";
  }
//  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");


  uint8_t day = timeinfo.tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);
   
  uint8_t month = timeinfo.tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);
  
  uint16_t year = timeinfo.tm_year + 1900;
  String yearStr = String(year);

  return dayStr + "." + monthStr + "." + yearStr;
}

String getTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Failed to obtain time";
  }
  String hour_str = String(timeinfo.tm_hour);
  String minute_str = String(timeinfo.tm_min);
  String sec_str = String(timeinfo.tm_sec);
  return hour_str + ":" + minute_str + ":" + sec_str;
}


void loop() {
  String date_str = getDate();
  String time_str = getTime();
  Serial.println(date_str);
  Serial.println(time_str);
  Serial.println(WiFi.status());
  float temp = dht.readTemperature();
  int humidity = dht.readHumidity();
  delay(2000);
  lcd.clear();
  Serial.print("Temperature : ");
  Serial.print(temp);
  Serial.println(" C");
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(temp,1);
  lcd.print(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  lcd.setCursor(0, 2);
  lcd.print("Humidity : ");
  lcd.print(humidity);
  lcd.print("%");

  if(dayNum < hatchingPeriodDay){
    if(humidity < minHumidity){
      digitalWrite(humidityFanBin, HIGH);
    }else if(humidity > maxHumidity){
      digitalWrite(humidityFanBin, LOW);
    }
  }else{
    if(humidity < minHumidityHatching){
      digitalWrite(humidityFanBin, HIGH);
    }else if(humidity > maxHumidityHatching){
      digitalWrite(humidityFanBin, LOW);
    }
  }
  

  if(temp < minTemp){
    digitalWrite(heatBulbBin, HIGH);
  }else if(temp > maxTemp){
    digitalWrite(heatBulbBin, LOW);
  }
  delay(50);

}