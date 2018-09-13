/* TODO:
    Load/Save settings from/to SD card - done
    Calculate angle from rainbow disc - done
    Create power on/off (need hardware modifications to control backlight) - done
    Create "sleep" mode - done
    Create communications protocol - done
    Create two separate settings for channels 1 and 2:
      - when switching between channels, update current screen settings to reflect proper values - done
    Create saturation setting - done
    Create brightness setting - done
    Create color setting - done
    Create lock functionality:
      - when locked, color dial will update both channel's colors - does not affect saturation or brightness - done
      - when locked, brightness will update both channel's brightness - does not affect color or saturation - done
      - when locked, saturation will update both channel's saturation - does not affect color or brightness - done
    Add check for SD card initialized failed - display red alert icon on display
    Create first boot sequence when no config file exists - sat 100, value 50, cycle 0-360 hue, sat 100 to 0 to 100, value 50 to 0 to 100 to 50
      - create 0xF0F0 command to let LED controller do first boot sequence
    Add option to control backlight brightness - not implementing since sleep mode exists
    Cleanup code and rename variables & functions more accordingly - done
    Put configuration into a structure and make only one read/write call when loading/saving - done
    Add current power state to configuration - done
    Add "cleaning" mode where panel does not respond to touches for 30 seconds
    Optimize code - so far so good - reduced 10% storage / 5% dynamic memory (currently 84% storage / 68% dynamic memory)
                  - before code minimization:
                    - Sketch uses 28874 bytes (89%) of program storage space. Maximum is 32256 bytes.
                    - Global variables use 1392 bytes (67%) of dynamic memory, leaving 656 bytes for local variables. Maximum is 2048 bytes.
                  - during code minimization:
                    - Sketch uses 25324 bytes (78%) of program storage space. Maximum is 32256 bytes.
                    - Global variables use 1359 bytes (66%) of dynamic memory, leaving 689 bytes for local variables. Maximum is 2048 bytes.
                  - switching to the Nano board, program storage space dropped 1536 bytes.
                  - during code minimization (nano):
                    - Sketch uses 24350 bytes (79%) of program storage space. Maximum is 30720 bytes.
                    - Global variables use 1345 bytes (65%) of dynamic memory, leaving 703 bytes for local variables. Maximum is 2048 bytes.
                  - code refactoring and moving around global variables
                    - Sketch uses 24542 bytes (79%) of program storage space. Maximum is 30720 bytes.
                    - Global variables use 1203 bytes (58%) of dynamic memory, leaving 845 bytes for local variables. Maximum is 2048 bytes.
                  - working final with all but startup color cycle, cleaning and exclamation todo's
                    - Sketch uses 24930 bytes (81%) of program storage space. Maximum is 30720 bytes.
                    - Global variables use 1204 bytes (58%) of dynamic memory, leaving 844 bytes for local variables. Maximum is 2048 bytes.
*/

/* BUGS:
    Width of the .rgb files is wrong.
    The gimp_bmp_to_drawrgb.c program has been fixed to put in the correct width.
    The .rgb files will need to be recreated.
    This is a minor problem since the width parameter isn't being used.
*/

/* NOTES:
    How to create the image resources:
      Base image is designed in Inkscape as an SVG.
      Keep the "Center Dial" layer hidden - may want to delete it in the future.
      Export PNG Image and save as ColorWheel_Menu.png.
      Use Gimp to mask off the various elements of the image.
      Copy/paste masked item to new image.
      Export image as .bmp with advanced options and selecting 24 bits (R8 G8 B8) (this may be skipped - but I like to keep an image handy that can be viewed easily).
      Export image as .c with prefixed name: gimp_image, and check only Save as RGB565 (16-bit).
      Open gimp_bmp_to_drawrgb.c and paste the exported .c file to the top of the file (overwrite the existing gimp_image structure.
      Rename the *.rgb file in the source code to match the exported image .c file name.
      Compile the code: cc gimp_bmp_to_drawrgb.c -o gimp_bmp_to_drawrgb
      Run the program: ./gimp_bmp_to_drawrgb
      The .rgb file create is now ready for reading - copy it into the appropriate directory
*/

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_SPITFT.h"
#include <Adafruit_FT6206.h>
#include <SPI.h>
#include <SD.h>
#include "TouchScreen.h"

