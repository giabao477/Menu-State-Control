#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Chân I/O
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define POT_PIN 34
#define BUTTON_PIN 13
#define RELAY_PIN 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool selected = false;
static int pointer = 0;

// Biến phục vụ lọc mượt
float smoothVal = 0;
float alpha = 0.5; 

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0X3C)) for(;;);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  display.clearDisplay();
  display.setTextColor(WHITE);
  digitalWrite(RELAY_PIN, HIGH);
  
  // Khởi tạo giá trị lọc ban đầu
  smoothVal = analogRead(POT_PIN);
}

void loop() {
  // BƯỚC 1: LỌC GIÁ TRỊ POT (Laplace Transform rút gọn)
  int raw = analogRead(POT_PIN);
  smoothVal = (alpha * raw) + ((1 - alpha) * smoothVal);
  
  handleInput();
  
  if(!selected) {
    showMenu();
  } else {
    showDetails();
  } 
  display.display();
}

void handleInput() {
  int currentbuttonState = digitalRead(BUTTON_PIN);
  static bool lastbuttonState = HIGH;
  
  // SỬ DỤNG GIÁ TRỊ ĐÃ LỌC Ở ĐÂY
  int realTimePot = map((int)smoothVal, 0, 4095, 0, 3);
  
  if(selected == true && realTimePot != pointer) {
    selected = false;
  }
  
  if(selected == false){
    pointer = realTimePot;
  }
  
  if(currentbuttonState == LOW && lastbuttonState == HIGH) {
    if(selected && pointer == 3){
      digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
    } else {
      selected = !selected;
      delay(200); 
    } 
  } 
  lastbuttonState = currentbuttonState;
}

void showMenu() {
  // SỬ DỤNG GIÁ TRỊ ĐÃ LỌC ĐỂ ĐỒNG BỘ HIỂN THỊ
  pointer = map((int)smoothVal, 0, 4095, 0, 3);
  
  display.clearDisplay(); // Xóa màn hình một lần ở đầu hàm vẽ
  display.setTextSize(1);
  display.setCursor(35,0); display.println("MENU CHINH");
  display.drawLine(0,10, 128, 10, WHITE);

  if(pointer <= 2) {
    display.setCursor(15,20); display.println("1. THONG TIN");
    display.setCursor(15,35); display.println("2. NGANH-TRUONG");
    display.setCursor(15,50); display.println("3. KET QUA GPA");
    
    // Khung chọn và Scrollbar (Trang 1)
    int y_pos = 17 + (pointer * 15);
    display.drawLine(125, 15, 125, 38, WHITE); // Thanh scroll dọc
    display.drawRect(5, y_pos, 118, 14, WHITE);
    display.setCursor(120, y_pos+3); display.print("<");
  } else {
    display.setCursor(15,20); display.println("4. RELAY CONTROL");
    
    // Khung chọn và Scrollbar (Trang 2)
    int y_pos = 17 + ((pointer - 3) * 15);
    display.drawLine(125, 38, 125, 61, WHITE); // Thanh scroll dọc xuống dưới
    display.drawRect(5, y_pos, 118, 14, WHITE);
    display.setCursor(120, y_pos+3); display.print("<");
  }
}

void showDetails() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
// Detail 
  switch (pointer) {
    case 0:
      display.println(">> THONG TIN");
      display.println("\nTen: MAI GIA BAO");
      display.setCursor(0,30);
      display.println("Nam sinh: 2007");
      display.setCursor(0,40);
      display.println("SDT: 0123456789");
      break;
    case 1:
      display.println(">> NGANH - TRUONG");
      display.println("\nNganh: Automation");
      display.println("\nDH Giao Thong VT");
      display.println("TP. Ho Chi Minh(UTH)");
      break;
    case 2:
      display.println(">> KET QUA GPA");
      display.setTextSize(2); display.println("\nGPA: 4.0");
      display.setTextSize(1); display.println("(perfect score!!!)");
      break;
    case 3:
      display.println(">> RELAY CONTROL");
      display.print("\nRelay is: ");
      if(digitalRead(RELAY_PIN) == LOW) {
        display.println("ON");
      } else {
        display.println("OFF");
      }
      display.println("\nBam nut de dao trang thai.");
      break;
  }
  display.setCursor(0,56); 
  display.print("Xoay pot de tro lai.");
}
