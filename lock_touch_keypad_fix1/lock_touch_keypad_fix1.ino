#include <TTP229.h>
#include<LiquidCrystal.h>
#include <EEPROM.h>
#include <Servo.h>

LiquidCrystal lcd(12,11,10,9,8,7);

Servo myservo;
int pos = 0;
int check_open = 0;
int set_pwd = 0;
//Connect to TTP229 with I2C of Adruino Nano
const int SCL_PIN = A5;  // The pin number of the clock pin.SCL
const int SDO_PIN = A4;  // The pin number of the data pin.SDA
// connect to Motor Servo
int servo_pin = A2;
// LEDS STATUS
int LED_OPEN = 5;
int LED_CLOSE = 6;

TTP229 ttp229(SCL_PIN, SDO_PIN); // TTP229(sclPin, sdoPin)

int i=0;
int error = 0;
uint8_t initial_pwd[4] = {0 , 0, 0, 0};
uint8_t new_pwd[4] = {0 , 0, 0, 0};
uint8_t cur_pwd[4] = {0 , 0, 0, 0};
uint8_t pwd[4] = {0 , 0, 0, 0};
uint8_t key_pressed = 0;

void setup(){
    // khoi tao 
    pinMode(LED_OPEN,OUTPUT);
    pinMode(LED_CLOSE,OUTPUT);
    digitalWrite(LED_OPEN,HIGH);
    digitalWrite(LED_CLOSE,LOW);
    Serial.begin(9600);
    myservo.attach(servo_pin);
    myservo.write(180);
    Serial.println("Start Touching One Key At a Time!");
    lcd.begin(16,2);
    lcd.print("LOADING");
    // in ra cac dau . lan luot tu trai sang phai
    for(int i = 7;i<16;i++){
        lcd.setCursor(i,0);
        lcd.print(".");
        delay(200);
    }
    //load mat khau tu eeprom
    init_pwd();
    delay(2000);
    lcd.clear();
    lcd.print(" ENTER PASSWORD");
}
void loop(){
     // doc phim duoc nhan va luu vao bien key_press
     key_pressed  = ttp229.ReadKey16();
     // phim duoc nhan la 13 thi no goi ham thay doi mat khau
     if(key_pressed == 13){
        set_pwd = 1;
        while(set_pwd==1){
          change_pwd();
        }
      }
     // neu phim duoc nhan va cua dang mo thi no se dong cua
     if(key_pressed == 16 and check_open == 1){
        close_door();
      }
     // neu nhan  phim khac phim 13 va phim 16 thi no in ra tung phim cho đen khi du 4 ky tu mat khau
     if(key_pressed){
        pwd[i++] = key_pressed;
        lcd.setCursor(5+i,1);
        lcd.print(key_pressed);
        delay(500);
        lcd.setCursor(5+i,1);
        lcd.print("*");
      }
     /*khi da du 4 ky tu kiem tra xem co dung mat khau khong
      dung: mở cửa
      sai <= 3 lần: báo sai mật khẩu yêu cầu nhập lại
      sai > 3 lần: cấm nhập 3 phút và nháy đèn cảnh báo*/
     if(i==4){
        delay(200);
        init_pwd();
        if(cmp_arr(pwd,initial_pwd)==1){
          error = 0;
          check_open = 1;
          lcd.clear();
          digitalWrite(LED_OPEN,LOW);
          digitalWrite(LED_CLOSE,HIGH);
          lcd.print("    Correct!");
          lcd.setCursor(0,1);
          lcd.print("   Openning");
          for(pos = 180; pos >= 90; pos -= 1) { // goes from 0 degrees to 90 degrees // in steps of 1 degree
              myservo.write(pos);              // tell servo to go to position in variable 'pos'
              delay(20);                       // waits 15ms for the servo to reach the position
          }
          delay(3000);
          lcd.clear();
          lcd.print("16: Close door");
          lcd.setCursor(0,1);
          lcd.print("13: Change pass");
          i = 0;
        }
        else{
          error++;
          if(error<=3){
              lcd.clear();
              lcd.print(" Wrong Password");
              lcd.setCursor(0,1);
              lcd.print("  Try again!");
              delay(2000);
              lcd.clear();
              lcd.print(" ENTER PASSWORD");
              i = 0;
          }
          else{
              lcd.clear();
              lcd.print("Wrong more than 3 times!");
              lcd.setCursor(0,1);
              lcd.print("Please wait after 3 minutes or reset!");
              for(int k = 0;k<180000;k++){
                 lcd.scrollDisplayLeft();
                 digitalWrite(LED_CLOSE,HIGH);
                 delay(500);
                 digitalWrite(LED_CLOSE,LOW);
                 delay(500);  
              }
              lcd.clear();
              lcd.print(" ENTER PASSWORD");
              i = 0;
              error = 0;
          }
       }
   }
}
/* hàm được gọi khi đổi mật khẩu*/
void change_pwd(){
  int j=0;
  lcd.clear();
  lcd.print("Current Password");
  lcd.setCursor(0,1);
  while(j<4){
    uint8_t key = ttp229.ReadKey16();
    if(key){
      cur_pwd[j++]=key;
      lcd.setCursor(5+j,1);
      lcd.print(key);
      delay(500);
      lcd.setCursor(5+j,1);
      lcd.print("*");
    }
    key=0;
  }
  delay(500);
  if(cmp_arr(cur_pwd,initial_pwd)==0){
      lcd.clear();
      lcd.print(" Wrong Password");
      lcd.setCursor(0,1);
      lcd.print("  Try again!");
      delay(2000);
    }
  else{
      j=0;
      lcd.clear();
      lcd.print("New Password:");
      lcd.setCursor(0,1);
      while(j<4){
        uint8_t key=ttp229.ReadKey16();
        if(key){
          initial_pwd[j]=key;
          lcd.setCursor(5+j,1);
          lcd.print(key);
          delay(500);
          lcd.setCursor(5+j,1);
          lcd.print("*");
          EEPROM.write(j,key);
          delay(5);
          j++;
        }
      }
      lcd.clear();
      lcd.print("Creating");
      for(int i = 8;i<16;i++){
        lcd.setCursor(i,0);
        lcd.print(".");
        delay(200);
      }
      delay(2000);
      lcd.setCursor(0,1);
      lcd.print("OK!Created");
      delay(2000);
      set_pwd = 0;   
  }
  lcd.clear();
  lcd.print(" Enter Password");
  lcd.setCursor(0,1);
  key_pressed=0;
}
/*hàm được gọi khi nhấn phím đóng cửa*/
void close_door(){
  check_open = 0;
  lcd.clear();
  lcd.print("  Closed door");
  digitalWrite(LED_CLOSE,LOW);
  digitalWrite(LED_OPEN,HIGH);
  for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 90 degrees // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(5000);
  lcd.clear();
  lcd.print(" ENTER PASSWORD");
  i = 0;
  key_pressed=0;
}
/*hàm so sánh 2 mảng ký tự*/
int cmp_arr(uint8_t a[],uint8_t b[]){
  for(int i = 0;i < 4;i++){
      if(a[i] != b[i]){
          return 0;
        }  
  }
  return 1;
}
/*hàm khởi tạo mật khẩu ban đầu*/
void init_pwd(){
  for(int i = 0;i<4;i++){
    initial_pwd[i] = EEPROM.read(i);
    delay(10);
  }  
}