bool isTouched;
bool isDirty;
bool isIdle;
bool isPowerPressed;
uint16_t curDegree;
unsigned long timeCheckDirty;
unsigned long timeCheckIdle;
unsigned long timeCheckPower;

AppSetting settings;
TS_Point touchPoint;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_FT6206 ts = Adafruit_FT6206();

/*
   main entry point - perform all setup code here
*/
void setup(void)
{
  Serial.begin(9600);

  initialize();

  tft.begin();
  tft.setRotation(1);
  analogWrite(BK_LT, 0);

  yield();

  SD.begin(SD_CS);
  ts.begin(32);

  readConfig();
  rgbDraw(MENU_BASE, 0, 0);
  setLockUnlock();
  setDisplayValues();
  if(settings.hasPower)
  {
    sendColors();
    fadePanelOnOff(ON);
  }
  else
  {
    HsvColor color = { 0, 0, 0 };
    sendColors(color, color);
  }
}


/*
   set program default values
*/
void initialize()
{
  isTouched = false;
  isDirty = false;
  isIdle = false;
  isPowerPressed = false;
  curDegree = -1;
  timeCheckDirty = 0;
  timeCheckPower = 0;
  timeCheckIdle = millis();
  touchPoint.x = 0;
  touchPoint.y = 0;

  settings.is1 = true;
  settings.is2 = false;
  settings.isLock = false;
  settings.hasPower = true;
  settings.curChannel = 1;
  settings.hsvColor1 = { 0, 100, 50 };
  settings.hsvColor2 = { 0, 100, 50 };
}


/*
   main program loop
*/
void loop()
{
  // check if panel has been touched
  if (ts.touched())
  {
    TS_Point p = ts.getPoint();
    touchPoint.x = map(p.y, 0, 320, 320, 0);
    touchPoint.y = p.x;

    // normal behavior is to set color and menu values
    if (settings.hasPower)
    {
      if(!isIdle)
      {
        if (isTouched)
        {
          getColors();
        }
        setPanelItems();

        if(isPowerPressed)
        {
          if(millis() - timeCheckPower > POWEROFFCHECK)
          {
            settings.hasPower = false;
            HsvColor color = { 0, 0, 0 };
            sendColors(color, color);
            fadePanelOnOff(OFF);
            timeCheckPower = millis();
            saveConfig();
            isDirty = false;
          }
        }
      }
    }
    else
    {
      if(!isTouched)
      {
        timeCheckPower = millis();
      }
    }

    isTouched = true;
  }
  else
  {
    isTouched = false;

    // untoggle the power indicator
    if(isPowerPressed)
    {
      isPowerPressed = false;
      setPowerIndicator();
    }
  }

  // check if panel is on or off
  if (settings.hasPower)
  {
    // check if settings are dirty (currently set to auto-save after 5 seconds of a change)
    saveIfDirty();

    // check and set panel idle status
    setIdleStatus();
  }
  else if(isTouched)
  {
    if(millis() - timeCheckPower > POWEROFFCHECK)
    {
      settings.hasPower = true;
      sendColors();
      setIdleStatus();
      fadePanelOnOff(ON);
      saveConfig();
    }
  }
}


/*
   determine panel idle status

   if the panel has not been touched for IDLECHEK milliseconds then fade the screen
   to black (turn off the backlight)

   if the panel is idle and the panel has been touched, restore the backlight brightness
   and pause for half a second to reduce the chance of changing a setting
*/
void setIdleStatus()
{
  if (!isTouched && !isIdle && millis() - timeCheckIdle > IDLECHECK)
  {
    fadePanelOnOff(OFF);
    isIdle = true;
  }
  else if (isTouched)
  {
    timeCheckIdle = millis();
    if (isIdle)
    {
      fadePanelOnOff(ON);
      isIdle = false;
    }
  }
}


/* fade panel on/off */
void fadePanelOnOff(bool isOn)
{
  if(isOn)
  {
      // restore backlight
      for (int index = 0; index <= BACKLIGHT; index++)
      {
        analogWrite(BK_LT, index);
        delay(1);
      }
      delay(500);
  }
  else
  {
    // fade to black
    for (int index = BACKLIGHT; index >= 0; index--)
    {
      analogWrite(BK_LT, index);
      delay(2);
    }
  }
}

