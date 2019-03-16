#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OLED_ADDR   0x3C
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
Adafruit_SSD1306 display(-1);
#define pinChoose 2
#define pinEnter 3

int a, b;
int time;
byte menuCount = 1;

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };


void setup()
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(100);
  display.clearDisplay();
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void loop()
{
  a = digitalRead(2);
  b = digitalRead(3);
  menuSelect();
  menuScreen();
  display.clearDisplay();
  delay(100);
}

void menuScreen()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println("Main Menu");
  //------------------------------
  display.setCursor(10, 20);
  display.println("Time: ");
  display.setCursor(50, 20);
  display.println(time);

  display.setCursor(10, 30);
  display.println("Temperature: ");
  display.setCursor(85, 30);
  display.println("25");

  display.setCursor(10, 40);
  display.println("Meal time: ");
  display.setCursor(80, 40);
  if (b == 1)
  {
    display.println("ON");
  }
  else
    display.println("OFF");
  display.setCursor(2, (menuCount * 10) + 10);
  display.println(">");
  display.display();
}

void menuSelect()
{
  if (a == 1 && menuCount < 4)
    menuCount++;
  if (a == 1 && menuCount >= 4)
    menuCount = 1;
}
