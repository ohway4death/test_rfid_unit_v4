// ---------------------------------------------------------------
//	RFID_RC522_CORE2.ino
//
//						Jun/8/2022
// ---------------------------------------------------------------
#define M5STACK_MPU6886

#include <M5Core2.h>
#include <driver/i2s.h>
#include "MFRC522_I2C.h"
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

#include <WiFi.h>

#define ace "1d f4 df 06"
#define jack "1d 75 01 55"
#define queen "1d fd 6e 58"
#define king "1d e1 27 55"

#define WAIT 1
#define SCAN 2
#define RESULT 3
#define TEST 4

static LGFX lcd; // LGFXのインスタンスを作成。

LGFX_Sprite spriteBase(&lcd); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。

LGFX_Sprite spriteImage(&spriteBase); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。

LGFX_Sprite spriteWord1(&spriteBase); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。
LGFX_Sprite spriteWord2(&spriteBase); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。
LGFX_Sprite spriteWord3(&spriteBase); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。
LGFX_Sprite spriteWord4(&spriteBase); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。

MFRC522_I2C mfrc522(0x28, -1); // Create MFRC522 instance.

void uid_display_proc();
void display_trump();
void wait();
void scan();
void result();
void zeroReset();
void check_acc();

// ---------------------------------------------------------------
void setup()
{
  M5.begin();
  Serial.begin(19200);          // シリアルポートを開き，通信速度を19200bpsに設定
  lcd.init();                   // LCD初期化
  spriteImage.setColorDepth(8); // カラーモード設定
  spriteBase.setColorDepth(8);  // カラーモード設定
  spriteWord1.setColorDepth(8); // カラーモード設定
  spriteWord2.setColorDepth(8); // カラーモード設定
  spriteWord3.setColorDepth(8); // カラーモード設定
  spriteWord4.setColorDepth(8); // カラーモード設定

  spriteImage.createSprite(320, 218);
  spriteBase.createSprite(320, 240);
  spriteWord1.createSprite(80, 80);
  spriteWord2.createSprite(80, 80);
  spriteWord3.createSprite(80, 80);
  spriteWord4.createSprite(80, 80);

  // spriteImage.drawJpgFile(SD, "/trump/card_spade_01.jpg", 0, 0);

  spriteWord1.setTextSize(6); // 文字サイズ42px
  spriteWord2.setTextSize(6); // 文字サイズ42px
  spriteWord3.setTextSize(6); // 文字サイズ42px
  spriteWord4.setTextSize(6); // 文字サイズ42px

  spriteWord1.setCursor(24, 24);
  spriteWord2.setCursor(24, 24);
  spriteWord3.setCursor(24, 24);
  spriteWord4.setCursor(24, 24);

  spriteWord1.printf("S");
  spriteWord2.printf("C");
  spriteWord3.printf("A");
  spriteWord4.printf("N");

  Wire.begin();
  mfrc522.PCD_Init();

  M5.IMU.Init();                     // 6軸センサ初期化
  M5.IMU.SetAccelFsr(M5.IMU.AFS_2G); // 加速度センサースケール初期値 ±2G(2,4,8,16)
  zeroReset();                       // 0リセット
}

// ---------------------------------------------------------------
uint16_t x = 0;
uint16_t y = 11;
float_t scaleX = 1;
float_t scaleY = 0.5;

int nocard = 0;
int state = WAIT;

void loop()
{

  if (state == WAIT)
  {
    wait();
  }
  else if (state == SCAN)
  {
    scan();
  }
  else if (state == RESULT || state == TEST)
  {
    result();
  }
  delay(200);
}

