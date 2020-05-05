## Project Description
   โครงงานนี้เราทำเกี่ยวกับ Microcontroller โดยจัดทำเป็นเกมส์ที่ใช้ไมโครคอนโทรลเลอร์ (Micro-controller) ในการทำ และใช้ตัว LED Dot Matrix เป็นตัวแสดงผลออกมาพัฒนามาจากเกมส์ Cookie Run และนำมาผสมผสานความคิดสร้างสรรค์ของผู้จัดทำ โดยตัวเกมส์จะแสดงผลออกมาเป็นไฟ LED ของ LED Dot Matrix มีปุ่มคอนโทรลให้เดินหน้า ถอยหลัง กระโดด จะมีสิ่งกีดขวางที่ผู้เล่นจะต้องหลบหลีก ระหว่างทาง เพื่อให้ผู้เล่นจะได้ไม่เบื่อในการเล่นเกมส์ และเพื่อความสนุกสนานของผู้เล่นอีกด้วย

### Source Code


```markdown
//เพื่อให้สามารถเชื่อมต่อ wifi ได้
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h> 

// Replace with your firebase link and secret keys
#define FIREBASE_HOST "mail-alert-cp62.firebaseio.com" 
#define FIREBASE_AUTH "OroOJHwaRxJMdRIkcXPGilYgRQCCDQX1mSZ18Tuy" 

// Replace with your wifi-name and password
// not use 5G to connect 
const char *ssid     = "SUNYA_2.4G";
const char *password = "sunya2516";

// Connect to IFTTT
const char* host = "maker.ifttt.com";
const char *privateKey = "b_Pze4ECGcr1C80epv7WS2"; //Replace with your key 
const char *event = "warning"; // Replace with your event 

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
}

// Set variables for check Mail to break loop
// if status = 1 --> do
// if status = 0 --> break
int status=1;

void loop() {
  // for Date and Time  
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");

  delay(2000);

  // censor for distance   
  long duration, cm;
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  delayMicroseconds(2);
  digitalWrite(D4, HIGH);
  delayMicroseconds(5);
  digitalWrite(D4, LOW);
  pinMode(D0, INPUT);
  duration = pulseIn(D0, HIGH);
  cm = microsecondsToCentimeters(duration);
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();

  //Condition to Send Line Notify when the letter enter the box  
  if(cm != 3 && status == 1){
    Line_Notify(currentDate, formattedTime);
    status = 0;
    //Send Data Time, Date, and Distance to Firebase    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["centimeter"] = cm;
    root["time"] = formattedTime;
    root["Date"] = currentDate;
    Firebase.push("data", root); 
    delay(1000); 
  } 
  //Condition to reset status when the letter out
  if(cm == 3) {
    status = 1;
  }
  delay(100);
}

long microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

// Function to Send Alert to Line
void Line_Notify(String msg, String msg2) {
  WiFiClientSecure client;
  client.setInsecure();
  const int httpPort = 443;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  String message = "value1=" + String(msg)+ "\tTime: " + String(msg2);
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "Content-Type: application/x-www-form-urlencoded\r\n" +
             "Content-Length: " + message.length() + "\r\n\r\n" +
             message + "\r\n");
             
  while (client.connected()){
      if (client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
      } else {
        delay(50);
      };
  }
}




### Equipment useds

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/maurelle/project/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Member

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
