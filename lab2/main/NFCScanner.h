// Routines for interfacing the PN532 sensor and reading NFC tags

#pragma once

// This union is for convenience to access an NFC tag as either 4 individual bytes or as
// a single int
typedef union {
    uint32_t uidInt;        // NFC tag as an unsigned int value
    uint8_t  uid[4];        // individual bytes of tag
} NFCTag;


// Function prototypes

// Configure an initialize the PN532 sensor
esp_err_t nfcScanner_config();

// Read an NFC tag
NFCTag nfcScanner_readTag();
