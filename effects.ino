// ================================= ЭФФЕКТЫ ====================================
// // =============================================================
// void blurScreen(fract8 blur_amount, CRGB *LEDarray = leds)
// {
//   blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
// }

// --------------------------------- конфетти ------------------------------------
void sparklesRoutine()
{
  for (byte i = 0; i < modes[0].scale; i++)
  {
    byte x = random(0, WIDTH);
    byte y = random(0, HEIGHT);
    if (getPixColorXY(x, y) == 0)
      leds[getPixelNumber(x, y)] = CHSV(random(0, 255), 255, 255);
  }
  fader(70);
}

// функция плавного угасания цвета для всех пикселей
void fader(byte step)
{
  for (byte i = 0; i < WIDTH; i++)
  {
    for (byte j = 0; j < HEIGHT; j++)
    {
      fadePixel(i, j, step);
    }
  }
}
void fadePixel(byte i, byte j, byte step)
{ // новый фейдер
  int pixelNum = getPixelNumber(i, j);
  if (getPixColor(pixelNum) == 0)
    return;

  if (leds[pixelNum].r >= 30 ||
      leds[pixelNum].g >= 30 ||
      leds[pixelNum].b >= 30)
  {
    leds[pixelNum].fadeToBlackBy(step);
  }
  else
  {
    leds[pixelNum] = 0;
  }
}

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"
#define SPARKLES 1 // вылетающие угольки вкл выкл
unsigned char line[WIDTH];
int pcnt = 0;

const unsigned char valueMask[8][16] PROGMEM = {
    {32, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 32},
    {64, 0, 0, 0, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 64},
    {96, 32, 0, 0, 0, 0, 32, 96, 96, 32, 0, 0, 0, 0, 32, 96},
    {128, 64, 32, 0, 0, 32, 64, 128, 128, 64, 32, 0, 0, 32, 64, 128},
    {160, 96, 64, 32, 32, 64, 96, 160, 160, 96, 64, 32, 32, 64, 96, 160},
    {192, 128, 96, 64, 64, 96, 128, 192, 192, 128, 96, 64, 64, 96, 128, 192},
    {255, 160, 128, 96, 96, 128, 160, 255, 255, 160, 128, 96, 96, 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}};

const unsigned char hueMask[8][16] PROGMEM = {
    {1, 11, 19, 25, 25, 22, 11, 1, 1, 11, 19, 25, 25, 22, 11, 1},
    {1, 8, 13, 19, 25, 19, 8, 1, 1, 8, 13, 19, 25, 19, 8, 1},
    {1, 8, 13, 16, 19, 16, 8, 1, 1, 8, 13, 16, 19, 16, 8, 1},
    {1, 5, 11, 13, 13, 13, 5, 1, 1, 5, 11, 13, 13, 13, 5, 1},
    {1, 5, 11, 11, 11, 11, 5, 1, 1, 5, 11, 11, 11, 11, 5, 1},
    {0, 1, 5, 8, 8, 5, 1, 0, 0, 1, 5, 8, 8, 5, 1, 0},
    {0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}};

void fireRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    generateLine();
  }
  if (pcnt >= 100)
  {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 30;
}

// Случайным образом генерирует следующую линию (matrix row)

void generateLine()
{
  for (uint8_t x = 0; x < WIDTH; x++)
  {
    line[x] = random(128, 255);
  }
}

void shiftUp()
{
  for (uint8_t y = HEIGHT - 1; y > 0; y--)
  {
    for (uint8_t x = 0; x < WIDTH; x++)
    {
      uint8_t newX = x;
      if (x > 15)
        newX = x - 15;
      if (y > 7)
        continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++)
  {
    uint8_t newX = x;
    if (x > 15)
      newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// рисует кадр, интерполируя между 2 "ключевых кадров"
// параметр pcnt - процент интерполяции

void drawFrame(int pcnt)
{
  int nextv;

  for (unsigned char y = HEIGHT - 1; y > 0; y--)
  {
    for (unsigned char x = 0; x < WIDTH; x++)
    {
      uint8_t newX = x;
      if (x > 15)
        newX = x - 15;
      if (y < 8)
      {
        nextv =
            (((100.0 - pcnt) * matrixValue[y][newX] + pcnt * matrixValue[y - 1][newX]) / 100.0) - pgm_read_byte(&(valueMask[y][newX]));

        CRGB color = CHSV(
            modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[y][newX])), // H
            255,                                                       // S
            (uint8_t)max(0, nextv)                                     // V
        );

        leds[getPixelNumber(x, y)] = color;
      }
      else if (y == 8 && SPARKLES)
      {
        if (random(0, 20) == 0 && getPixColorXY(x, y - 1) != 0)
          drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else
          drawPixelXY(x, y, 0);
      }
      else if (SPARKLES)
      {

        // старая версия для яркости
        if (getPixColorXY(x, y - 1) > 0)
          drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else
          drawPixelXY(x, y, 0);
      }
    }
  }

  //Перавя стрка интерполируется со следующей "next" линией
  for (unsigned char x = 0; x < WIDTH; x++)
  {
    uint8_t newX = x;
    if (x > 15)
      newX = x - 15;
    CRGB color = CHSV(
        modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[0][newX])),                     // H
        255,                                                                           // S
        (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
    );
    leds[getPixelNumber(newX, 0)] = color;
  }
}