// ---------------------------------------------------------------
void display_trump()
{
  String strBuf[mfrc522.uid.size];
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    strBuf[i] = String(mfrc522.uid.uidByte[i], HEX);
    if (strBuf[i].length() == 1)
    {
      strBuf[i] = "0" + strBuf[i];
    }
  }

  String strUID = strBuf[0] + " " + strBuf[1] + " " + strBuf[2] + " " + strBuf[3];

  if (strUID.equalsIgnoreCase(ace))
  {
    spriteImage.drawJpgFile(SD, "/trump/card_spade_01.jpg", 0, 0);
    // spriteImage.pushSprite(&lcd, x, y);
  }
  else if (strUID.equalsIgnoreCase(jack))
  {
    spriteImage.drawJpgFile(SD, "/trump/card_spade_11.jpg", 0, 0);
    // spriteImage.pushSprite(&lcd, x, y);
  }
  else if (strUID.equalsIgnoreCase(queen))
  {
    spriteImage.drawJpgFile(SD, "/trump/card_spade_12.jpg", 0, 0);
    // spriteImage.pushSprite(&lcd, x, y);
  }
  else if (strUID.equalsIgnoreCase(king))
  {
    spriteImage.drawJpgFile(SD, "/trump/card_spade_13.jpg", 0, 0);
    // spriteImage.pushSprite(&lcd, x, y);
  }
  else
  {
    Serial.println(strUID);
  }

  for (float i = 0; i <= 360; i += 10)
  {
    spriteBase.fillScreen(BLACK); // 画面の塗りつぶし
    spriteImage.pushRotateZoom(160, 120, i, i / 360, i / 360);
    spriteBase.pushSprite(0, 0);
    delay(100);
  }
}
// ---------------------------------------------------------------

void wait()
{

  spriteBase.fillScreen(BLACK); // 画面の塗りつぶし
  spriteWord1.pushRotateZoom(280, 120, 90, 1, 1);
  spriteWord2.pushRotateZoom(200, 120, 90, 1, 1);
  spriteWord3.pushRotateZoom(120, 120, 90, 1, 1);
  spriteWord4.pushRotateZoom(40, 120, 90, 1, 1);
  spriteBase.pushSprite(&lcd, 0, 0);

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  else
  {
    state = SCAN;
  }
}

void scan()
{
  display_trump();
  state = RESULT;
}


float baseAccX, baseAccY, baseAccZ = 0; // 基準加速度格納用
float accX, accY, accZ;                 // 加速度格納用
float diffAccX, diffAccY, diffAccZ = 0;    // 現在値格納用

float maxAccX, maxAccY, maxAccZ = 0;

void result()
{
  M5.update();
  if (M5.BtnA.isPressed())
  {
    state = WAIT;
  }
  else if (M5.BtnC.isPressed())
  {
    state = TEST;
    zeroReset();
  }

  if (state == TEST){
    
    M5.IMU.getAccelData(&accX, &accY, &accZ); // 加速度データ取得

    diffAccX = accX - baseAccX; // 補正後の数値を表示値としてセット
    diffAccY = accY - baseAccY;
    diffAccZ = accZ - baseAccZ;

    if(maxAccX < diffAccX){ maxAccX = diffAccX; }
    if(maxAccY < diffAccY){ maxAccY = diffAccY; }
    if(maxAccZ < diffAccZ){ maxAccZ = diffAccZ; }

    spriteBase.fillScreen(BLACK);             // 画面の塗りつぶし

    spriteBase.setTextColor(ORANGE, BLACK); // X軸測定値
    spriteBase.setCursor(30, 24);
    spriteBase.printf("X:%7.3f ", accX);    // 現在値
    spriteBase.setCursor(140, 24);
    spriteBase.printf("X:%7.3f ", maxAccX); // 最大値

    spriteBase.setTextColor(CYAN, BLACK);   // Y軸測定値
    spriteBase.setCursor(30, 44);
    spriteBase.printf("Y:%7.3f ", accY);    // 現在値
    spriteBase.setCursor(140, 44);
    spriteBase.printf("Y:%7.3f ", maxAccY); // 最大値

    spriteBase.setTextColor(GREEN, BLACK);  // Z軸測定値
    spriteBase.setCursor(30, 64);
    spriteBase.printf("Z:%7.3f ", accZ);    // 現在値
    spriteBase.setCursor(140, 64);
    spriteBase.printf("Z:%7.3f ", maxAccZ); // 最大値    

    spriteBase.pushSprite(&lcd, 0, 0);
  }

  if (diffAccY > 0.5)
  {
    state = WAIT;
  }
  
}

void zeroReset()
{
  M5.IMU.getAccelData(&accX, &accY, &accZ); // 加速度データ取得
  baseAccX = accX;                               // 取得値を補正値としてセット
  baseAccY = accY;
  baseAccZ = accZ;
}

void check_acc()
{
  zeroReset();
  M5.update();                              // ボタン状態更新
  M5.IMU.getAccelData(&accX, &accY, &accZ); // 加速度データ取得
}