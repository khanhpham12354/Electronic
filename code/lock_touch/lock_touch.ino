#include <TTP229.h>
#include<LiquidCrystal.h>
#include <EEPROM.h>
#include <Servo.h>

LiquidCrystal lcd(12,11,10,9,8,7);

Servo myservo;
//Connect to TTP229 with I2C of Adruino Nano
const int SCL_PIN = A5;  // The pin number of the clock pin.SCL
const int SDO_PIN = A4;  // The pin number of the data pin.SDA
// connect to Motor Servo
int servo_pin = A2;
// LEDS STATUS
int LED_OPEN = 5;
int LED_CLOSE = 6;
TTP229 ttp229(SCL_PIN, SDO_PIN); // TTP229(sclPin, sdoPin)

int i, j, count = 0,mode = 0,check_open = 0;

uint8_t PASSWORD[4] = {0 , 0, 0, 0};
uint8_t str[4] = {0 , 0, 0, 0};

void setup(){
  pinMode(LED_OPEN,OUTPUT);
  pinMode(LED_CLOSE,OUTPUT);
  digitalWrite(LED_OPEN,HIGH);
  digitalWrite(LED_CLOSE,LOW);
  Serial.begin(9600);
  myservo.attach(servo_pin);
  myservo.write(180);
  Serial.println("Start Touching One Key At a Time!");
  lcd.begin(16,2);
  lcd.print("LOADING.......");
  for(int i = 0;i<4;i++){
    PASSWORD[i] = EEPROM.read(i);
    delay(10);
    }
  delay(2000);
  lcd.clear();
  lcd.print("ENTER PASSWORD");
}
void loop(){
  uint8_t key = ttp229.ReadKey16(); // Blocking
  if(key){
    Serial.println(key);
    if(i == 0){
      str[0] = key;
      lcd.setCursor(6,1);
      lcd.print(str[0]);
      delay(500); // Ký tự hiển thị trên màn hình LCD trong 1s
      lcd.setCursor(6,1);
      lcd.print("*"); // Ký tự được che bởi dấu *
    }
    if(i == 1){
      str[1] = key;
      lcd.setCursor(7,1);
      lcd.print(str[1]);
      delay(500);
      lcd.setCursor(7, 1);
      lcd.print("*");
    }
    if(i == 2){
      str[2] = key;
      lcd.setCursor(8,1);
      lcd.print(str[2]);
      delay(500);
      lcd.setCursor(8,1);
      lcd.print("*");
    }
    if(i == 3){
      str[3] = key;
      lcd.setCursor(9,1);
      lcd.print(str[3]);
      delay(500);
      lcd.setCursor(9,1);
      lcd.print("*");
      count = 1;
    }
    i = i + 1; 
  }
  /*Check Password*/
  if(count == 1){
    if(str[0] == PASSWORD[0] && str[1] == PASSWORD[1] && str[2] == PASSWORD[2] && str[3] == PASSWORD[3] && mode!=2){
      check_open = 1;
      lcd.clear();
      digitalWrite(LED_OPEN,LOW);
      digitalWrite(LED_CLOSE,HIGH);
      lcd.print("    Correct!");
      myservo.write(90);
      delay(3000);
      lcd.clear();
      lcd.print("      Opened!");
      i = 0;
      count = 0;
    }else if(mode == 2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Create Pass");
      lcd.setCursor(0,1);
      lcd.print("Creating....");
      for(int i = 0; i< 4;i++){
        EEPROM.write(i,str[i]);
        delay(5);
        }
      for(int i = 0;i<4;i++){
        PASSWORD[i] = EEPROM.read(i);
        delay(5);
        }
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Create Pass");
      lcd.setCursor(0,1);
      lcd.print("OK! Created");
      delay(3000);
      lcd.clear();
      lcd.print(" Enter Password");
      i = 0;
      count = 0;
      mode = 0;
      }
    else{
      lcd.clear();
      lcd.print("   Incorrect!");
      delay(3000);
      lcd.clear();
      lcd.print("   Try Again!");
      delay(3000);
      lcd.clear();
      lcd.print(" Enter Password");
      i = 0;
      count = 0;
    }
  }
  switch(key){
    case 16:
      check_open = 0;
      lcd.clear();
      lcd.print("     Closed!");
      digitalWrite(LED_CLOSE,LOW);
      digitalWrite(LED_OPEN,HIGH);
      myservo.write(180);
      delay(10000);
      lcd.clear();
      lcd.print(" Enter Password");
      i = 0;
      break;
    case 13:
      if(check_open == 1){
        lcd.clear();
        lcd.print("Enter New Pass");
        i = 0;
        count = 0;
        mode = 2;
        break;
    }
  }
}