byte hue;
// ---------------------------------------- радуга ------------------------------------------
void rainbowVertical()
{
  hue += 2;
  for (byte j = 0; j < HEIGHT; j++)
  {
    CHSV thisColor = CHSV((byte)(hue + j * modes[2].scale), 255, 255);
    for (byte i = 0; i < WIDTH; i++)
      drawPixelXY(i, j, thisColor);
  }
}
void rainbowHorizontal()
{
  hue += 2;
  for (byte i = 0; i < WIDTH; i++)
  {
    CHSV thisColor = CHSV((byte)(hue + i * modes[3].scale), 255, 255);
    for (byte j = 0; j < HEIGHT; j++)
      drawPixelXY(i, j, thisColor); // leds[getPixelNumber(i, j)] = thisColor;
  }
}

// ---------------------------------------- ЦВЕТА ------------------------------------------
void colorsRoutine()
{
  hue += modes[4].scale;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
void colorRoutine()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(modes[14].scale, 255, 255);
  }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine()
{
  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++)
  {
    for (byte y = 0; y < HEIGHT - 1; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }

  for (byte x = 0; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, HEIGHT - 2) == 0 && (random(0, modes[15].scale) == 0))
      drawPixelXY(x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, HEIGHT - 1, 0x000000);
  }
}

