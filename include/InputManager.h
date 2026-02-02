#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Arduino.h>
#include <OneButton.h>
#include "Config.h"
#include "GlobalTypes.h"

class InputManager {
  private:
    OneButton* btn; 

    // Multi-click Vars
    int clickCount;
    unsigned long lastClickTime;
    const unsigned long CLICK_TIMEOUT = 400;

    // Manual Read Vars
    int lastSteadyState;
    int lastFlickerableState;
    unsigned long lastDebounceTime;
    
    // [BARU] Variabel untuk filter durasi tekan
    unsigned long btnPressTime; 

    // Output
    int finalCommand;
    bool jumpTriggered;

    // Mode reference
    SystemMode *modePtr;

    // Callbacks
    static void staticClick(void *ptr);
    static void staticLongPress(void *ptr);

    void onClick();
    void onLongPress();

  public:
    InputManager();

    void begin();
    void update();

    void bindMode(SystemMode *mode);

    int  getCommand();
    bool isGameJump();
};

#endif