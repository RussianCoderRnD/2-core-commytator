////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                        //
//   Комутатор фаз выполняет смену фазы для потребителя. когда входное напряжение ниже 170 вольт или      //
// выше 250 вольт. Эти значения задаются в переменных "lowVolts" и "hiVolts". Также можно установить      //
// время задержки переключения фаз в переменной "switchingDeflagLine_Ay". для семисторных переключателей  //
// хватает "50", но для магнитных пускателей возможна необходимость изменить длительность                 //
// задержки воизбежания смыкания фаз.                                                                     //
// Флаги "flagLine_A/flagLine_B/flagLine_C" используются для недопущения вклюения одновременно наскольких //
// фаз переменной "U" присваекается новое значения согласно тому какая фазавключена                       //
//   P.S: рекомендация очевидная, но всёже: колибровать датчики напряжения лучше когда на все три         //
// датчика подаётся одна и таже фазаю тем самым колибруются все три датчика на одном опорном напряжении   //
//                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <ZMPT101B.h> // библиотека управления датчиком напряжения

#define LineA A1           // входной PIN LineA
#define LineB A2           // входной PIN LineB
#define LineC A3           // входной PIN LineC
#define OUTLineA 12        // выходной PIN LineA
#define OUTLineB 11        // выходной PIN LineB
#define OUTLineC 10        // выходной PIN LineC
#define OUTLineA_oneCore 9 // выходной PIN LineA
#define OUTLineB_oneCore 8 // выходной PIN LineB
#define OUTLineC_oneCore 7 // выходной PIN LineC
#define setLineA 6         // входной PIN для выблра LineA
#define setLineB 5         // входной PIN для выбора LineB
#define setLineC 4         // входной PIN для выбора LineC

ZMPT101B voltageSensorLineA(LineA); // активаци датчика на пряжения с ПИНом
ZMPT101B voltageSensorLineB(LineB); // активаци датчика на пряжения с ПИНом
ZMPT101B voltageSensorLineC(LineC); // активаци датчика на пряжения с ПИНом

