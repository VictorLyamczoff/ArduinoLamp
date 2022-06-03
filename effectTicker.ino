uint32_t effTimer;
// byte ind;
// byte xPosition[] = {0, 4, 8, 12};

void effectsTick()
{
  {
    if (ONflag && millis() - effTimer >= ((currentMode < 5 || currentMode > 13) ? modes[currentMode].speed : 50))
    {
      effTimer = millis();
      switch (currentMode)
      {
      case 0:
        sparklesRoutine();
        break;
      case 1:
        fireRoutine();
        break;
      case 2:
        rainbowVertical();
        break;
      case 3:
        rainbowHorizontal();
        break;
      case 4:
        colorsRoutine();
        break;
      case 5:
        madnessNoise();
        break;
      case 6:
        cloudNoise();
        break;
      case 7:
        lavaNoise();
        break;
      case 8:
        plasmaNoise();
        break;
      case 9:
        rainbowNoise();
        break;
      case 10:
        rainbowStripeNoise();
        break;
      case 11:
        zebraNoise();
        break;
      case 12:
        forestNoise();
        break;
      case 13:
        oceanNoise();
        break;
      case 14:
        colorRoutine();
        break;
      case 15:
        snowRoutine();
        break;
      case 16:
        matrixRoutine();
        break;
      case 17:
        whiteLamp();
        break;
      case 18:
        RainRoutine();
        break;
      case 19:
        rainbowDiagonalRoutine();
        break;
      case 20:
        stormRoutine();
        break;
        // case 18:
        //   lavaLamp();
        //   break;
      }
      // switch (numHold)
      // { // индикатор уровня яркости/скорости/масштаба
      // case 1:
      //   ind = constrain(modes[currentMode].brightness / 16, 1, 255);
      //   Serial.println(ind);
      //   for (byte x = 0; x < 4; x++)
      //     for (byte y = 0; y < HEIGHT; y++)
      //     {
      //       if (ind > y)
      //         drawPixelXY(xPosition[x], y, CHSV(10, 255, 255));
      //       else
      //         drawPixelXY(xPosition[x], y, 0);
      //     }
      //   break;
      // case 2:
      //   ind = constrain(modes[currentMode].speed - 16, 1, 255);
      //   for (byte x = 0; x < 4; x++)
      //     for (byte y = 0; y <= HEIGHT; y++)
      //     {
      //       if (ind <= y)
      //         drawPixelXY(xPosition[x], y, CHSV(100, 255, 255));
      //       else
      //         drawPixelXY(xPosition[x], y, 0);
      //     }
      //   break;
      // case 3:
      //   ind = constrain(modes[currentMode].scale / 16, 1, 255);
      //   for (byte x = 0; x < 4; x++)
      //     for (byte y = 0; y < HEIGHT; y++)
      //     {
      //       if (ind > y)
      //         drawPixelXY(xPosition[x], y, CHSV(150, 255, 255));
      //       else
      //         drawPixelXY(xPosition[x], y, 0);
      //     }
      //   break;
      // }
      FastLED.show();
    }
  }
}

void changePower()
{ // плавное включение/выключение
  if (ONflag)
  {
    effectsTick();
    for (int i = 0; i < modes[currentMode].brightness; i += 8)
    {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.setBrightness(modes[currentMode].brightness);
    delay(2);
    FastLED.show();
  }
  else
  {
    effectsTick();
    for (int i = modes[currentMode].brightness; i > 8; i -= 8)
    {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.clear();
    delay(2);
    FastLED.show();
  }
}
