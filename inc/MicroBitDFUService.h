#ifndef MICROBIT_DFU_SERVICE_H
#define MICROBIT_DFU_SERVICE_H

#include "MicroBit.h"

// MicroBit ControlPoint OpCodes

// Requests transfer to the Nordic DFU bootloader.
// This will only occur if 
#define MICROBIT_DFU_OPCODE_START_DFU       1   
#define MICROBIT_DFU_OPCODE_START_PAIR      2

#define MICROBIT_DFU_HISTOGRAM_WIDTH        5
#define MICROBIT_DFU_HISTOGRAM_HEIGHT       5

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitDFUServiceUUID[];
extern const uint8_t  MicroBitDFUServiceControlCharacteristicUUID[];
extern const uint8_t  MicroBitDFUServiceFlashCodeCharacteristicUUID[]; 

// Handle on the memory resident Nordic bootloader.
extern "C" void bootloader_start(void);

/**
  * Class definition for a MicroBit Device Firmware Update loader.
  *
  * This is actually just a frontend to a memory resident nordic DFU loader.
  * Here we deal with the MicroBit 'pairing' functionality with BLE devices, and
  * very basic authentication and authorization. 
  *
  * This implementation is not intended to be fully secure, but rather intends to:
  *
  * 1. Provide a simple mechanism to identify an individual MicroBit amongst a classroom of others
  * 2. Allow BLE devices to discover and cache a passcode that can be used to flash the device over BLE.
  * 3. Provide an escape route for programs that 'brick' the MicroBit. 
  *
  * Represents the device as a whole, and includes member variables to that reflect the components of the system.
  */
class MicroBitDFUService
{                                    
    public:
    
    /**
      * Constructor. 
      * Create a representation of a MicroBit device.
      * @param messageBus callback function to receive MicroBitMessageBus events.
      */
    MicroBitDFUService(BLEDevice &BLE);  
    
    /**
      * Returns the friendly name for this device, autogenerated from our Device ID.
      *
      * @param name Pointer to a string where the data will be written.
      * @return The number of bytes written.
      */
    int getName(char *name);

    /**
      * Begin the pairing process. Typically called when device is powered up with buttons held down.
      * Scroll a description on the display, then displays the device ID code as a histogram on the matrix display.
      */
    void pair();
    
    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    virtual void onDataWritten(const GattWriteCallbackParams *params);

    private:

    // BLE pairing name of this device, encoded as an integer.
    uint32_t    flashCode;
    
    // State of paiting process.
    bool authenticated;
    bool flashCodeRequested;
    
    // Bluetooth stack we're running on.
    BLEDevice           &ble;

    // memory for our 8 bit control characteristics.
    uint8_t             controlByte;

    GattAttribute::Handle_t microBitDFUServiceControlCharacteristicHandle;
    GattAttribute::Handle_t microBitDFUServiceFlashCodeCharacteristicHandle;

    // Displays the device's ID code as a histogram on the LED matrix display.
    void showNameHistogram();
    
    // Displays an acknowledgement on the LED matrix display.
    void showTick();

    // Update BLE characteristic to release our flash code.
    void releaseFlashCode();
};

#endif
