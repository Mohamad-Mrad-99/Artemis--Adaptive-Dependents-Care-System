#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "RTClib.h"
#include <Servo.h>

//Define the pins fo|r the keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Set the I2C address of the LCD module
LiquidCrystal_I2C lcd(0x27, 16, 2);
//initialize a servo object for the connected servo 
Servo servo_test;      //initialize a servo object for the connected servo 
// Initialize the RTC module
RTC_DS3231 rtc;

// Flag to indicate whether the user is setting the time
bool isSettingTime = false;

// String to store the user input for the hour
String inputHour = "";
int t1, t2, t3, t4, t5, t6;
int r[6];
boolean feed = true; // condition for alarm
char customKey;
void setup() {
  servo_test.attach(10);
  servo_test.write(65);
  Serial.begin(9600);
  Wire.begin();
  lcd.backlight();
  lcd.init();

  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC not found");
    while (1);
  }

  // Set the time if the RTC has lost power and is reset
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
   customKey = customKeypad.getKey();

  if (customKey == '*') {
    setFeedingTime();
    }

    // Get the current date and time from the RTC module
    DateTime now = rtc.now();

    // Format the date as a string
    String date = String("Date") + ":" + String(now.day(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.year(), DEC);

    // Convert the hours to 12-hour format and add an "AM" or "PM" indicator
    int hours = now.hour();
    String ampm = (hours < 12) ? "AM" : "PM";
    hours = (hours % 12) ?: 12;
    //Serial.println(hours);

    // Format the time as a string
    String time = String(hours, DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC) + " " + ampm;
    //String hr= hours;
      t1 = time.charAt(0)-48;
      t2 = time.charAt(1)-48;
      //Serial.print(t2);
      t3 = time.charAt(2)-48;
      //Serial.print(t3);
      t4 = time.charAt(3)-48;
      t5 = time.charAt(4)-48;
      t6 = time.charAt(6)-48;
      t6 = time.charAt(7)-48;
    // Print the date and time on the LCD
      lcd.setCursor(0, 0);
      lcd.print(date);
      lcd.setCursor(0, 1);
      lcd.print("Time:"+time);
  
    // Wait for one second before updating the display again
    delay(1000);
    //Serial.println(r[0]);
    //Serial.println(r[1]);
    //Serial.println(r[2]);
    //Serial.println(t1);
    //Serial.println(t3);
    //Serial.println(t4);
    
   
   
   if(hours<10 && now.minute()>9){
    
    if (t1==r[0] && t3==r[1] && t4==r[2] && feed==true){ 
    Serial.print("hello");
    servo_test.write(120);                   //command to rotate the servo to the specified angle
    delay(3000);  
    servo_test.write(65);
    feed=false;
  } 
  
 }
  else if(hours<10 && now.minute()<10){
    
    if (t1==r[0] && t3==r[2] && r[1]==0 && feed==true){ 
    Serial.print("hello");
    servo_test.write(120);                   //command to rotate the servo to the specified angle
    delay(3000);  
    servo_test.write(65);
    feed=false;
  } 
  
 }
 else if( 10<=hours<=12 && now.minute()>9){
  if (t1==r[0] && t2==r[1] && t4==r[2] && t5==r[3] && feed==true){
   Serial.print("hello");
    servo_test.write(120);                   //command to rotate the servo to the specified angle
    delay(3000);  
    servo_test.write(65);
    feed=false;
  }
 }
 else if( 10<=hours<=12 && now.minute()<10){
  if (t1==r[0] && t2==r[1] && t4==r[3] && r[2]==0 && feed==true){
   Serial.print("hello");
    servo_test.write(120
    );                   //command to rotate the servo to the specified angle
    delay(3000);  
    servo_test.write(65);
    feed=false;
  }
 }
}
  void setFeedingTime()
{
  feed = true;
   int i=0;
   int j=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set feeding Time");
  lcd.clear();
  lcd.print("HH:MM");
  lcd.setCursor(0,1);
  while(1){
    customKey = customKeypad.getKey();
    

  if(customKey!=NO_KEY){
    lcd.setCursor(j,1);
    lcd.print(customKey);
    //inputHour += customKey;
    r[i]= customKey-48;
    if(r[0]==0){
      i--;
    }
    //Serial.print(inputHour);
    //Serial.print(r[i]);
    j++;
    i++;

    if (j==2)
    {
      //r[i]=10;
      //Serial.print(r[i]);
      lcd.print(":"); j++;
    }
    delay(500);
  }

  if (customKey == '#')
  {customKey=0;
  j=0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your time is set");
  delay(1000);
  lcd.clear();
  break; }
  }
}