uint8_t U;                                          // переменная для вывода напряжение на дисплей
uint8_t valA, valB, valC;                           // тип переменной для valA = analogRead(LineA), valB = analogRead(LineB), valC = analogRead(LineC)
uint8_t hiVolts = 250;                              // значение верхнего уровня отключения
uint8_t lowVolts = 170;                             // значение нижнего уровня тоключения
boolean flagLine_A, flagLine_B, flagLine_C = false; // тип переменной для FLAG flagLine_A, flagLine_B, LC и присвоение " 0 " ;
uint8_t switchingDelay = 50;                        //длительность задержки включения исполнительного устройства (твердотельного реле/ магнитного пускателя)
bool LiA, LiB, LiC;
//!============================== checkingSelectionLine ====================================
void checkingSelectionLine()
{
    if (LiA = digitalRead(6))
    {
        LiA = 1;
    }
    else
    {
        LiA = 0;
    }
    Serial.println(String("LiA ") + LiA);
    if (LiB = digitalRead(5))
    {
        LiB = 1;
    }
    else
    {
        LiB = 0;
    }
    Serial.println(String("LiB ") + LiB);
    if (LiC = digitalRead(4))
    {
        LiC = 1;
    }
    else
    {
        LiC = 0;
    }
    Serial.println(String("LiC ") + LiC);
    delay(1000);
}
//!============================= void setup() ==============================================
void setup()
{
    Serial.begin(9600); // открытие последовательного порта

    pinMode(LineA, INPUT);    // открытие порта на ВХОД
    pinMode(LineB, INPUT);    // открытие порта на ВХОД
    pinMode(LineA, INPUT);    // открытие порта на ВХОД
    pinMode(setLineA, INPUT); // открытие порта на ВХОД для установки коммутатора на фазу А
    pinMode(setLineB, INPUT); // открытие порта на ВХОД для установки коммутатора на фазу В
    pinMode(setLineC, INPUT); // открытие порта на ВХОД для установки коммутатора на фазу С

    pinMode(OUTLineA, OUTPUT);         // открытие порта OUTLineA на выход
    pinMode(OUTLineB, OUTPUT);         // открытие порта OUTLineB на выход
    pinMode(OUTLineC, OUTPUT);         // открытие порта OUTLineC на выход
    pinMode(OUTLineA_oneCore, OUTPUT); // открытие порта OUTLineA_oneCore на выход
    pinMode(OUTLineB_oneCore, OUTPUT); // открытие порта OUTLineB_oneCore на выход
    pinMode(OUTLineC_oneCore, OUTPUT); // открытие порта OUTLineC_oneCore на выход

    digitalWrite(OUTLineA, LOW);         // установка состояния OUTLineA = 0
    digitalWrite(OUTLineB, LOW);         // установка состояния OUTLineB = 0
    digitalWrite(OUTLineC, LOW);         // установка состояния OUTLineC = 0
    digitalWrite(OUTLineA_oneCore, LOW); // установка состояния OUTLineA_oneCore = 0
    digitalWrite(OUTLineB_oneCore, LOW); // установка состояния OUTLineB_oneCore = 0
    digitalWrite(OUTLineC_oneCore, LOW); // установка состояния OUTLineC_oneCore = 0
}
//!============================= void loop() ===============================================
void loop()
{
    valA = voltageSensorLineA.getVoltageAC(50) * 5; // считывание пораметра с LineA и сохранение его в valA
    valB = voltageSensorLineB.getVoltageAC(50) * 5; // считывание пораметра с LineB и сохранение его в valB
    valC = voltageSensorLineC.getVoltageAC(50) * 5; // считывание пораметра с LineC и сохранение его в valC

    if (LiA == 1 || valA >= lowVolts && valA <= hiVolts && valA > 150 && (flagLine_B + flagLine_C == false))
    {                                         // если valA >= 170 И valA <=249 И valA > 150 && (LB+LC == false) то
        flagLine_A = 1;                       // состояние флага flagLine_A = 1
        digitalWrite(OUTLineB, LOW);          // установка состояния OUTLineB = HIGH
        digitalWrite(OUTLineC, LOW);          // установка состояния OUTLineC = HIGH
        delay(switchingDelay);                // длительность задержки включения исполнительного устройства
        digitalWrite(OUTLineA, HIGH);         // установка состояния OUTLineA = LOW
        U = valA;                             // присвоение переменной U присваевается значение фазы valA
        digitalWrite(OUTLineB_oneCore, LOW);  // установка состояния OUTLineB_oneCore = 1
        digitalWrite(OUTLineC_oneCore, LOW);  // установка состояния OUTLineC_oneCore = 1
        digitalWrite(OUTLineA_oneCore, HIGH); // установка состояния OUTLineA_oneCore = 1
    }
    else // если условие if не выполненоб тогда
    {
        flagLine_A = 0; // состояние флага flagLine_A = 0
    }

    if (LiB == 1 || valB >= lowVolts && valB <= hiVolts && valB > 150 && (flagLine_A + flagLine_C == false))
    {
        flagLine_B = 1;
        digitalWrite(OUTLineA, LOW);
        digitalWrite(OUTLineC, LOW);
        delay(switchingDelay); // длительность задержки включения исполнительного устройства
        digitalWrite(OUTLineB, HIGH);
        U = valB;
        digitalWrite(OUTLineA_oneCore, LOW);  // установка состояния OUTLineA_oneCore = 1
        digitalWrite(OUTLineC_oneCore, LOW);  // установка состояния OUTLineC_oneCore = 1
        digitalWrite(OUTLineB_oneCore, HIGH); // установка состояния OUTLineB_oneCore = 1
    }
    else
    {
        flagLine_B = 0;
    }

    if (LiC == 1 || valC >= lowVolts && valC <= hiVolts && valC > 150 && (flagLine_A + flagLine_B == false))
    {
        flagLine_C = 1;
        digitalWrite(OUTLineB, LOW);
        digitalWrite(OUTLineA, LOW);
        delay(switchingDelay); //длительность задержки включения исполнительного устройства
        digitalWrite(OUTLineC, HIGH);
        U = valC;
        digitalWrite(OUTLineB_oneCore, LOW);  // установка состояния OUTLineB_oneCore = 1
        digitalWrite(OUTLineA_oneCore, LOW);  // установка состояния OUTLineA_oneCore = 1
        digitalWrite(OUTLineC_oneCore, HIGH); // установка состояния OUTLineC_oneCore = 1
    }
    else
    {
        flagLine_C = 0;
    }
    //!-----------------------------------------------------------------------------------------
    if (valA < lowVolts || valA > hiVolts)
    {
        digitalWrite(OUTLineA, LOW);
        digitalWrite(OUTLineA_oneCore, LOW);
    }

    if (valB < lowVolts || valB > hiVolts)
    {
        digitalWrite(OUTLineB, LOW);
        digitalWrite(OUTLineB_oneCore, LOW);
    }

    if (valC < lowVolts || valC > hiVolts)
    {
        digitalWrite(OUTLineC, LOW);
        digitalWrite(OUTLineC_oneCore, LOW);
    }
}