// ------------------------------ МАТРИЦА ------------------------------
void matrixRoutine()
{
  for (byte x = 0; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1);
    if (thisColor == 0)
      drawPixelXY(x, HEIGHT - 1, 0x00FF00 * (random(0, modes[16].scale) == 0));
    else if (thisColor < 0x002000)
      drawPixelXY(x, HEIGHT - 1, 0);
    else
      drawPixelXY(x, HEIGHT - 1, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++)
  {
    for (byte y = 0; y < HEIGHT - 1; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}

// ------------------------------ БЕЛАЯ ЛАМПА ------------------------------
void whiteLamp()
{
  for (byte y = 0; y < (HEIGHT / 2); y++)
  {
    CHSV color = CHSV(100, 1, constrain(modes[17].brightness - (long)modes[17].speed * modes[17].brightness / 255 * y / 2, 1, 255));
    for (byte x = 0; x < WIDTH; x++)
    {
      drawPixelXY(x, y + 8, color);
      drawPixelXY(x, 7 - y, color);
    }
  }
}

// // ------------------------------ ЛАВОЛАМПА ------------------------------
// #define LIGHTERS_AM ((WIDTH + HEIGHT) / 4)
// int16_t lightersPos[2][LIGHTERS_AM];
// int8_t lightersSpeed[2][LIGHTERS_AM];
// byte lightersColor[LIGHTERS_AM];
// byte lightersBright[LIGHTERS_AM];

// float ball[(WIDTH / 2) - ((WIDTH - 1) & 0x01)][2];

// void lavaLamp()
// {
//   if (loadingFlag)
//   {
//     for (byte i = 0; i < (WIDTH / 2) - ((WIDTH - 1) & 0x01); i++)
//     {
//       lightersSpeed[0][i] = random(1, 3);
//       ball[i][1] = (float)random8(5, 11) / (modes[18].speed) / 4.0;
//       ball[i][0] = 0;
//       lightersSpeed[1][i] = i * 2U + random8(2);
//     }
//     loadingFlag = false;
//   }
//   LavaLampRoutine();
// }
// void drawBlob(uint8_t l, CRGB color)
// { //раз круги нарисовать не получается, будем попиксельно вырисовывать 2 варианта пузырей
//   if (lightersSpeed[0][l] == 2)
//   {
//     for (int8_t x = -2; x < 3; x++)
//       for (int8_t y = -2; y < 3; y++)
//         if (abs(x) + abs(y) < 4)
//           drawPixelXYF_Y(lightersSpeed[1][l] + x, ball[l][0] + y, color);
//   }
//   else
//   {
//     for (int8_t x = -1; x < 3; x++)
//       for (int8_t y = -1; y < 3; y++)
//         if (!(x == -1 && (y == -1 || y == 2) || x == 2 && (y == -1 || y == 2)))
//           drawPixelXYF_Y(lightersSpeed[1][l] + x, ball[l][0] + y, color);
//   }
// }

// void LavaLampRoutine()
// {
//   dimAll(100);
//   for (byte i = 0; i < (WIDTH / 2) - ((WIDTH - 1) & 0x01); i++)
//   {
//     if (modes[18].scale == 1)
//     {
//       drawBlob(i, CHSV(hue, 255, 255));
//       hue++;
//     }
//     else
//       drawBlob(i, CHSV(modes[18].scale, 255, 255));

//     if (ball[i][0] + lightersSpeed[0][i] >= HEIGHT - 1)
//       ball[i][0] += (ball[i][1] * ((HEIGHT - 1 - ball[i][0]) / lightersSpeed[0][i] + 0.005));
//     else if (ball[i][0] - lightersSpeed[0][i] <= 0)
//       ball[i][0] += (ball[i][1] * (ball[i][0] / lightersSpeed[0][i] + 0.005));
//     else
//       ball[i][0] += ball[i][1];
//     if (ball[i][0] < 0.01)
//     { // почему-то при нуле появляется мерцание (один кадр, еле заметно)
//       ball[i][1] = (float)random8(5, 11) / (257U - modes[18].speed) / 4.0;
//       ball[i][0] = 0.01;
//     }
//     else if (ball[i][0] > HEIGHT - 1.01)
//     { // тоже на всякий пожарный
//       ball[i][1] = (float)random8(5, 11) / (257U - modes[18].speed) / 4.0;
//       ball[i][1] = -ball[i][1];
//       ball[i][0] = HEIGHT - 1.01;
//     }
//   }
// }

//// ----------------------------- СВЕТЛЯКИ ------------------------------ в ардуино под него не хватает памяти
// #define LIGHTERS_AM 100
// int lightersPos[2][LIGHTERS_AM];
// int8_t lightersSpeed[2][LIGHTERS_AM];
// CHSV lightersColor[LIGHTERS_AM];
// byte loopCounter;

// int angle[LIGHTERS_AM];
// int speedV[LIGHTERS_AM];
// int8_t angleSpeed[LIGHTERS_AM];

// void lightersRoutine()
// {
//   if (loadingFlag)
//   {
//     loadingFlag = false;
//     randomSeed(millis());
//     for (byte i = 0; i < LIGHTERS_AM; i++)
//     {
//       lightersPos[0][i] = random(0, WIDTH * 10);
//       lightersPos[1][i] = random(0, HEIGHT * 10);
//       lightersSpeed[0][i] = random(-10, 10);
//       lightersSpeed[1][i] = random(-10, 10);
//       lightersColor[i] = CHSV(random(0, 255), 255, 255);
//     }
//   }
//   FastLED.clear();
//   if (++loopCounter > 20)
//     loopCounter = 0;
//   for (byte i = 0; i < modes[17].scale; i++)
//   {
//     if (loopCounter == 0)
//     { // меняем скорость каждые 255 отрисовок
//       lightersSpeed[0][i] += random(-3, 4);
//       lightersSpeed[1][i] += random(-3, 4);
//       lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
//       lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
//     }

//     lightersPos[0][i] += lightersSpeed[0][i];
//     lightersPos[1][i] += lightersSpeed[1][i];

//     if (lightersPos[0][i] < 0)
//       lightersPos[0][i] = (WIDTH - 1) * 10;
//     if (lightersPos[0][i] >= WIDTH * 10)
//       lightersPos[0][i] = 0;

//     if (lightersPos[1][i] < 0)
//     {
//       lightersPos[1][i] = 0;
//       lightersSpeed[1][i] = -lightersSpeed[1][i];
//     }
//     if (lightersPos[1][i] >= (HEIGHT - 1) * 10)
//     {
//       lightersPos[1][i] = (HEIGHT - 1) * 10;
//       lightersSpeed[1][i] = -lightersSpeed[1][i];
//     }
//     drawPixelXY(lightersPos[0][i] / 10, lightersPos[1][i] / 10, lightersColor[i]);
//   }
// }
