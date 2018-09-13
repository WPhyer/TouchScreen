#define USE_FAST_PINIO
#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 4
#define BK_LT 5

#define COLORCENTER_X 119
#define COLORCENTER_Y 95
#define MAXRADIUS 81
#define MINRADIUS 19
#define KNOBBACKGROUND 0x2125 //tft.color565(34, 39, 46)
#define RGBBUFFER 100
#define DIRTYCHECK 5000 // settings saved to be done in 5 second intervals
#define IDLECHECK 15000 // display idle fade after 15 seconds of inactivity
#define POWEROFFCHECK 1000 // turn off power after pressing power indicator for 1 second
#define POWERONCHECK 2000 // turn on power after pressing panel for 2 seconds
#define BACKLIGHT 255 // 255 max bright / 0 off
#define SLIDERMINX 22
#define SLIDERMAXX 217
#define SLIDERSATMINY 186
#define SLIDERSATMAXY 206
#define SLIDERVALMINY 211
#define SLIDERVALMAXY 231

#define SLIDERBITMAPSIZE 13
#define KNOBBITMAPSIZE 15

#define ON true
#define OFF false

#define MENU_CONFIG      0
#define MENU_BASE        1
#define MENU_ACTIVE1     2
#define MENU_INACTIVE1   3
#define MENU_ACTIVE2     4
#define MENU_INACTIVE2   5
#define MENU_LOCK        6
#define MENU_UNLOCK      7
#define MENU_SATURATION  8 
#define MENU_BRIGHTNESS  9
#define MENU_POWER       10
#define MENU_POWERACTIVE 11

typedef struct
{
  uint16_t hue;
  byte saturation;
  byte value;
} HsvColor;

typedef struct
{
  char xoffset;
  char yoffset;
  byte datasize;
  char rows;
} BitmapInfo;

typedef struct
{
  bool hasPower;
  byte curChannel;
  bool isLock;
  bool is1;
  bool is2;
  HsvColor hsvColor1;
  HsvColor hsvColor2;
} AppSetting;

const char PROGMEM fileName_01[] = "menu1/config.dat";
const char PROGMEM fileName_02[] = "menu1/base.rgb";
const char PROGMEM fileName_03[] = "menu1/active_1.rgb";
const char PROGMEM fileName_04[] = "menu1/inact_1.rgb";
const char PROGMEM fileName_05[] = "menu1/active_2.rgb";
const char PROGMEM fileName_06[] = "menu1/inact_2.rgb";
const char PROGMEM fileName_07[] = "menu1/lock.rgb";
const char PROGMEM fileName_08[] = "menu1/unlock.rgb";
const char PROGMEM fileName_09[] = "menu1/sat.rgb";
const char PROGMEM fileName_10[] = "menu1/bright.rgb";
const char PROGMEM fileName_11[] = "menu1/power.rgb";
const char PROGMEM fileName_12[] = "menu1/poweract.rgb";

/* filename string table */
const PROGMEM char* const fileName_table[] = {
  fileName_01,
  fileName_02,
  fileName_03,
  fileName_04,
  fileName_05,
  fileName_06,
  fileName_07,
  fileName_08,
  fileName_09,
  fileName_10,
  fileName_11,
  fileName_12
};

