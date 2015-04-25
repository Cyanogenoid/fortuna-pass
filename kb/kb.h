#ifndef KB_H
#define KB_H



#include "Keyboard.h"

void kb_init(void);
void send_text(char*);

void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData);
void ProcessLEDReport(const uint8_t LEDReport);
void SendNextReport(void);
void ReceiveNextReport(void);
void HID_Task(void);

#endif
