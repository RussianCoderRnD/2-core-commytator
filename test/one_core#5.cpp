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

#define LineA A1             // входной PIN LineA
#define LineB A2             // входной PIN LineB
#define LineC A3             // входной PIN LineC
#define LineA_from_coreOne 9 // входной PIN состояния фазы со второго ядра
#define LineB_from_coreOne 6 // входной PIN состояния фазы со второго ядра
#define LineC_from_coreOne 4 // входной PIN состояния фазы со второго ядра
#define LiA_in_coreTwo 12    // входной PIN выбора фазы на втором ядре
#define LiB_in_coreTwo 11    // входной PIN выбора фазы на втором ядре
#define LiC_in_coreTwo 10    // входной PIN выбора фазы на втором ядре
#define Blynk_Status 5       // PIN на светодиод отражающий статус состояния Blynk

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "itel A16 Plus_plus";
char pass[] = "Acer5560g!";

SoftwareSerial EspSerial(2, 3); // RX, TX, назначение пинов для передачи на ESP01
ESP8266 wifi(&EspSerial);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled; // подключение библиотеки дисплея

uint8_t valA, valB, valC; // тип переменной для valA = analogRead(LineA), valB = analogRead(LineB), valC = analogRead(LineC)
uint8_t hiVolts = 250;    // значение верхнего уровня отключения
uint8_t lowVvolts = 170;  // значение нижнего уровня тоключения
uint8_t setX = 0;         // переменная для корректировки по Х на дисплее
uint8_t setY = 0;         // переменная для корректировки по Y на дисплее
bool setLiA, setLiB, setLiC;

ZMPT101B voltageSensorLineA(LineA); // подключение библиотеки датчика напряжения
ZMPT101B voltageSensorLineB(LineB); // подключение библиотеки датчика напряжения
ZMPT101B voltageSensorLineC(LineC); // подключение библиотеки датчика напряжения

class MyDisplay                                                                              //
{                                                                                            //
public:                                                                                      //
    void myDisplay(uint8_t x, uint8_t y, uint8_t size_font, uint8_t volts, String text = "") //
    {
        oled.setScale(size_font);
        oled.setCursorXY(x, y); // курсор в (пиксель X, строка Y)
        oled.print(text);
        oled.print(volts);
    }
};

MyDisplay MyDis; // создание обьекта MyDis класса MyDisplay

//!================================ BLYNK_CONNECTED ========================
BLYNK_CONNECTED()
{
    // функцианал для получения приоритетных данных с сервера Blynk
    Blynk.syncVirtual(V7);
    Blynk.syncVirtual(V8);
    Blynk.syncVirtual(V9);
}
//!=================================BLYNK_WRITE(V7)=========================
BLYNK_WRITE(V7)
{
    setLiA = param.asInt();
}
//!=================================BLYNK_WRITE(V8)=========================
BLYNK_WRITE(V8)
{
    setLiB = param.asInt();
}
//!=================================BLYNK_WRITE(V9)=========================
BLYNK_WRITE(V9)
{
    setLiC = param.asInt();
}
//!======================= BlynkArguments ==================================
void swich_LiA_LiB_LiC()
{
    if (setLiA == 1)
    {
        digitalWrite(LiA_in_coreTwo, setLiA);
        Serial.println(String("setLiA ") + setLiA); /* code */
        Blynk.virtualWrite(V7, 0);
        delay(1000);
        setLiA = 0;
    }
    digitalWrite(LiA_in_coreTwo, setLiA);
    Serial.println(String("setLiA ") + setLiA);

    if (setLiB == 1)
    {
        digitalWrite(LiB_in_coreTwo, setLiB);
        Serial.println(String("setLiB ") + setLiB); /* code */
        Blynk.virtualWrite(V8, 0);
        delay(1000);

        setLiB = 0;
    }
    digitalWrite(LiB_in_coreTwo, setLiB);
    Serial.println(String("setLiB ") + setLiB);

    if (setLiC == 1)
    {
        digitalWrite(LiC_in_coreTwo, setLiC);
        Serial.println(String("setLiC ") + setLiC); /* code */
        Blynk.virtualWrite(V9, 0);
        delay(1000);

        setLiC = 0;
    }
    digitalWrite(LiC_in_coreTwo, setLiC);
    Serial.println(String("setLiC ") + setLiC);
}
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
    oled.print("Blynk connect...");
}
//!======================= Display2 ========================================
void StartDisplay2()
{
    oled.clear();         // очистить дисплей
    oled.setCursor(0, 3); // курсор в (пиксель X, строка Y)
    oled.setScale(1);
    oled.print("Blynk start...");
    delay(2000);
    oled.clear(); // очистить дисплей
}
//!======================= Line A: =========================================
void Line_A()
{
    valA = voltageSensorLineA.getVoltageAC(50) * 5;        // считывание пораметра с LineA и сохранение его в valA
    boolean statusLineA = digitalRead(LineA_from_coreOne); // сичтывание значения состояния фазы с PIN-а
    Serial.println(String("statusLineA ") + statusLineA);
    if (statusLineA == 1)
    {
        Blynk.virtualWrite(V4, 1);
        oled.invertText(true); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 1, 2, valA, "Line A:");
    }
    else
    {
        Blynk.virtualWrite(V4, 0);
        oled.invertText(false); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 1, 2, valA, "Line A:");
    }
}
//!======================= Line B: =========================================
void Line_B()
{
    valB = voltageSensorLineB.getVoltageAC(50) * 5;        // считывание пораметра с LineB и сохранение его в valB
    boolean statusLineB = digitalRead(LineB_from_coreOne); // считывания значения состояния фазы с PIN-а
    Serial.println(String("statusLineB ") + statusLineB);
    if (statusLineB == 1)
    {
        Blynk.virtualWrite(V5, 1);
        oled.invertText(true); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 24, 2, valB, "Line B:");
    }
    else
    {
        Blynk.virtualWrite(V5, 0);
        oled.invertText(false); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 24, 2, valB, "Line B:");
    }
}
//!======================= Line C: =========================================
void Line_C()
{
    valC = voltageSensorLineC.getVoltageAC(50) * 5;        // считывание пораметра с LineC и сохранение его в valC
    boolean statusLineC = digitalRead(LineC_from_coreOne); // считывание значения состояния фазы с PIN-а
    Serial.println(String("statusLineC ") + statusLineC);
    if (statusLineC == 1)
    {
        Blynk.virtualWrite(V6, 1);
        oled.invertText(true); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 48, 2, valC, "Line C:");
    }
    else
    {
        Blynk.virtualWrite(V6, 0);
        oled.invertText(false); // инвертируй текст!
        MyDis.myDisplay(setX, setY + 48, 2, valC, "Line C:");
    }
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
    pinMode(LiA_in_coreTwo, OUTPUT);     //
    pinMode(LiB_in_coreTwo, OUTPUT);     //
    pinMode(LiC_in_coreTwo, OUTPUT);     //
    pinMode(Blynk_Status, OUTPUT);       // открытие порта для статуса Blynk на выход
    StartDisplay1();                     // функция отрисовки первого экрана
    Blynk.begin(auth, wifi, ssid, pass); // подключение к Blynk
    StartDisplay2();                     // фукция отрисовки второго экрана
}
//!=========================void loop()=====================================
void loop()
{
    swich_LiA_LiB_LiC();
    Line_A();
    Line_B();
    Line_C();
    BlynkArguments();
}