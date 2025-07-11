#ifndef ROCKBLOCK_9603_H
#define ROCKBLOCK_9603_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef SERIAL_CONTEXT_SETUP_FUNC
    #ifdef __linux__
        #define SERIAL_CONTEXT_SETUP_FUNC setContextLinux
    #elif __APPLE__
        #define SERIAL_CONTEXT_SETUP_FUNC setContextLinux
    #elif _WIN32
        #define SERIAL_CONTEXT_SETUP_FUNC setContextWindows
    #elif ARDUINO
        #define SERIAL_CONTEXT_SETUP_FUNC setContextArduino
    #endif
#endif

#define RB9603_BAUD 19200U
#define RB9603_MAX_MESSAGE_SIZE 340U

bool rbBegin(const char * port);
bool rbEnd(void);
bool rbSendMessage(const char * data, size_t length, int timeout);
size_t rbReceiveMessage(char * buffer, size_t bufferLength);
int8_t rbGetSignal(void);

#ifdef __cplusplus
}
#endif

#endif // ROCKBLOCK_9603_H
