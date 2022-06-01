boolean brightDirection, speedDirection, scaleDirection;
// byte numHold;
uint32_t tickTimer = 0;

boolean ticker()
{
  if ((millis() - tickTimer >= 100))
  {
    tickTimer = millis();
    return true;
  }
  else
    return false;
}

void buttonTick()
{
  touch.tick();

  if (ONflag)
  { // если включено

    if ((touch.hasClicks()) && (touch.getClicks() == 5))
    { // если было пятикратное нажатие на кнопку, то производим сохранение параметров // && (touch.hasClicks())
      if (EEPROM.read(0) != 102)
        EEPROM.write(0, 102);
      if (EEPROM.read(1) != currentMode)
        EEPROM.write(1, currentMode); // запоминаем текущий эфект
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
      ONflag = false;
      changePower();
      delay(200);
      ONflag = true;
      changePower();
    }

    if (touch.isHold() && ticker())
    {
      switch (touch.getHoldClicks() + 1)
      {
      case 1:
        if (numHold != 1)
          brightDirection = !brightDirection;

        modes[currentMode].brightness = constrain(modes[currentMode].brightness + 5 * (brightDirection * 2 - 1), 1, 255);
        numHold = 1;
        break;

      case 2:
        if (numHold != 2)
          speedDirection = !speedDirection;

        modes[currentMode].speed = constrain(modes[currentMode].speed + 5 * (speedDirection * 2 - 1), 1, 255);
        numHold = 2;
        break;

      case 3:
        if (numHold != 3)
          scaleDirection = !scaleDirection;

        modes[currentMode].scale = constrain(modes[currentMode].scale + 5 * (scaleDirection * 2 - 1), 1, 255);
        numHold = 3;
        break;
      }

      if (numHold != 0)
        numHold_Timer = millis();
      loadingFlag = true;
    }

    if ((millis() - numHold_Timer) > numHold_Time)
    {
      numHold = 0;
      numHold_Timer = millis();
    }
    FastLED.setBrightness(modes[currentMode].brightness);

    if (touch.isDouble())
    {
      if (++currentMode >= MODE_AMOUNT)
        currentMode = 0;

      FastLED.setBrightness(modes[currentMode].brightness);

      loadingFlag = true;

      FastLED.clear();

      delay(1);
    }
    if (touch.isTriple())
    {
      if (--currentMode < 0)
        currentMode = MODE_AMOUNT - 1;

      FastLED.setBrightness(modes[currentMode].brightness);

      loadingFlag = true;

      FastLED.clear();

      delay(1);
    }
  }

  if (touch.isSingle())
  {
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
  }
}
