#ifndef WIFI_UI_H
#define WIFI_UI_H
#include "BaseUI.h"

class WifiUI : public BaseUI {
  public:
    using BaseUI::BaseUI;

    void render(OTAData ota, bool blink) {
        d->setDrawColor(1);
        d->drawRFrame(0, 0, 128, 64, 4);
        
        // Header
        d->drawBox(0, 0, 128, 14);
        d->setDrawColor(0);
        d->setFont(u8g2_font_helvB08_tr);
        d->setCursor(35, 11); d->print("OTA UPDATE");
        d->setDrawColor(1);

        d->setFont(FONT_TINY);
        d->setCursor(10, 28); d->print("SSID: "); d->print(ota.ssid);
        d->setCursor(10, 38); d->print("IP:   "); d->print(ota.ip);

        // BAGIAN INI YANG DIRINGANKAN
        if (ota.isUpdating) {
            d->setFont(u8g2_font_helvB08_tr);
            d->setCursor(10, 52); 
            // Cukup tampilkan teks, gak usah bar
            if (blink) d->print("UPLOADING..."); 
            else d->print("DO NOT TURN OFF");
        } else {
            if (blink) {
                d->setCursor(10, 55); d->print("Open IP in Browser");
            }
        }
    }
};
#endif