/*
   if any settings have changed wait for DIRTYCHECK milliseconds to expire
   before saving - this will reduce spamming the SD card with excessive writes
*/
void saveIfDirty()
{
  if (millis() - timeCheckDirty > DIRTYCHECK)
  {
    timeCheckDirty = millis();
    if (isDirty)
    {
      saveConfig();
    }
    isDirty = false;
  }
}


/*
   get new color, saturation and value (brightness)
*/
void getColors()
{
  bool hsvChanged = false;
  uint16_t degree = getValidDegree(touchPoint.x, touchPoint.y);

  if (degree != -1)
  {
    // valid color
    if (curDegree != degree)
    {
      hsvChanged = true;
      drawSelectorKnob(degree);
      if (settings.is1)
      {
        settings.hsvColor1.hue = degree;
      }
      if (settings.is2)
      {
        settings.hsvColor2.hue = degree;
      }
    }
  }

  if (touchPoint.x >= (SLIDERMINX - 10) && touchPoint.x <= (SLIDERMAXX + 10))
  {
    // set over/under scan touch points to max/min slider values
    if (touchPoint.x < SLIDERMINX)
    {
      touchPoint.x = SLIDERMINX;
    } else if (touchPoint.x > SLIDERMAXX)
    {
      touchPoint.x = SLIDERMAXX;
    }

    if (touchPoint.y > SLIDERSATMINY && touchPoint.y < SLIDERSATMAXY)
    {
      // saturation slider
      hsvChanged = true;
      drawSliderButton(touchPoint.x, 188, 1);
      if (settings.is1)
      {
        settings.hsvColor1.saturation = map(touchPoint.x, SLIDERMINX, SLIDERMAXX, 100, 0);
      }
      if (settings.is2)
      {
        settings.hsvColor2.saturation = map(touchPoint.x, SLIDERMINX, SLIDERMAXX, 100, 0);
      }
    }
    else if (touchPoint.y > SLIDERVALMINY && touchPoint.y < SLIDERVALMAXY)
    {
      // value (brightness) slider
      hsvChanged = true;
      drawSliderButton(touchPoint.x, 212, 2);
      if (settings.is1)
      {
        settings.hsvColor1.value = map(touchPoint.x, SLIDERMINX, SLIDERMAXX, 0, 100);
      }
      if (settings.is2)
      {
        settings.hsvColor2.value = map(touchPoint.x, SLIDERMINX, SLIDERMAXX, 0, 100);
      }
    }
  }

  if (hsvChanged)
  {
    isDirty = true;
    sendColors();
  }
}


/*
   set the channel and lock indicators
*/
void setPanelItems()
{
  if (!isTouched)
  {
    // set lock/unlock
    if (touchPoint.x > 259 && touchPoint.x < 289 && touchPoint.y > 80 && touchPoint.y < 109)
    {
      settings.isLock = !settings.isLock;
      setLockUnlock();
    }

    // set channel 1 or 2 or power
    if (!settings.isLock && touchPoint.x > 252 && touchPoint.x < 297)
    {
      if (!settings.is1 && touchPoint.y > 35 && touchPoint.y < 68)
      {
        settings.curChannel = 1;
        setDisplayValues();
      } else if (!settings.is2 && touchPoint.y > 120 && touchPoint.y < 155)
      {
        settings.curChannel = 2;
        setDisplayValues();
      }
    }

    // make power indicator active
    if(touchPoint.x > 254 && touchPoint.x < 295)
    {
      if (!isPowerPressed && touchPoint.y > 188 && touchPoint.y < 224)
      {
        isPowerPressed = true;
        timeCheckPower = millis();
        setPowerIndicator();
      }
    }
  }
}


