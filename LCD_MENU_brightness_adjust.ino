#include <LiquidCrystal.h>
#include <EEPROM.h>
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define KEY_NONE   0
#define KEY_RIGHT  1
#define KEY_UP     2
#define KEY_DOWN   3
#define KEY_LEFT   4
#define KEY_SELECT 5

int pot;
uint8_t numChr    = 47;
uint8_t cursorPos = 6;
uint8_t ctrPass   = 0;
uint8_t oldKey;

uint16_t lightLevelAddress      = EEPROM.length() - 11;    //244
uint16_t customChrIdxAddress    = EEPROM.length() - 15;    //240
uint16_t customChrCursorAddress = EEPROM.length() - 18;    //237

uint8_t customChrIdx    = 0;
uint8_t customChrCursor = 8;

char newPassword[7];
char password[7]        = "222222";

boolean sensing;
boolean enter           = false;
uint16_t lightLevel     = 255;
uint8_t lcd_PWM         = 10;

byte bar_7[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte bar_6[8] = {
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte bar_5[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte bar_4[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte bar_3[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte bar_2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111
};
byte bar_1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111
};

byte bar_0[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111
};

uint8_t getKey()
{
  int16_t adc0, adc;
  do
  {
    adc0 = analogRead(0);
    delay(20);
    adc = analogRead(0);
  }
  while (abs(adc - adc0) > 20);
  if (adc < 50)
  {
    return KEY_RIGHT;
  }
  else if (adc < 220)
  {
    return KEY_UP;
  }
  else if (adc < 340)
  {
    return KEY_DOWN;
  }
  else if (adc < 550)
  {
    return KEY_LEFT;
  }
  else if (adc < 770)
  {
    return KEY_SELECT;
  }
  return KEY_NONE;
}

void sensor(void)
{
  pot = analogRead(2);
  lcd.setCursor(0,0);
  lcd.print("Sensor : ");
  lcd.print(pot);
}

void setup() 
{
  if (EEPROM[lightLevelAddress] == 0)
  {
    EEPROM.write(lightLevelAddress, lightLevel);
  }
  if (EEPROM[customChrIdxAddress] == 0)
  {
    EEPROM.write(customChrIdxAddress, customChrIdx);
  }
  if (EEPROM[customChrCursorAddress] == 0)
  {
    EEPROM.write(customChrCursorAddress, customChrCursor);
  }
  
  Serial.begin(115200);
  pinMode(lcd_PWM, OUTPUT);
  DDRB |= bit(4);   // Set D12 as output
  DDRC |= bit(1);
  pinMode(A2, INPUT);
  Serial.println(F("PIR Interrupt Test...\n\n"));
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("MOTION  DETECTOR");
  lcd.createChar(0, bar_0);
  lcd.createChar(1, bar_1);
  lcd.createChar(2, bar_2);
  lcd.createChar(3, bar_3);
  lcd.createChar(4, bar_4);
  lcd.createChar(5, bar_5);
  lcd.createChar(6, bar_6);
  lcd.createChar(7, bar_7);
  delay(2000);
  lcd.clear();
  EEPROM.get(lightLevelAddress, lightLevel);
  EEPROM.get(customChrIdxAddress, customChrIdx);
  EEPROM.get(customChrCursorAddress, customChrCursor);
  analogWrite(lcd_PWM, lightLevel);
  oldKey = getKey();
}

void loop()
{
  uint8_t key = getKey();
  if (key != oldKey)
  {
    if (oldKey == KEY_NONE)
    {
      soundTick();
      if (key == KEY_SELECT)
      {
        enter = true;
        goto getPassword;
      }
    }
    oldKey = key;
  }
  
  sensor();
  lcd.setCursor(0,1);
  lcd.print("MENU >> [SELECT]");
  
  getPassword:
  while(enter)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   Enter  PIN   ");
    while(enter)
    {
      lcd.setCursor(0,1);
      lcd.print("PIN :");
      lcd.setCursor(cursorPos, 1);
      lcd.cursor();
      lcd.blink();
      key = getAKey();
      switch (key) 
      {
        case KEY_RIGHT:
          cursorPos++;
          ctrPass++;
          numChr = 48;
          if (cursorPos == 12)
          {
            ctrPass = 0;
            cursorPos = 6;
          }
          lcd.setCursor(cursorPos, 1);
          lcd.noCursor();
          lcd.noBlink();
          break;
          
        case KEY_LEFT:
          if (ctrPass < 0)
          {
            ctrPass = 0;
          }
          cursorPos--;
          ctrPass--;
          numChr = 48;
          if (cursorPos < 6)
          {
            ctrPass = 0;
            cursorPos = 6;
          }
          lcd.setCursor(cursorPos, 1);
          lcd.noCursor();
          lcd.noBlink();
          break;
  
        case KEY_UP:
          if (numChr < 48)
          {
            numChr = 48;
          }
          if (numChr == 58)
          {
            numChr = 48;
          }
          newPassword[ctrPass] = numChr;
          lcd.setCursor(cursorPos,1);
          lcd.noCursor();
          lcd.noBlink();
          lcd.print(char(numChr));
          numChr++;
          break;
        
        case KEY_DOWN:
          if (numChr < 48)
          {
            numChr = 48;
          }
          numChr--;
          if (numChr == 47)
          {
            numChr = 32;
          }
          newPassword[ctrPass] = numChr;
          lcd.setCursor(cursorPos,1);
          lcd.noCursor();
          lcd.noBlink();
          lcd.print(char(numChr));
          break;
        
        case KEY_SELECT:
          lcd.setCursor(0,1);
          lcd.noCursor();
          lcd.noBlink();
          enter = false;
          break;
        
        case 255:
          return false;
      }
    }
    for(byte i = 0; i < 6; i++)
      password[i] = password[i];
      if(strcmp(newPassword, password) == 0)
      //if (newPassword == password)
      {
        lcd.clear();
        lcd.noCursor();
        lcd.noBlink();
        lcd.setCursor(0,0);
        lcd.print(F("  PIN Accepted  "));
        for (int x = 0; x < 3; x++)
        {
          PORTC |= bit(1);
          delay(50);
          PORTC &= ~bit(1);
          delay(50);
        }
        delay(2000);
        memset(newPassword, 0, sizeof newPassword);
        ctrPass = 0;
        cursorPos = 6;
        numChr = 47;
        enter = true;
        goto menu_1;
        //return false;
      }
      else
      {
        lcd.clear();
        lcd.noCursor();
        lcd.noBlink();
        lcd.setCursor(0,0);
        lcd.print(F("   WRONG  PIN   "));
        tone(A1, 1046);
        delay(200);
        tone(A1, 1318);
        delay(100);
        tone(A1, 1568, 200);
        delay(2000);
        cursorPos = 6;
        ctrPass = 0;
        numChr = 47;
        lcd.clear();
        enter = false;
      }
  }

  menu_1:
  while(enter)
  {
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(">Set Turbidity         "));
    lcd.setCursor(0,1);
    lcd.print(F(" Set Phone No          "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:

        break;
      
      case KEY_DOWN:
        goto menu_2;
        break;
      
      case KEY_SELECT:
        break;
      
      case 255:
        return false;
    }   
  }
  
  menu_2:
  while(enter)
  {
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(" Set Turbidity    "));
    lcd.setCursor(0,1);
    lcd.print(F(">Set Phone No     "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:
        goto menu_1;
        break;
      
      case KEY_DOWN:
        goto menu_3;
        break;
      
      case KEY_SELECT:
        break;
      
      case 255:
        return false;
    }
  }

  menu_3:
  while(enter)
  {
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(">Set Password       "));
    lcd.setCursor(0,1);
    lcd.print(F(" Set Brightness    "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:
        goto menu_2;
        break;
      
      case KEY_DOWN:
        goto menu_4;
        break;
      
      case KEY_SELECT:
        break;
      
      case 255:
        return false;
    }
  }

  menu_4:
  byte i;
  while(enter)
  {
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(" Set Password         "));
    lcd.setCursor(0,1);
    lcd.print(F(">Set Brightness    "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:
        goto menu_3;
        break;
      
      case KEY_DOWN:
        goto menu_5;
        break;
      
      case KEY_SELECT:
        ensureBrightnessPos();
        goto menu_Brightness;
        break;
      
      case 255:
        return false;
    }
  }

  menu_5:
  while(enter)
  {
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(" Set Brightness    "));
    lcd.setCursor(0,1);
    lcd.print(F(">Exit              "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:
        goto menu_6;
        break;
      
      case KEY_DOWN:
        break;
      
      case KEY_SELECT:
        //memset(newPassword, 0, sizeof newPassword);
        lcd.clear();
        delay(500);
        enter = false;
        chrIdx = true;
        break;
      
      case 255:
        return false;
    }
  }

  menu_6:
  while(enter)
  {
    byte i;
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F(">Set Brightness    "));
    lcd.setCursor(0,1);
    lcd.print(F(" Exit              "));
    delay(5);

    key = getAKey();
    switch (key) 
    {
      case KEY_UP:
        goto menu_3;
        break;
      
      case KEY_DOWN:
        goto menu_5;
        break;
      
      case KEY_SELECT:
        ensureBrightnessPos();
        goto menu_Brightness;
        break;
      
      case 255:
        return false;
    }
  }

  menu_Brightness:
  while(enter)
  {
    analogWrite(lcd_PWM, lightLevel);
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print(F("Brightness : "));
    lcd.print(lightLevel);
    lcd.print("  ");
    lcd.setCursor(0,1);
    lcd.print("Level : ");

    key = getAKey();
    switch (key) 
    {
      case KEY_RIGHT:
        if (customChrCursor < 16)
        {
          customChrCursor++;
        }
        lcd.setCursor(customChrCursor, 1);
        if (customChrIdx < 8)
        {
          customChrIdx++;
        }
        lcd.print(char(customChrIdx));
        lcd.print("   ");
        if (lightLevel < 255)
        {
          lightLevel += 35;
        }
        if (lightLevel > 255)
        {
          lightLevel = 255;
        }
        break;
        
      case KEY_LEFT:
        if (customChrCursor > 8)
        {
          customChrCursor--;
        }
        lcd.setCursor(customChrCursor, 1);
        if (customChrIdx > 0)
        {
          customChrIdx--;
        }
        lcd.print(char(customChrIdx));
        lcd.print("   ");
        if (lightLevel > 1)
        {
          lightLevel -= 35;
        }
        if (lightLevel < 35)
        {
          lightLevel = 35;
        }
        break;

      case KEY_UP:
        customChrCursor = 8;
        lightLevel = 255;
        for (customChrIdx = 0; customChrIdx < 8; customChrIdx++)
        {
          lcd.setCursor(customChrCursor + customChrIdx, 1);
          lcd.print(char(customChrIdx));
          lcd.print("   ");
        }
        customChrIdx = 7;
        customChrCursor = 15;
        break;
      
      case KEY_DOWN:
        customChrCursor = 8;
        customChrIdx = 0;
        lcd.setCursor(customChrCursor,1);
        lcd.print(char(customChrIdx));
        lcd.print("          ");
        lightLevel = 1;
        break;
      
      case KEY_SELECT:
        updateBrightness();
        goto menu_4;
        break;
      
      case 255:
        return false;
    }
  }
}

void updateBrightness()
{
  EEPROM.update(lightLevelAddress, lightLevel);
  EEPROM.update(customChrIdxAddress, customChrIdx);
  EEPROM.update(customChrCursorAddress, customChrCursor);  
}

void ensureBrightnessPos()
{
  lcd.clear();
  EEPROM.get(customChrCursorAddress, customChrCursor);
  customChrIdx = EEPROM.get(customChrIdxAddress, customChrIdx);
  for (i = 0; i < customChrIdx + 1; i++)
  {
    lcd.setCursor(8 + i, 1);
    lcd.print(char(i));
    lcd.print("   ");
  }
}

void checkBrightness()
{
  lcd.setCursor(customChrCursor,1);
  lcd.print(char(customChrIdx));
  lcd.print("     ");
}

uint8_t getAKey()
{
  uint8_t key, key0 = getKey();
  while (true)
  {
    key = getKey();
    if (key != key0)
    {
      if (key0 == KEY_NONE)
      {
        soundTick();
        return key;
      }
      key0 = key;
    }
  }
}

void soundTick()
{
  tone(A2, 4000, 25);
}
