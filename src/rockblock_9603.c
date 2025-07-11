#include "rockblock_9603.h"
#include "serial.h"
#include "crossplatform.h"
#include <string.h>
#include <stdio.h>

static bool sendCommand(const char *cmd, char *response, size_t responseLen, unsigned int timeoutMs);
static int readLine(char *buffer, size_t length, unsigned int timeoutMs);

bool rbBegin(const char *port)
{
    if (!SERIAL_CONTEXT_SETUP_FUNC(port, RB9603_BAUD))
    {
        return false;
    }
    return context.serialInit();
}

bool rbEnd(void)
{
    if (context.serialDeInit)
    {
        return context.serialDeInit();
    }
    return false;
}

bool rbSendMessage(const char *data, size_t length, int timeout)
{
    if (!data || length == 0 || length > RB9603_MAX_MESSAGE_SIZE)
    {
        return false;
    }

    char cmd[RB9603_MAX_MESSAGE_SIZE + 16];
    snprintf(cmd, sizeof(cmd), "AT+SBDWT=%s\r", data);
    char resp[128] = {0};
    if (!sendCommand(cmd, resp, sizeof(resp), 5000))
    {
        return false;
    }
    if (strstr(resp, "OK") == NULL)
    {
        return false;
    }

    if (!sendCommand("AT+SBDIX\r", resp, sizeof(resp), (unsigned int)timeout * 1000))
    {
        return false;
    }

    /* Successful transmit if MO status is 0 */
    int moStat = -1;
    sscanf(resp, "+SBDIX: %d", &moStat);
    return (moStat == 0);
}

size_t rbReceiveMessage(char *buffer, size_t bufferLength)
{
    if (!buffer || bufferLength == 0)
    {
        return 0;
    }

    char resp[128] = {0};
    if (!sendCommand("AT+SBDIX\r", resp, sizeof(resp), 45000))
    {
        return 0;
    }

    int moStat, moMsn, mtStat, mtMsn, mtLen, queued;
    moStat = moMsn = mtStat = mtMsn = mtLen = queued = 0;
    sscanf(resp, "+SBDIX: %d,%d,%d,%d,%d,%d", &moStat, &moMsn, &mtStat, &mtMsn, &mtLen, &queued);
    if (mtLen > 0 && mtLen < (int)bufferLength)
    {
        if (!sendCommand("AT+SBDRT\r", buffer, bufferLength, 5000))
        {
            return 0;
        }
        size_t len = strlen(buffer);
        return len > (size_t)mtLen ? (size_t)mtLen : len;
    }

    return 0;
}

int8_t rbGetSignal(void)
{
    char resp[64] = {0};
    if (!sendCommand("AT+CSQ\r", resp, sizeof(resp), 5000))
    {
        return -1;
    }
    int sig = -1;
    sscanf(resp, "+CSQ: %d", &sig);
    return (int8_t)sig;
}

static bool sendCommand(const char *cmd, char *response, size_t responseLen, unsigned int timeoutMs)
{
    if (!context.serialWrite || !context.serialRead)
    {
        return false;
    }
    context.serialWrite(cmd, strlen(cmd));
    return readLine(response, responseLen, timeoutMs) > 0;
}

static int readLine(char *buffer, size_t length, unsigned int timeoutMs)
{
    size_t pos = 0;
    unsigned long start = millis();
    while (pos < length - 1)
    {
        if ((millis() - start) > timeoutMs)
        {
            break;
        }
        char c;
        int r = context.serialRead(&c, 1);
        if (r > 0)
        {
            if (c == '\r')
            {
                continue;
            }
            if (c == '\n')
            {
                if (pos == 0)
                {
                    continue;
                }
                break;
            }
            buffer[pos++] = c;
        }
        else
        {
            usleep(1000);
        }
    }
    buffer[pos] = '\0';
    return (int)pos;
}
