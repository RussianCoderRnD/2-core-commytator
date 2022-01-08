#define BLYNK_TEMPLATE_ID " TMPLeMQ9x2YJ "
#define BLYNK_DEVICE_NAME "Komutator"
#define BLYNK_AUTH_TOKEN "qUZbcMIOvx7OizFk8Oyj88uumNpdUy7t"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>
#include <ZMPT101B.h>  // Библиотека датчика напряжения
#include <GyverOLED.h> // Библиотека дисплея

#define ESP8266_BAUD 9600 // определение скрости обмена с ESP01

#define LineA A1              // входной PIN LineA
#define LineB A2              // входной PIN LineB
#define LineC A3              // входной PIN LineC
#define LineA_from_coreOne A7 // входной PIN состояния фазы со второго ядра
#define LineB_from_coreOne A6 // входной PIN состояния фазы со второго ядра
#define LineC_from_coreOne A0 // входной PIN состояния фазы со второго ядра
#define Blynk_Status 4        // PIN на светодиод отражающий статус состояния Blynk

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "itel A16 Plus_plus";
char pass[] = "Acer5560g!";

SoftwareSerial EspSerial(2, 3); // RX, TX, назначение пинов для передачи на ESP01
ESP8266 wifi(&EspSerial);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled; // подключение библиотеки дисплея

uint8_t valA, valB, valC;           // тип переменной для valA = analogRead(LineA), valB = analogRead(LineB), valC = analogRead(LineC)
uint8_t hiVolts = 250;              // значение верхнего уровня отключения
uint8_t lowVvolts = 170;            // значение нижнего уровня тоключения
uint8_t setX = 0;                   // переменная для корректировки по Х на дисплее
uint8_t setY = 0;                   // переменная для корректировки по Y на дисплее
ZMPT101B voltageSensorLineA(LineA); // подключение библиотеки датчика напряжения
ZMPT101B voltageSensorLineB(LineB); // подключение библиотеки датчика напряжения
ZMPT101B voltageSensorLineC(LineC); // подключение библиотеки датчика напряжения

class Display
{
public:
    Display(int x, int y, int size_font, int volts)
    {
        oled.setCursorXY(x + 12, y); // курсор в (пиксель X, строка Y)
        oled.setScale(size_font);
        oled.print("  ");
        oled.setCursorXY(x, y); // курсор в (пиксель X, строка Y)
        oled.setScale(size_font);
        oled.print(volts);
    }
};

//!======================= BlynkArguments ==================================
void BlynkArguments()
{
    Blynk.virtualWrite(V1, valA);
    Blynk.virtualWrite(V2, valB);
    Blynk.virtualWrite(V3, valC);
    Blynk.run();
}
//!======================= Display1 ========================================
void StartDisplay1()
{
    oled.setCursor(0, 3); // курсор в (пиксель X, строка Y)
    oled.setScale(1);
    oled.print("Blynk start...");
}
//!======================= Display2 ========================================
void StartDisplay2()
{
    oled.setCursor(0, 3); // курсор в (пиксель X, строка Y)
    oled.setScale(1);
    oled.print("Blynk connect...");
}
//!======================= Display =========================================
void Display(int x, int y, int size_font, int volts)
{
    oled.setCursorXY(x + 12, y); // курсор в (пиксель X, строка Y)
    oled.setScale(size_font);
    oled.print("  ");
    oled.setCursorXY(x, y); // курсор в (пиксель X, строка Y)
    oled.setScale(size_font);
    oled.print(volts);
}
//!======================= Display2 ========================================
void Display2(int x, int y, int size_font)
{
    oled.setCursorXY(x, y); // курсор в (пиксель X, строка Y)
    oled.setScale(size_font);
}
//!=======================void setup()======================================
void setup()
{
    Serial.begin(9600);            // инициялизация серийного порта с присвоением скорости 9600
    EspSerial.begin(ESP8266_BAUD); // Set ESP8266 baud rate

    oled.init();  // инициализация
    oled.clear(); // очистить дисплей

    pinMode(LineA, INPUT);               // открытие порта входа для принятия значений от датчика напряжения
    pinMode(LineB, INPUT);               // открытие порта входа для принятия значений от датчика напряжения
    pinMode(LineC, INPUT);               // открытия порта входа для принятия значений от датчика напряжения
    pinMode(LineA_from_coreOne, INPUT);  // открытие порта OUTLineA на вход
    pinMode(LineB_from_coreOne, INPUT);  // открытие порта OUTLineB на вход
    pinMode(LineC_from_coreOne, INPUT);  // открытие порта OUTLineC на вход
    pinMode(Blynk_Status, OUTPUT);       // открытие порта для статуса Blynk на выход
    StartDisplay1();                     // функция отрисовки первого экрана
    Blynk.begin(auth, wifi, ssid, pass); // подключение к Blynk
    StartDisplay2();                     // фукция отрисовки второго экрана
    delay(2000);
}
//!=========================void loop()=====================================
void loop()
{
    valA = voltageSensorLineA.getVoltageAC(50) * 5;        // считывание пораметра с LineA и сохранение его в valA
    valB = voltageSensorLineB.getVoltageAC(50) * 5;        // считывание пораметра с LineB и сохранение его в valB
    valC = voltageSensorLineC.getVoltageAC(50) * 5;        // считывание пораметра с LineC и сохранение его в valC
    boolean statusLineA = digitalRead(LineA_from_coreOne); // сичтывание значения состояния фазы с PIN-а
    boolean statusLineB = digitalRead(LineB_from_coreOne); // считывания значения состояния фазы с PIN-а
    boolean statusLineC = digitalRead(LineC_from_coreOne); // считывание значения состояния фазы с PIN-а

    //!----------- Логка выбора фазы ----------------------------------------------
    if (statusLineA == HIGH)
    {
        Blynk.virtualWrite(V4, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 1, 2);
        oled.print("Line A:");
        Display(setX + 84, setY + 1, 2, valA);
    }
    else
    {
        Blynk.virtualWrite(V4, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX + 1, setY + 1, 2);
        oled.print("Line A:");
        Display(setX + 84, setY + 1, 2, valA);
    }

    if (statusLineB == 10)
    {
        Blynk.virtualWrite(V5, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 24, 2);
        oled.print("Line B:");
        Display(setX + 84, setY + 24, 2, valB);
    }
    else
    {
        Blynk.virtualWrite(V5, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX, setY + 24, 2);
        oled.print("Line B:");
        Display(setX + 84, setY + 24, 2, valB);
    }

    if (statusLineC == HIGH)
    {
        Blynk.virtualWrite(V6, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 48, 2);
        oled.print("Line C:");
        Display(setX + 84, setY + 48, 2, valC);
    }
    else
    {
        Blynk.virtualWrite(V6, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX, setY + 48, 2);
        oled.print("Line C:");
        Display(setX + 84, setY + 48, 2, valC);
    }
    BlynkArguments();
}

