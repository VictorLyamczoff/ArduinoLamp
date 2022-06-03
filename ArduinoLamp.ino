// ---------- МАТРИЦА ---------
#define BRIGHTNESS 40      // стандартная маскимальная яркость (0-255)
#define CURRENT_LIMIT 2000 // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

#define WIDTH 16  // ширина матрицы
#define HEIGHT 16 // высота матрицы

#define COLOR_ORDER GRB // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE 0      // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 0 // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 0  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"

#define numHold_Time 1500 // время отображения индикатора уровня яркости/скорости/масштаба

// ============= ДЛЯ РАЗРАБОТЧИКОВ =============
#define LED_PIN 6 // пин ленты
#define BTN_PIN 2
#define MODE_AMOUNT 18
#define NUM_LEDS WIDTH *HEIGHT
#define SEGMENTS 1     // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
#define PARSE_AMOUNT 1 // число значений в массиве, который хотим получить

// ---------------- БИБЛИОТЕКИ -----------------
//#include "timerMinim.h"
#include <EEPROM.h>
#include <FastLED.h>
#include <GyverButton.h>

// ------------------- ТИПЫ --------------------
CRGB leds[NUM_LEDS];

GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN); //сенсорная кнопка
// GButton touch(BTN_PIN, HIGH_PULL, NORM_OPEN);  //механическая кнопка

// ----------------- ПЕРЕМЕННЫЕ ------------------
static const byte maxDim = max(WIDTH, HEIGHT);
int8_t currentMode = 1;
boolean loadingFlag = true;
boolean ONflag = true;
byte numHold = 0;
unsigned long numHold_Timer = 0;

boolean recievedFlag;
boolean getStarted;
byte index;
String string_convert = "";
byte intData[PARSE_AMOUNT]; // массив численных значений после парсинга
unsigned char matrixValue[8][16];

struct
{
  byte brightness = 50;
  byte speed = 30;
  byte scale = 10;
} modes[MODE_AMOUNT];

// ==================== Bluetooth ================================
void bluetooth()
{
  if (recievedFlag)
  { // если получены данные
    recievedFlag = false;

    // Serial.println(intData[0]);

    switch (intData[0])
    {
    case 1:
      if (ONflag)
      {
        ONflag = false;
        changePower();
      }
      else
      {
        ONflag = true;
        changePower();
      }
      break;

    case 2:
      if (--currentMode < 0)
        currentMode = MODE_AMOUNT;
      FastLED.setBrightness(modes[currentMode].brightness);
      loadingFlag = true;
      memset8(leds, 0, NUM_LEDS * 3);

      delay(1);
      break;

    case 3:
      if (++currentMode > MODE_AMOUNT)
        currentMode = 0;
      FastLED.setBrightness(modes[currentMode].brightness);
      loadingFlag = true;
      memset8(leds, 0, NUM_LEDS * 3);

      delay(1);
      break;

    case 4:
      modes[currentMode].brightness = constrain(modes[currentMode].brightness + 1, 1, 255);

      FastLED.setBrightness(modes[currentMode].brightness);
      loadingFlag = true;
      break;

    case 5:
      modes[currentMode].brightness = constrain(modes[currentMode].brightness - 1, 1, 255);

      FastLED.setBrightness(modes[currentMode].brightness);
      loadingFlag = true;
      break;

    case 6:
      modes[currentMode].scale = constrain(modes[currentMode].scale + 1, 1, 255);

      loadingFlag = true;
      break;

    case 7:
      modes[currentMode].scale = constrain(modes[currentMode].scale - 1, 1, 255);

      loadingFlag = true;
      break;

    case 8:
      modes[currentMode].speed = constrain(modes[currentMode].speed - 1, 1, 255);

      loadingFlag = true;
      break;

    case 9:
      modes[currentMode].speed = constrain(modes[currentMode].speed + 1, 1, 255);

      loadingFlag = true;
      break;

    case 10:
      if (EEPROM.read(0) != 102)
        EEPROM.write(0, 102);
      if (EEPROM.read(1) != currentMode)
        EEPROM.write(1, currentMode); // запоминаем текущий эффект
      for (byte x = 0; x < MODE_AMOUNT; x++)
      { // сохраняем настройки всех режимов
        if (EEPROM.read(x * 3 + 11) != modes[x].brightness)
          EEPROM.write(x * 3 + 11, modes[x].brightness);
        if (EEPROM.read(x * 3 + 12) != modes[x].speed)
          EEPROM.write(x * 3 + 12, modes[x].speed);
        if (EEPROM.read(x * 3 + 13) != modes[x].scale)
          EEPROM.write(x * 3 + 13, modes[x].scale);
      }
      // индикация сохранения
      showWarning(CHSV(96, 255, 255), 1000, 200);
      break;

    default:
      break;
    }
    memset(intData, 0, PARSE_AMOUNT); // очищаем массив принятых данных
  }

  parsing();
}

// ======================== Парсинг ==============================
void parsing()
{
  if (Serial.available() > 0)
  {
    char incomingByte = Serial.read(); // обязательно ЧИТАЕМ входящий символ
    if (getStarted)
    { // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ',' && incomingByte != ';')
      {                                 // если это не запятая И не конец
        string_convert += incomingByte; // складываем в строку
      }
      else
      {                                          // если это запятая или ; конец пакета
        intData[index] = string_convert.toInt(); // преобразуем строку в int и кладём в массив
        string_convert = "";                     // очищаем строку
        index++;                                 // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$')
    {                      // если это $
      getStarted = true;   // поднимаем флаг, что можно парсить
      index = 0;           // сбрасываем индекс
      string_convert = ""; // очищаем строку
    }
    if (incomingByte == ';')
    {                      // если таки приняли ; - конец парсинга
      getStarted = false;  // сброс
      recievedFlag = true; // флаг на принятие
    }
  }
}

void setup()
{
  // ЛЕНТА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0)
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.clear();
  FastLED.show();

  touch.setStepTimeout(100);
  touch.setClickTimeout(500);

  Serial.begin(9600);
  Serial.println();

  if (EEPROM.read(0) == 102)
  { // если было сохранение настроек, то восстанавливаем их (с)НР
    currentMode = EEPROM.read(1);
    for (byte x = 0; x < MODE_AMOUNT; x++)
    {
      modes[x].brightness = EEPROM.read(x * 3 + 11); // (2-10 байт - резерв)
      modes[x].speed = EEPROM.read(x * 3 + 12);
      modes[x].scale = EEPROM.read(x * 3 + 13);
    }
    FastLED.setBrightness(modes[currentMode].brightness);
  }
}

void loop()
{
  effectsTick();

  buttonTick();

  bluetooth();
}
