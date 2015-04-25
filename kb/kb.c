#include "kb.h"

#include "os.h"



static volatile char* to_send = "";

void kb_init(void) {
    USB_Init();

    os_add_task( HID_Task,            10, 1);
    os_add_task( USB_USBTask,         10, 1);
}

void send_text(char* s) {
    to_send = s;
}

/* Translates a character into its corresponding scancode.
Supported range: All printable ASCII characters. [32, 127), [0x20, 0x7F)
*/
kb_input resolve_char(char c) {
    kb_input kb = {
        .code = 0x00,
        .shift = false,
    };

    if ('A' <= c && c <= 'Z') {
        kb.shift = true;
        c = c - 'A' + 'a'; // treat the same as lowercase
    }

    if ('a' <= c && c <= 'z') {
        kb.code = c - 'a' + HID_KEYBOARD_SC_A;
    } else if ('0' <= c && c <= '9') {
        if (c == '0') {
            c += 10;
        }
        kb.code = c - '1' + HID_KEYBOARD_SC_1_AND_EXCLAMATION;
    } else if (' ' <= c && c <= '/') {
        uint8_t us_map[] = {
        /*   */ HID_KEYBOARD_SC_SPACE,
        /* ! */ HID_KEYBOARD_SC_1_AND_EXCLAMATION,
        /* " */ HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE,
        /* # */ HID_KEYBOARD_SC_3_AND_HASHMARK,
        /* $ */ HID_KEYBOARD_SC_4_AND_DOLLAR,
        /* % */ HID_KEYBOARD_SC_5_AND_PERCENTAGE,
        /* & */ HID_KEYBOARD_SC_7_AND_AMPERSAND,
        /* ' */ HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE,
        /* ( */ HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS,
        /* ) */ HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS,
        /* * */ HID_KEYBOARD_SC_8_AND_ASTERISK,
        /* + */ HID_KEYBOARD_SC_EQUAL_AND_PLUS,
        /* , */ HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN,
        /* - */ HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
        /* . */ HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN,
        /* / */ HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK,
        };
        kb.code = us_map[c - ' '];
        kb.shift = !(c == '\''
                  || c == ','
                  || c == '-'
                  || c == '.'
                  || c == '/');
    } else if (':' <= c && c <= '@') {
        uint8_t us_map[] = {
        /* : */ HID_KEYBOARD_SC_SEMICOLON_AND_COLON,
        /* ; */ HID_KEYBOARD_SC_SEMICOLON_AND_COLON,
        /* < */ HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN,
        /* = */ HID_KEYBOARD_SC_EQUAL_AND_PLUS,
        /* > */ HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN,
        /* ? */ HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK,
        /* @ */ HID_KEYBOARD_SC_2_AND_AT,
        };
        kb.code = us_map[c - ':'];
        kb.shift = !(c == ';'
                  || c == '=');
    } else if ('[' <= c && c <= '`') {
        uint8_t us_map[] = {
        /* [ */ HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE,
        /* \ */ HID_KEYBOARD_SC_BACKSLASH_AND_PIPE,
        /* ] */ HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE,
        /* ^ */ HID_KEYBOARD_SC_6_AND_CARET,
        /* _ */ HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
        /* ` */ HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE,
        };
        kb.code = us_map[c - '['];
        kb.shift = c == '_'
                || c == '^';
    } else if ('{' <= c && c <= '~') {
        uint8_t us_map[] = {
        /* { */ HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE,
        /* | */ HID_KEYBOARD_SC_BACKSLASH_AND_PIPE,
        /* } */ HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE,
        /* ~ */ HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE,
        };
        kb.code = us_map[c - '{'];
        kb.shift = true;
    }

    return kb;
}


/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param[in] LEDReport  LED status report from the host
 */
void ProcessLEDReport(const uint8_t LEDReport) {
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void SendNextReport(void)
{
    static USB_KeyboardReport_Data_t PrevKeyboardReportData;
    USB_KeyboardReport_Data_t        KeyboardReportData;
    bool                             SendReport = false;

    /* Create the next keyboard report for transmission to the host */
    CreateKeyboardReport(&KeyboardReportData);

    /* Check if the idle period is set and has elapsed */
    if (IdleCount && (!(IdleMSRemaining)))
    {
        /* Reset the idle time remaining counter */
        IdleMSRemaining = IdleCount;

        /* Idle period is set and has elapsed, must send a report to the host */
        SendReport = true;
    }
    else
    {
        /* Check to see if the report data has changed - if so a report MUST be sent */
        SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
    }

    /* Select the Keyboard Report Endpoint */
    Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

    /* Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report */
    if (Endpoint_IsReadWriteAllowed() && SendReport)
    {
        /* Save the current report data for later comparison to check for changes */
        PrevKeyboardReportData = KeyboardReportData;

        /* Write Keyboard Report Data */
        Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();
    }
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
    /* Select the Keyboard LED Report Endpoint */
    Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);

    /* Check if Keyboard LED Endpoint contains a packet */
    if (Endpoint_IsOUTReceived())
    {
        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Read in the LED report from the host */
            uint8_t LEDReport = Endpoint_Read_8();

            /* Process the read LED report from the host */
            ProcessLEDReport(LEDReport);
        }

        /* Handshake the OUT Endpoint - clear endpoint and ready for next report */
        Endpoint_ClearOUT();
    }
}

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 */
void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData)
{
    static bool just_sent;
    uint8_t UsedKeyCodes      = 0;

    /* Clear the report contents */
    memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

    if (!just_sent) {
        display_string(to_send);
        display_string("\n");
        if (*to_send != '\0') {
          kb_input inp = resolve_char(*to_send);
          ReportData->KeyCode[UsedKeyCodes++] = inp.code;
          if (inp.shift) {
              ReportData->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
          }

          to_send++;
          just_sent = true;
        }
    } else {
        just_sent = false;
    }
}

/** Function to manage HID report generation and transmission to the host, when in report mode. */
void HID_Task(void)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
      return;

    /* Send the next keypress report to the host */
    SendNextReport();

    /* Process the LED report sent from the host */
    ReceiveNextReport();
}