/*
   draw color, saturation and value (brightness) indicators
*/
void setDisplayValues()
{
  HsvColor hsvColor;

  // set current color based on selected channel
  if (settings.curChannel == 1)
  {
    memcpy(&hsvColor, &settings.hsvColor1, sizeof(HsvColor));
  }
  else
  {
    memcpy(&hsvColor, &settings.hsvColor2, sizeof(HsvColor));
  }

  // set slider values for the saturation and value (brightness)
  uint16_t satx = map(hsvColor.saturation, 100, 0, SLIDERMINX, SLIDERMAXX);
  uint16_t valx = map(hsvColor.value, 0, 100, SLIDERMINX, SLIDERMAXX);

  // draw visual elements
  drawSliderButton(satx, 188, 1);
  drawSliderButton(valx, 212, 2);
  drawSelectorKnob(hsvColor.hue);
  setLockUnlock();
}


/*
   draw the lock and channel indicators
*/
void setLockUnlock()
{
  if (settings.isLock)
  {
    settings.is1 = true;
    settings.is2 = true;
    rgbDraw(MENU_LOCK, 251, 78);
    rgbDraw(MENU_ACTIVE1, 243, 32);
    rgbDraw(MENU_ACTIVE2, 243, 118);
  }
  else
  {
    if (settings.curChannel == 1)
    {
      settings.is1 = true;
      settings.is2 = false;
      rgbDraw(MENU_ACTIVE1, 243, 32);
      rgbDraw(MENU_INACTIVE2, 243, 118);
      rgbDraw(MENU_UNLOCK, 251, 78);
    }
    else
    {
      settings.is1 = false;
      settings.is2 = true;
      rgbDraw(MENU_ACTIVE2, 243, 118);
      rgbDraw(MENU_INACTIVE1, 243, 32);
      rgbDraw(MENU_UNLOCK, 251, 78);
    }
  }
}


/*
   draw the power indicator
*/
void setPowerIndicator()
{
  if (isPowerPressed)
  {
    rgbDraw(MENU_POWERACTIVE, 245, 186);
  }
  else
  {
    rgbDraw(MENU_POWER, 245, 186);
  }
}


/*
   draw the slider button indicator
*/
void drawSliderButton(uint16_t x, uint16_t y, byte slider)
{
  if (slider == 1)
  {
    rgbDraw(MENU_SATURATION, 14, 186);
  }
  else
  {
    rgbDraw(MENU_BRIGHTNESS, 14, 211);
  }

  // y is the center point
  int offset = 0;
  x = x - 8;
  for (const BitmapInfo &bmpInfo : sliderBitmapIdx)
  {
    BitmapInfo bitmapInfo;
    memcpy_P(&bitmapInfo, &bmpInfo, sizeof(BitmapInfo));
    drawRGBBitmap_P(x, y, sliderBitmap, offset, bitmapInfo);
    offset += bitmapInfo.datasize;
  }
}


/*
   draw the color selection indicator
*/
void drawSelectorKnob(uint16_t degree)
{
  /*  x:119 y:59 center/top position
      radius: 26 from 119,95 center point
      knob height x width: 22x22
      background color: r:34 g:39 b:46
  */
  double angle = (double)degree * (PI / 180.0);
  double prevAngle = (double)curDegree * (PI / 180.0);
  int dx = (int)(26.0 * sin(angle));
  int dy = (int)(26.0 * cos(angle));
  int px = (int)(26.0 * sin(prevAngle));
  int py = (int)(26.0 * cos(prevAngle));
  int offset = 0;

  int x = 119 - dx - 11;
  int y = 95 - dy;
  int ex = 119 - px - 11;
  int ey = 95 - py;

  // erase the selection indicator
  if (curDegree != -1)
  {
    tft.fillCircle(ex + 10, ey, 12, KNOBBACKGROUND);
  }

  // draw new position of the selection indicator
  for (const BitmapInfo &bmpInfo : knobBitmapIdx)
  {
    BitmapInfo bitmapInfo;
    memcpy_P(&bitmapInfo, &bmpInfo, sizeof(BitmapInfo));
    drawRGBBitmap_P(x, y, knobBitmap, offset, bitmapInfo);
    offset += bitmapInfo.datasize;
  }

  curDegree = degree;
}