/* slider button for saturation and brightness */
const PROGMEM BitmapInfo sliderBitmapIdx[SLIDERBITMAPSIZE] = {
  { 6, 0, 4, 1 },
  { 4, 1, 8, 1 },
  { 3, 2, 10, 1 },
  { 2, 3, 12, 1 },
  { 1, 4, 14, 1 },
  { 1, 5, 15, 1 },
  { 0, 6, 96, 6 },
  { 1, 11, 15, 1 },
  { 1, 12, 14, 1 },
  { 1, 13, 13, 1 },
  { 1, 14, 12, 1 },
  { 2, 15, 11, 1 },
  { 2, 16, 10, 1 }
};
const PROGMEM uint16_t sliderBitmap[234] = {
  0x634e, 0x4aac, 0x4aac, 0x634e,
  0x52ec, 0x4a6a, 0x31a7, 0x2987, 0x2987, 0x31a7, 0x4a6a, 0x52ec,
  0x52ab, 0x2987, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2987, 0x52ab,
  0x4a8b, 0x2966, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2966, 0x4a8b,
  0x4a8b, 0x2967, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2967, 0x4229,
  0x2987, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2987, 0x52cb,
  0x52cb, 0x2946, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2946, 0x31c8,
  0x2987, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2125, 0x2987,
  0x1925, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1925,
  0x2966, 0x1904, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1905, 0x1904, 0x2986,
  0x31a7, 0x18e4, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x1904, 0x18e4, 0x2125,
  0x18e4, 0x10c4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x10c4, 0x2966,
  0x2125, 0x10c3, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x18e4, 0x10c3, 0x1904, 0x2987,
  0x2986, 0x10c4, 0x10a3, 0x10c4, 0x18c4, 0x18c4, 0x18c4, 0x18c4, 0x18c4, 0x18c4, 0x10c4, 0x10a3, 0x18c4, 0x2166,
  0x2987, 0x2966, 0x18e4, 0x1083, 0x10a3, 0x10c3, 0x10c4, 0x10c4, 0x10c3, 0x10a3, 0x1083, 0x18c4, 0x2146,
  0x31a7, 0x2987, 0x2966, 0x2125, 0x18e4, 0x10a3, 0x0882, 0x1082, 0x10a3, 0x10c4, 0x2125, 0x2166,
  0x2987, 0x2987, 0x2966, 0x2146, 0x2146, 0x2146, 0x2146, 0x2146, 0x2166, 0x2987, 0x2987,
  0x31e8, 0x31c8, 0x31a7, 0x2987, 0x2987, 0x2966, 0x2987, 0x2987, 0x2987, 0x29a7
};

