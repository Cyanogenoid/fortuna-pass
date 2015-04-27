#ifndef KB_H
#define KB_H



#include "Keyboard.h"

typedef struct kb_input_struct {
    uint8_t code;
    bool shift;
} kb_input;



void kb_init(void);
void send_text(char*);

kb_input resolve_char(char);

void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData);
void ProcessLEDReport(const uint8_t LEDReport);
void SendNextReport(void);
void ReceiveNextReport(void);
int USB_Task(int);
int HID_Task(int);

#endif
