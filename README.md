# 📡 RockBLOCK 9603N

Minimal C library for communicating with the RockBLOCK 9603N modem using the Iridium Short Burst Data (SBD) protocol.

## Building

This project uses CMake. A simple build on Linux might look like:

```bash
cmake -B build
cmake --build build
```

## Usage

```
#include "rockblock_9603.h"

int main(void) {
    if (rbBegin("/dev/ttyUSB0")) {
        rbSendMessage("Hello", 5, 60);
        char buf[RB9603_MAX_MESSAGE_SIZE];
        if (rbReceiveMessage(buf, sizeof(buf)) > 0) {
            // process received data
        }
        rbEnd();
    }
    return 0;
}
```
