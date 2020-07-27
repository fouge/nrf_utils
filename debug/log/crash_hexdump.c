//
// Created by Cyril Fougeray on 2019-08-27.
//

#include <assert.h>
#include <CrashCatcher.h>
#include <nrf_delay.h>
#include <app_uart.h>
#include <boards.h>

/* We use crashcatcher when Hardfault implementation is not from nRF */
#if (HARDFAULT_HANDLER_ENABLED == 0)

static void _uart_init()
{
    // Configure UART0 pins.
    nrf_gpio_cfg_output(TX_PIN_NUMBER);
    nrf_gpio_cfg_input(RX_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);

    NRF_UART0->PSELTXD = TX_PIN_NUMBER;
    NRF_UART0->PSELRXD = RX_PIN_NUMBER;
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1M;

    // Clean out possible events from earlier operations
    NRF_UART0->EVENTS_RXDRDY = 0;
    NRF_UART0->EVENTS_TXDRDY = 0;
    NRF_UART0->EVENTS_ERROR = 0;

    // Activate UART.
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
    NRF_UART0->INTENSET = 0;
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 0;
}

static void _uart_put_byte(uint8_t cr)
{
    NRF_UART0->TXD = cr;

    while (NRF_UART0->EVENTS_TXDRDY != 1)
    {
        // Wait for TXD data to be sent.
    }

    NRF_UART0->EVENTS_TXDRDY = 0;
}

static void _print(uint8_t* buf, uint8_t len)
{
    uint8_t l = 0;
    NRF_UART0->TASKS_STARTTX = 1;
    while (l < len)
    {
        _uart_put_byte(buf[l]);
        l++;
    }
    NRF_UART0->TASKS_STOPTX = 1;
}

static void dumpHexDigit(uint8_t nibble)
{
    static const char hexToASCII[] = "0123456789ABCDEF";

    assert( nibble < 16 );
    _print((uint8_t *) &hexToASCII[nibble], 1);
}

static void dumpByteAsHex(uint8_t byte)
{
    dumpHexDigit(byte >> 4);
    dumpHexDigit(byte & 0xF);
}

static void dumpBytes(const uint8_t* pMemory, size_t elementCount)
{
    size_t i;
    uint8_t new_line[2] = "\r\n";

    for (i = 0 ; i < elementCount ; i++)
    {
        /* Only dump 16 bytes to a single line before introducing a line break. */
        if (i != 0 && (i & 0xF) == 0)
            _print(new_line, sizeof(new_line));
        dumpByteAsHex(*pMemory++);
    }
}

static void dumpHalfwords(const uint16_t* pMemory, size_t elementCount)
{
    size_t i;
    uint8_t new_line[2] = "\r\n";

    for (i = 0 ; i < elementCount ; i++)
    {
        uint16_t val = *pMemory++;
        /* Only dump 8 halfwords to a single line before introducing a line break. */
        if (i != 0 && (i & 0x7) == 0)
            _print(new_line, sizeof(new_line));
        dumpBytes((uint8_t*)&val, sizeof(val));
    }
}

static void dumpWords(const uint32_t* pMemory, size_t elementCount)
{
    size_t i;
    uint8_t new_line[2] = "\r\n";

    for (i = 0 ; i < elementCount ; i++)
    {
        uint32_t val = *pMemory++;
        /* Only dump 4 words to a single line before introducing a line break. */
        if (i != 0 && (i & 0x3) == 0)
            _print(new_line, sizeof(new_line));
        dumpBytes((uint8_t*)&val, sizeof(val));
    }
}

/* Called at the beginning of crash dump. You should provide an implementation which prepares for the dump by opening
   a dump file, prompting the user to begin a crash dump, or whatever makes sense for your scenario. */
void CrashCatcher_DumpStart(const CrashCatcherInfo* pInfo)
{
#if NRF_MODULE_ENABLED(APP_UART)
    /* 
     * If we are using the UART for logging
     * We give some time to print the last characters
     * then we close UART
     */
    nrf_delay_ms(500);
    app_uart_flush();
    app_uart_close();
#endif
    
    _uart_init();

    uint8_t crash_start_flag[17] = "\r\n\r\n###CRASH###\r\n";
    _print(crash_start_flag, sizeof(crash_start_flag));
}

/* Called to obtain an array of regions in memory that should be dumped as part of the crash.  This will typically
   be all RAM regions that contain volatile data.  For some crash scenarios, a user may decide to also add peripheral
   registers of interest (ie. dump some ethernet registers when you are encountering crashes in the network stack.)
   If NULL is returned from this function, the core will only dump the registers. */
const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void)
{
    static const CrashCatcherMemoryRegion regions[] = {
#if defined(BOARD_CUSTOM)
        {0x20002558, 0x20010000, CRASH_CATCHER_BYTE}, /* RAM content */
        {0xFFFFFFFF, 0xFFFFFFFF, CRASH_CATCHER_BYTE}
#else
    #error "Target device isn't supported."
#endif
    };
    return regions;
}

/* Called to dump the next chunk of memory to the dump (this memory may point to register contents which has been copied
   to memory by CrashCatcher already.  The element size will be 8-bits, 16-bits, or 32-bits.  The implementation should
   use reads of the specified size since some memory locations may only support the indicated size. */
void CrashCatcher_DumpMemory(const void* pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount)
{
    uint8_t new_line[2] = "\r\n";

    switch (elementSize)
    {
        case CRASH_CATCHER_BYTE:
            dumpBytes(pvMemory, elementCount);
            break;
        case CRASH_CATCHER_HALFWORD:
            dumpHalfwords(pvMemory, elementCount);
            break;
        case CRASH_CATCHER_WORD:
            dumpWords(pvMemory, elementCount);
            break;
    }
    _print(new_line, sizeof(new_line));
}

/* Called at the end of crash dump. You should provide an implementation which cleans up at the end of dump. This could
   include closing a dump file, blinking LEDs, infinite looping, and/or returning CRASH_CATCHER_TRY_AGAIN if
   CrashCatcher should prepare to dump again incase user missed the first attempt. */
CrashCatcherReturnCodes CrashCatcher_DumpEnd(void)
{
    nrf_delay_ms(100);

    uint8_t crash_end_flag[12] = "###END###\r\n";
    _print(crash_end_flag, sizeof(crash_end_flag));

    return CRASH_CATCHER_EXIT;
}

#endif // (HARDFAULT_HANDLER_ENABLED == 0)
