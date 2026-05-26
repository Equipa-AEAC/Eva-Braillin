#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// MP3 (DFPlayer Mini)
SoftwareSerial mp3Serial(8, 9);

// Braille inputs (LASA22-3)
const int a1 = 5;
const int a2 = 6;
const int a3 = 7;

const int b1 = 2;
const int b2 = 3;
const int b3 = 4;

// estado anterior
int prevP1=0, prevP2=0, prevP3=0;
int prevP4=0, prevP5=0, prevP6=0;

// debounce
unsigned long lastPressTime = 0;
const int debounceDelay = 150;

// contador LCD
int charCount = 0;

// ---------------- BRAILLE ----------------
char decode(int p1,int p2,int p3,int p4,int p5,int p6) {

  if (p1 && !p2 && !p3 && !p4 && !p5 && !p6) return 'A';
  if (p1 && p2 && !p3 && !p4 && !p5 && !p6) return 'B';
  if (p1 && !p2 && !p3 && p4 && !p5 && !p6) return 'C';
  if (p1 && !p2 && !p3 && p4 && p5 && !p6) return 'D';
  if (p1 && !p2 && !p3 && !p4 && p5 && !p6) return 'E';
  if (p1 && p2 && !p3 && p4 && !p5 && !p6) return 'F';
  if (p1 && p2 && !p3 && p4 && p5 && !p6) return 'G';
  if (p1 && p2 && !p3 && !p4 && p5 && !p6) return 'H';
  if (!p1 && p2 && !p3 && p4 && !p5 && !p6) return 'I';
  if (!p1 && p2 && !p3 && p4 && p5 && !p6) return 'J';

  if (p1 && !p2 && p3 && !p4 && !p5 && !p6) return 'K';
  if (p1 && p2 && p3 && !p4 && !p5 && !p6) return 'L';
  if (p1 && !p2 && p3 && p4 && !p5 && !p6) return 'M';
  if (p1 && !p2 && p3 && p4 && p5 && !p6) return 'N';
  if (p1 && !p2 && p3 && !p4 && p5 && !p6) return 'O';
  if (p1 && p2 && p3 && p4 && !p5 && !p6) return 'P';
  if (p1 && p2 && p3 && p4 && p5 && !p6) return 'Q';
  if (p1 && p2 && p3 && !p4 && p5 && !p6) return 'R';
  if (!p1 && p2 && p3 && p4 && !p5 && !p6) return 'S';
  if (!p1 && p2 && p3 && p4 && p5 && !p6) return 'T';

  if (p1 && !p2 && p3 && !p4 && !p5 && p6) return 'U';
  if (p1 && p2 && p3 && !p4 && !p5 && p6) return 'V';
  if (!p1 && p2 && !p3 && p4 && p5 && p6) return 'W';
  if (p1 && !p2 && p3 && p4 && !p5 && p6) return 'X';
  if (p1 && !p2 && p3 && p4 && p5 && p6) return 'Y';
  if (p1 && !p2 && p3 && !p4 && p5 && p6) return 'Z';

  return 0;
}

// ---------------- MP3 FUNCTIONS ----------------
void selectTFCard() {
  mp3Serial.write(0x7E);
  mp3Serial.write(0x03);
  mp3Serial.write(0x35);
  mp3Serial.write(0x01);
  mp3Serial.write(0xEF);
  delay(100);
}

void setVolume(byte volume) {
  if (volume > 30) volume = 30;

  mp3Serial.write(0x7E);
  mp3Serial.write(0x03);
  mp3Serial.write(0x31);
  mp3Serial.write(volume);
  mp3Serial.write(0xEF);
  delay(100);
}

void playSound(byte trackNumber) {
  mp3Serial.write(0x7E);
  mp3Serial.write(0x04);
  mp3Serial.write(0x41);
  mp3Serial.write((byte)0x00);
  mp3Serial.write(trackNumber);
  mp3Serial.write(0xEF);
  delay(200);
}

// ---------------- SETUP ----------------
void setup() {

  pinMode(a1, INPUT_PULLUP);
  pinMode(a2, INPUT_PULLUP);
  pinMode(a3, INPUT_PULLUP);

  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  mp3Serial.begin(9600);
  delay(2000); // MUITO IMPORTANTE para o DFPlayer arrancar

  lcd.print("Braille Ready");
  delay(1500);
  lcd.clear();

  setVolume(25);
  selectTFCard();
}

// ---------------- LOOP ----------------
void loop() {

  int p1 = !digitalRead(a1);
  int p2 = !digitalRead(a2);
  int p3 = !digitalRead(a3);

  int p4 = !digitalRead(b1);
  int p5 = !digitalRead(b2);
  int p6 = !digitalRead(b3);

  bool newPress =
    (p1 && !prevP1) ||
    (p2 && !prevP2) ||
    (p3 && !prevP3) ||
    (p4 && !prevP4) ||
    (p5 && !prevP5) ||
    (p6 && !prevP6);

  if (newPress && millis() - lastPressTime > debounceDelay) {

    char c = decode(p1,p2,p3,p4,p5,p6);

    if (c != 0) {

      if (charCount >= 16) {
        lcd.clear();
        charCount = 0;
      }

      lcd.print(c);
      charCount++;

      // 🔊 SOM DA LETRA
      int track = (c - 'A') + 1;
      playSound(track);
    }

    lastPressTime = millis();
  }

  prevP1 = p1;
  prevP2 = p2;
  prevP3 = p3;
  prevP4 = p4;
  prevP5 = p5;
  prevP6 = p6;
}