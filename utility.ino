// служебные функции

// залить все
void fillAll(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }
}

// функция отрисовки точки по координатам X Y
void drawPixelXY(int8_t x, int8_t y, CRGB color)
{
  if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
    return;
  int thisPixel = getPixelNumber(x, y) * SEGMENTS;
  for (byte i = 0; i < SEGMENTS; i++)
  {
    leds[thisPixel + i] = color;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisSegm)
{
  int thisPixel = thisSegm * SEGMENTS;
  if (thisPixel < 0 || thisPixel > NUM_LEDS - 1)
    return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8) | (long)leds[thisPixel].b);
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(int8_t x, int8_t y)
{
  return getPixColor(getPixelNumber(x, y));
}
// =================================================================
void drawPixelXYF_Y(uint16_t x, float y, const CRGB &color)
{
  if (x < 0 || y < 0 || x > ((float)WIDTH) || y > ((float)HEIGHT))
    return;

  uint8_t yy = (y - (int)y) * 255, iy = 255 - yy;
  uint8_t wu[2] = {iy, yy};

  for (int8_t i = 1; i >= 0; i--)
  {
    int16_t yn = y + (i & 1);
    CRGB clr = getPixColorXY(x, yn);
    if (yn > 0 && yn < (int)HEIGHT - 1)
    {
      clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
      clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
      clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    }
    else if (yn == 0 || yn == (int)HEIGHT - 1)
    {
      clr.r = qadd8(clr.r, (color.r * 85) >> 8);
      clr.g = qadd8(clr.g, (color.g * 85) >> 8);
      clr.b = qadd8(clr.b, (color.b * 85) >> 8);
    }
    drawPixelXY(x, yn, clr);
  }
}

// =============================================================
void dimAll(uint8_t value)
{
  fadeToBlackBy(leds, NUM_LEDS, 255U - value);
}

// **************** НАСТРОЙКА МАТРИЦЫ ****************
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y (WIDTH - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y (WIDTH - x - 1)

#else
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif

// получить номер пикселя в ленте по координатам
uint16_t getPixelNumber(int8_t x, int8_t y)
{
  if ((THIS_Y % 2 == 0) || MATRIX_TYPE)
  { // если чётная строка
    return (THIS_Y * _WIDTH + THIS_X);
  }
  else
  { // если нечётная строка
    return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1);
  }
}

// ============================================================
uint16_t XY(uint8_t x, uint8_t y)
{
  if (!(THIS_Y & 0x01) || MATRIX_TYPE)
    return (THIS_Y * _WIDTH + THIS_X);
  else
    return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1);
}
