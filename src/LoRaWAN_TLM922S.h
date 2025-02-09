/***************
 *
 * LoRaWAN_TLM922S.h - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LORAWAN_TLM922S_H
#define __LORAWAN_TLM922S_H

#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif

#include <Arduino.h>
#include "LoRaWAN_TLM922S_Table.h"

//
// オプション定数
//
#define ECHO_OFF	false
#define ECHO_ON		true
#define JOIN_OTAA	false
#define JOIN_ABP	true
#define ADR_OFF		false
#define ADR_ON		true
#define TX_UCNF		false
#define TX_CNF		true

//
// クラス定義
//
class LoRaWAN_TLM922S : public Stream {
private:
    String _echoBack;               // エコーバックバッファヒープ
    String _terminal;               // ターミナルストアヒープ
    uint8_t *_rxData = nullptr;     // Downlinkデータストアヒープ
    uint8_t _rxDataLen = 0;
    int32_t _value;
    uint8_t _current;
    uint8_t _result;
    uint8_t _rxPort;
    int8_t _margin;
    int8_t _gateways;
    bool _echo = false;

    uint8_t parsePrompt (const uint8_t);
    void putCommand (const uint8_t);
    void clearEchoBack (void) { _echoBack = ""; }
    void pushEchoBack (const uint8_t c) { if (_echo) _echoBack += (char)c; }
    void putEchoBack (void) {
        if (_echo) LORAWAN_TLM922S_DEBUG.print(_echoBack); clearEchoBack();
    }

    bool wait(uint16_t = 0);
    tlmps_t skipPrompt (uint8_t = PS_READY, uint8_t = PS_READY, uint16_t = 1000);
    bool runCommand (uint8_t, uint16_t = 1000);
    bool runBoolCommand (uint8_t, uint16_t = 1000);
    bool getStringCommand (uint8_t, uint16_t = 1000);
    uint32_t getValueCommand (uint8_t, uint16_t = 1000);

    uint32_t parseDecimal (void);
    void parseHexData (void);
    uint32_t parseValue (bool = false, uint16_t timeout = 1000);

public:
 	using super = Stream;
	using super::super;
	using super::write;
	size_t write (const uint8_t c);

	virtual size_t writeRaw (const uint8_t c) = 0;

    // command interface method

    void setEchoThrough (bool = false);
    inline tlmps_t getResult (void) { return (tlmps_t)_result; }
    bool getReady (void);
    tlmps_t nextPrompt (uint16_t = 100);
    inline int32_t getValue (void) { return _value; }
    inline String getData (void) { return _terminal; }
    inline uint8_t *getRxData (void) { return _rxData; }
    inline uint8_t isData (void) { return _terminal.length(); }
    inline uint8_t isRxData (void) { return (_rxData && _rxDataLen ? _rxDataLen : 0); }

    // controler module method

    inline bool factoryReset (void) { return runCommand(EX_MOD_FACTRY); }
    bool reset (void);
    inline bool getVersion (void) { return getStringCommand(EX_MOD_GET_VER); }
    inline bool getDevEUI (void) { return getStringCommand(EX_MOD_GET_DEVEUI); }
    inline bool getAllKey (void) { return getStringCommand(EX_MP_GETKEY); }
    bool sleep (uint16_t = 0);
    bool wakeUp (void);
    void setBaudRate (long);
    inline bool setEcho (bool echo = ECHO_ON) {
        return runCommand(echo ? EX_MOD_SET_ECHO_ON : EX_MOD_SET_ECHO_OFF);
    }
    inline bool modSave (void) { return runCommand(EX_MOD_SAVE); }

    // radio module method

    inline int8_t getDataRate (void) { return getValueCommand(EX_LORA_GET_DR); }
    bool setDataRate (uint8_t);
    int16_t getMaxPayloadSize (int8_t);
    inline bool getAdr (void) { return runBoolCommand(EX_LORA_GET_ADR); }
    inline bool setAdr (bool adr = ADR_ON) {
        return runCommand(adr ? EX_LORA_SET_ADR_ON : EX_LORA_SET_ADR_OFF);
    }
    inline bool loraSave (void) { return runCommand(EX_LORA_SAVE); }

    bool join (bool = JOIN_OTAA);
    bool joinResult (void);

    inline bool getDevAddr (void) { return getStringCommand(EX_LORA_GET_DEVADDR); }
    inline uint32_t getUpCount (void) { return getValueCommand(EX_LORA_GET_UPCNT); }
    inline uint32_t getDownCount (void) { return getValueCommand(EX_LORA_GET_DWCNT); }

    inline bool setLinkCheck (void) { return runCommand(EX_LORA_SET_LINK); }
    inline bool tx (bool confirm, uint8_t fport, String data) {
        return ( tx(confirm, fport) && txData(data) && txRequest() );
    }
    bool tx (bool, uint8_t);
    inline bool txData (char v) { return txData((uint32_t)(v), 2); }
    inline bool txData (uint8_t v) { return txData((uint32_t)(v), 2); }
    inline bool txData (int v) { return txData((uint32_t)(v), 4); }
    inline bool txData (uint16_t v) { return txData((uint32_t)(v), 4); }
    inline bool txData (long v) { return txData((uint32_t)(v), 8); }
    inline bool txData (uint32_t v) { return txData((uint32_t)(v), 8); }
    bool txData (uint32_t, int);
    bool txData (String);
    bool txData (const char*, int);
    bool txRequest (void);
    inline bool txRequest(bool c, uint8_t f, String s) { return tx(c, f, s); }
    bool txResult (void);

    inline bool isLinkCheck (void) { return (_margin >= 0 && _gateways > 0); }
    inline int8_t getMargin (void) { return _margin; }
    inline int8_t getGateways (void) { return _gateways; }
    inline int8_t getRxPort (void) { return _rxPort; }
};

#endif
