#include <Arduino.h>
#include <SPI.h>
#include "U8g2lib.h"
#include <cmath>

#define CLK_PIN 33
#define DATA_PIN 26
#define SW_PIN 14


TaskHandle_t DrawDisplay;


U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, 21, 17, 16); // 硬SPI
int x1, x2;
int currentStateCLK;
int lastStateCLK;
int location = 5; // 数字位置
bool dowEn = true;

[[noreturn]] void drawDisplay(void *pvParameters) {
    for (;;) {
        // 多线程
        currentStateCLK = digitalRead(CLK_PIN);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
            if (digitalRead(DATA_PIN) != currentStateCLK) {
                // 反向旋转
                Serial.println("反转");
                if (location >= 6) {
                    dowEn = true;
                    location = location - 1;
                }
            } else {
                // 正向旋转
                Serial.println("正转");
                if (location <= 122) {
                    dowEn = true;
                    location = location + 1;
                }
            }
        }
        lastStateCLK = currentStateCLK;
        int btnState = digitalRead(SW_PIN);
        if (btnState == LOW) {
            // 按下按钮事件
        }
        delay(1);
    }
}

void setup() {
    Serial.begin(115200);
    // u8g2.setBusClock(8000000);
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    pinMode(DATA_PIN, INPUT);
    pinMode(CLK_PIN, INPUT);
    xTaskCreatePinnedToCore(drawDisplay, "drawDisplay", 10000, nullptr, 1, &DrawDisplay, 0);
}

void loop() {
    if (dowEn) {
        u8g2.clearBuffer();
        for (int y = 0; y < 128; ++y) {
            x1 = round(10 * sin((0.025 * PI * y + PI / 2) - 0.35) + 10);
            u8g2.drawPixel(x1, y);
            x2 = round(10 * sin((0.025 * PI * y + 14.8 / PI) - 0.35) + 10);
            u8g2.drawPixel(x2, y);
            if (y == location) {
                u8g2.drawHLine(x1, y, abs((10 - x1) + 30));
                u8g2.drawUTF8(x1 + abs((10 - x1) + 30) + 3, y + 4, "hello");
            }
            if (y % 5 == 0) { // 间隔横线
                u8g2.drawLine(x1, y, x2, y);
            }
        }
        u8g2.sendBuffer();
        dowEn = false;
    }
    delay(1);
}