/*
   calculate the current degree between 0 and 360
*/
uint16_t getValidDegree(int16_t x, int16_t y)
{
  int16_t degree;
  double rad;
  double dx = COLORCENTER_X - x;
  double dy = COLORCENTER_Y - y;

  int16_t radius = (int16_t)sqrt((dx * dx) + (dy * dy));

  if (radius < 81 && radius > 19)
  {
    rad = atan2(dx, dy);
    degree = (int16_t)(rad * (180.0 / PI));
    if (degree < 0)
    {
      degree += 360;
    }
    return degree;
  }
  else
  {
    return -1;
  }
}


/*
   transmit the color structure for both channels
*/
void sendColors()
{
  sendColors(settings.hsvColor1, settings.hsvColor2);
}

void sendColors(HsvColor color1, HsvColor color2)
{
  byte bufSize = sizeof(HsvColor);
  byte buf1[bufSize];
  byte buf2[bufSize];
  memcpy(buf1, &color1, bufSize);
  memcpy(buf2, &color2, bufSize);
  Serial.write(0xF1);
  Serial.write(0xF1);
  for (int index = 0; index < bufSize; index++)
  {
    Serial.write(buf1[index]);
  }
  for (int index = 0; index < bufSize; index++)
  {
    Serial.write(buf2[index]);
  }
  Serial.write(0xF4);
  Serial.write(0xF4);
}


/*
   save the current settings to a config file on the SD card
*/
void saveConfig()
{
  File configFile;

  if ((configFile = SD.open("menu1/config.dat", O_WRITE | O_CREAT | O_TRUNC)) == 0) {
    return;
  }

  configFile.write((const uint8_t *)&settings, sizeof(AppSetting));
  configFile.close();
}


/*
   load the current settings from a config file on the SD card
*/
void readConfig()
{
  File configFile;

  if ((configFile = SD.open("menu1/config.dat")) == 0) {
    return;
  }

  configFile.read((uint8_t *)&settings, sizeof(AppSetting));
  configFile.close();
}


/**********************************************************
   optimized code that was removed from existing libraries
 **********************************************************/

void drawRGBBitmap(int16_t x, int16_t y, uint16_t *colors565, int16_t width, int16_t height)
{
  tft.startWrite();
  tft.setAddrWindow(x, y, width, height); // Clipped area
  while (height--)
  {
    tft.writePixels(colors565, width);
    colors565 += width;
  }
  tft.endWrite();
}


void drawRGBBitmap_P(uint16_t x, uint16_t y, const uint16_t *bitmap, uint16_t offset, BitmapInfo bitmapInfo)
{
  uint16_t width = bitmapInfo.datasize / bitmapInfo.rows;
  uint16_t x1 = x + bitmapInfo.xoffset;
  uint16_t y1 = y + bitmapInfo.yoffset;
  tft.startWrite();
  for (int16_t yrow = 0; yrow < bitmapInfo.rows; yrow++)
  {
    for (int16_t xcol = 0; xcol < width; xcol++ )
    {
      tft.writePixel(x1 + xcol, y1 + yrow, pgm_read_word_near(&bitmap[offset + (yrow * width + xcol)]));
    }
  }
  tft.endWrite();
}


void rgbDraw(uint8_t fileIdx, uint16_t x, uint16_t y)
{
  File rgbFile;
  uint16_t width;
  uint16_t height;
  uint16_t offset;
  uint16_t pixels;
  uint16_t data[RGBBUFFER];
  uint16_t idx;
  uint16_t xoff;
  uint16_t dataRead;
  uint16_t lines;
  char filename[19];

  strcpy_P(filename, (char*)pgm_read_word(&(fileName_table[fileIdx])));

  if ((rgbFile = SD.open(filename)) == 0) {
    return;
  }

  rgbFile.read(&width, 2);
  rgbFile.read(&height, 2);

  for (lines = 0; lines < height; lines++)
  {
    idx = 0;
    rgbFile.read(&xoff, 2);
    rgbFile.read(&pixels, 2);
    while (idx < pixels)
    {
      if (pixels - idx > RGBBUFFER)
      {
        dataRead = RGBBUFFER;
      }
      else
      {
        dataRead = pixels - idx;
      }
      rgbFile.read(data, sizeof(data[0]) * dataRead);

      drawRGBBitmap(x + xoff, y + lines, data, dataRead, 1);
      idx += dataRead;
      xoff += dataRead;
    }
  }

  rgbFile.close();
}