/* selector knob for colors */
const PROGMEM BitmapInfo knobBitmapIdx[KNOBBITMAPSIZE] = {
  { 7, -11, 8, 1 },
  { 5, -10, 12, 1 },
  { 4, -9, 15, 1 },
  { 3, -8, 17, 1 },
  { 2, -7, 18, 1 },
  { 1, -6, 20, 1 },
  { 1, -5, 21, 1 },
  { 0, -4, 176, 8 },
  { 1, 4, 21, 1 },
  { 1, 5, 19, 1 },
  { 3, 6, 18, 1 },
  { 3, 7, 17, 1 },
  { 5, 8, 14, 1 },
  { 5, 9, 13, 1 },
  { 7, 10, 9, 1 }
};
const PROGMEM uint16_t knobBitmap[398] = {
  0x1905, 0x2125, 0x3a08, 0x31a7, 0x31a7, 0x4229, 0x1904, 0x1904,
  0x1904, 0x2966, 0x4a6a, 0x6b4d, 0x7bef, 0x8c51, 0x8c71, 0x9492, 0x8431, 0x630c, 0x3a08, 0x1904,
  0x2105, 0x52ab, 0x8430, 0x9cd3, 0x9cd3, 0x94b2, 0x9cd3, 0x9cf3, 0x9cf3, 0xa514, 0xad55, 0xa534, 0x738e, 0x2966, 0x1925,
  0x2145, 0x738e, 0xa534, 0xad75, 0xa534, 0x9cd3, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xa534, 0xb5b6, 0x9492, 0x39e8, 0x1925,
  0x18e4, 0x73cf, 0xbdd7, 0xbdf7, 0xb596, 0xad55, 0xa514, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xad75, 0xc618, 0x9cd3, 0x31a7,
  0x1904, 0x630c, 0xbdf7, 0xce79, 0xc618, 0xbdd7, 0xad75, 0xa534, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x9cd3, 0xa534, 0xbdd7, 0xc638, 0x8c51, 0x18e4,
  0x2986, 0xad75, 0xdedb, 0xd6ba, 0xce79, 0xc618, 0xbdd7, 0xad55, 0x94b2, 0x9cd3, 0x9cf3, 0xa514, 0x94b2, 0x94b2, 0xa534, 0xb596, 0xb5b6, 0xc638, 0xc638, 0x52ab, 0x1925,
  0x1905, 0x632d, 0xd6ba, 0xdefb, 0xdefb, 0xd6ba, 0xce79, 0xbdf7, 0xb596, 0xa514, 0x9cf3, 0xb596, 0xb596, 0xad55, 0xb596, 0xad55, 0xbdd7, 0xbdd7, 0xc618, 0xdedb, 0x9492, 0x1904,
  0x2966, 0x8c72, 0xdedb, 0xdefb, 0xe73c, 0xe73c, 0xdedb, 0xd69a, 0xbdf7, 0xad55, 0x94b2, 0xbdf7, 0xd69a, 0xd69a, 0xc618, 0xbdf7, 0xc618, 0xce59, 0xd69a, 0xdedb, 0xc618, 0x6b6e,
  0x4208, 0xa514, 0xd69a, 0xd69a, 0xd6ba, 0xdedb, 0xdedb, 0xdedb, 0xe73c, 0xce59, 0xce59, 0xdedb, 0xe71c, 0xe73c, 0xd6ba, 0xce79, 0xd69a, 0xd6ba, 0xdedb, 0xe71c, 0xce79, 0x632c,
  0x31a7, 0xa534, 0xc638, 0xc638, 0xce79, 0xc638, 0xd69a, 0xd69a, 0xd69a, 0xe71c, 0xe73c, 0xe71c, 0xe71c, 0xe73c, 0xe71c, 0xdefb, 0xdedb, 0xe73c, 0xe73c, 0xef7d, 0xdedb, 0x630c,
  0x31c7, 0x9cf3, 0xbdf7, 0xbdf7, 0xbdf7, 0xbdf7, 0xbdf7, 0xbdf7, 0xbdf7, 0xd6ba, 0xe71c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xe73c, 0xef5d, 0xef5d, 0xf79e, 0xe71c, 0x632d,
  0x4208, 0x9492, 0xb596, 0xad75, 0xad75, 0xad75, 0xad55, 0xa514, 0x94b2, 0xad75, 0xc618, 0xce59, 0xf7be, 0xf79e, 0xef7d, 0xdefb, 0xdedb, 0xe73c, 0xef7d, 0xf7be, 0xdefb, 0x632d,
  0x2145, 0x7bcf, 0xad55, 0xa534, 0xa534, 0xa514, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xbdd7, 0xdefb, 0xef7d, 0xe73c, 0xdefb, 0xdefb, 0xe73c, 0xef7d, 0xf7be, 0xce79, 0x4249,
  0x1905, 0x528a, 0x9cd3, 0x9cd3, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xad75, 0xbdf7, 0xdefb, 0xdefb, 0xdedb, 0xdefb, 0xef5d, 0xf79e, 0xf79e, 0xa534, 0x18e4,
  0x2986, 0x7bcf, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xad55, 0xbdd7, 0xce79, 0xdedb, 0xdefb, 0xe73c, 0xf79e, 0xf79e, 0xdedb, 0x632c, 0x1905,
  0x1904, 0x528a, 0x8c71, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x9cf3, 0xa534, 0xb5b6, 0xc618, 0xd6ba, 0xe71c, 0xef7d, 0xef7d, 0xef5d, 0xa534,
  0x632c, 0x9492, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0x9cd3, 0xad55, 0xbdd7, 0xc638, 0xd6ba, 0xe71c, 0xef7d, 0xef5d, 0xb5b6, 0x4228, 0x1925,
  0x2966, 0x6b4d, 0x8c71, 0x94b2, 0x94b2, 0x94b2, 0x94b2, 0xa514, 0xad55, 0xb5b6, 0xc638, 0xd69a, 0xdedb, 0xe71c, 0xb5b6, 0x528a, 0x1905,
  0x5acb, 0x7bef, 0x94b2, 0x94b2, 0x94b2, 0xa514, 0xad55, 0xb5b6, 0xc618, 0xce59, 0xbdf7, 0x94b2, 0x3a08, 0x1905,
  0x1904, 0x31c7, 0x5aeb, 0x7bcf, 0x8430, 0x9492, 0x9cf3, 0xa514, 0x9cf3, 0x8430, 0x52ab, 0x2125, 0x1925,
  0x1904, 0x2125, 0x4a6a, 0x4229, 0x4249, 0x52aa, 0x2986, 0x1904, 0x1905
};

void setIdleStatus();
void fadePanelOnOff(bool isOn);
void saveIfDirty();
void getColors();
void setPanelItems();
void setDisplayValues();
void setLockUnlock();
void setPowerIndicator();
void drawSliderButton(uint16_t x, uint16_t y, byte slider);
void drawSelectorKnob(uint16_t degree);
uint16_t getValidDegree(int16_t x, int16_t y);
void sendColors();
void sendColors(HsvColor color1, HsvColor color2);
void saveConfig();
void readConfig();
void drawRGBBitmap(int16_t x, int16_t y, uint16_t *colors565, int16_t width, int16_t height);
void drawRGBBitmap_P(uint16_t x, uint16_t y, uint16_t *bitmap, uint16_t offset, BitmapInfo bitmapInfo);
void rgbDraw(uint8_t fileIdx, uint16_t x, uint16_t y);