/*
Display(87, 1, 2, valA);
Display(87, 24, 2, valB);
Display(87, 48, 2, valC);
    if (statusLineA == HIGH)
    {
        Blynk.virtualWrite(V4, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 1, 2);
        oled.print("Line A:");
        Display(setX + 87, setY + 1, 2, valA);
    }
    else
    {
        Blynk.virtualWrite(V4, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX + 1, setY + 1, 2);
        oled.print("Line A:");
        Display(setX + 87, setY + 1, 2, valA);
    }

    if (statusLineB == 10)
    {
        Blynk.virtualWrite(V5, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 24, 2);
        oled.print("Line B:");
        Display(setX + 87, setY + 24, 2, valB);
    }
    else
    {
        Blynk.virtualWrite(V5, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX, setY + 24, 2);
        oled.print("Line B:");
        Display(setX + 87, setY + 24, 2, valB);
    }

    if (statusLineC == HIGH)
    {
        Blynk.virtualWrite(V6, 1);
        oled.invertText(true); // инвертируй текст!
        Display2(setX, setY + 48, 2);
        oled.print("Line C:");
        Display(setX + 87, setY + 48, 2, valC);
    }
    else
    {
        Blynk.virtualWrite(V6, 0);
        oled.invertText(false); // инвертируй текст!
        Display2(setX, setY + 48, 2);
        oled.print("Line C:");
        Display(setX + 87, setY + 48, 2, valC);
    }

    // Display printDisplayvalA(1, 1, 2, valA, 255);
    //  Display printDisplayvalB(1, 3, 2, valB, 255);
    //   Display printDisplayvalC(1, 5, 2, valC, 255);

    BlynkArguments();
}
/*
class Display
{
public:
    Display(int x, int y, int size_font, int volts, int bright = 255)
    {
        _x = x;
        _y = y;
        _size_font = size_font;
        _volts = volts;
        _bright = bright;
    }
    void Display(x, y, size_font, volts)
{
    oled.setCursorXY(x + 12, y); // курсор в (пиксель X, строка Y)
    oled.setScale(size_font);
    oled.print("  ");
    oled.setCursorXY(x, y); // курсор в (пиксель X, строка Y)
    oled.setScale(size_font);
    oled.print(volts);
}

private:
    int _x;
    int _y;
    int _size_font;
    int _volts;
    int _bright;
};
*/

/*
class Display
{
public:
    Display(int x, int y, int size_font, int volts, int bright = 255);
    void setX(int x);
    void setY(int y);
    void setSize_font(int size_font);
    void setVolts(int volts);
    void setBright(int bright);
    int getX();
    int getY();
    int getSize_font();
    int getVolts();
    int getBright();

private:
    int _x;
    int _y;
    int _size_font;
    int _volts;
    int _bright;
};
// реализация методов
Display::Display(int x, int y, int size_font, int volts, int bright)
{ // конструктор
    _x = x;
    _y = y;
    _size_font = size_font;
    _volts = volts;
    _bright = bright;
}

void Display::setX(int x) { _x = x; }
void Display::setY(int y) { _y = y; }
void Display::setSize_font(int size_font) { _size_font = size_font; }
void Display::setVolts(int volts) { _volts = volts; }
void Display::setBright(int bright) { _bright = bright; }

int Display::getX() { return _x; }
int Display::getY() { return _y; }
int Display::getSize_font() { return _size_font; }
int Display::getVolts() { return _volts; }
int Display::getBright() { return _bright; }

*/

/*



    Display printDisplay(1, 1, 2, valA, "Line A: ", 255);
    Display printDisplay(1, 3, 2, valB, "Line B: ", 255);
    Display printDisplay(1, 4, 2, valC, "Line C: ", 255);

    if (digitalRead(LineA_from_coreOne == HIGH))
    {
        Display printDisplay(1, 1, 3, valA, "Line A: ", 255);
    }
    if (digitalRead(LineB_from_coreOne == HIGH))
    {
        Display printDisplay(1, 1, 3, valA, "Line B: ", 255);
    }
    if (digitalRead(LineC_from_coreOne == HIGH))
    {
        Display printDisplay(1, 1, 3, valA, "Line C: ", 255);
    }
*/