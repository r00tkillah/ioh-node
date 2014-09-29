#include "ir.h"

#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
#include <os.h>

/* needed for debug */
#include "DeviceDefs.h"


#define RXBUFSIZE  16
#define TXBUFSIZE  16

static uint8_t u1_rx_buf[RXBUFSIZE];
static uint8_t u1_tx_buf[TXBUFSIZE];

/*
 *  Message format:
 *  BOOP followed by 4 bytes followed by 2 byte checksum
 */

/**
 *
 * 0: unseen
 * ..
 * ..
 * 10: final byte
 */
static uint8_t boop_state[2];
enum boop_states {
    BOOP_NONE = 0,
    BOOP_B,
    BOOP_O0,
    BOOP_O1,
    BOOP_P,
    BOOP_I0,
    BOOP_I1,
    BOOP_I2,
    BOOP_I3,
    BOOP_C0,
    BOOP_C1,
    BOOP_MAX };

static uint8_t scratch[2][6];


static uint8_t uart_to_index(uint8_t uart)
{
    if (uart == E_AHI_UART_0) {
        return 0;
    }
    return 1;
}

static uint8_t uart_to_scratch(uint8_t uart)
{
    if (uart == E_AHI_UART_0) {
        return 0;
    }
    return 1;
}

static bool is_a_boop(uint8_t input, uint8_t state)
{
    static const uint8_t boop[4] = { 'B', 'O', 'O', 'P'};

    if (state > sizeof(boop)) return false;
    if (input == boop[state]) return true;
    return false;
}


static void handle_rx(uint8_t uart)
{
    uint16_t lvl;
    uint8_t  byte;
    uint8_t *state;

    lvl = u16AHI_UartReadRxFifoLevel(uart);
    if (lvl == 0) return;

    state = &(boop_state[uart_to_index(uart)]);
    DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nstate=%d", *state);
    byte = u8AHI_UartReadData(uart_to_index(uart));
    DBG_vPrintf(DEBUG_DEVICE_FUNC, " byte=%c", byte);
    /* states will be NONE, B, O, O, P, I0, I1, I2 I3, C0, C1 */
    if (*state < BOOP_P) { /* still looking for rest of BOOP */
        /* we haven't gotten boop yet */
        if (is_a_boop(byte, *state)) {
            *state = *state + 1;
        } else {
            *state = BOOP_NONE;
        }
    } else if (*state < BOOP_C1) { /* looking for integer digits  or checksum*/
        const uint8_t index = uart_to_scratch(uart);
        const uint8_t offset = *state - BOOP_P;
        scratch[index][*state - BOOP_P] = byte;
        *state = *state + 1;
    }
    if (*state == BOOP_C1) { /* full packet in buffer */
        const uint8_t index = uart_to_scratch(uart);
        const uint32_t number = *((uint32_t*)(&(scratch[index][0])));
        const uint16_t checksum = *((uint16_t*)(&(scratch[index][4])));

        /* TODO: check checksum */

        /* TODO: do something with valid packet! */

        *state = BOOP_NONE;
    }
}


bool initialize_ir_hw(void)
{
    boop_state[0] = BOOP_NONE;
    boop_state[1] = BOOP_NONE;
    if (!bAHI_UartEnable(E_AHI_UART_1,
                         u1_tx_buf,
                         TXBUFSIZE,
                         u1_rx_buf,
                         RXBUFSIZE)) {
        return false;
    }
    vAHI_UartSetBaudRate(E_AHI_UART_1, E_AHI_UART_RATE_4800);
    vAHI_UartSetControl(E_AHI_UART_1, FALSE, FALSE, E_AHI_UART_WORD_LEN_8,
                        E_AHI_UART_1_STOP_BIT,
                        E_AHI_UART_RTS_LOW);
    /* TODO: setup UART0 */

    return true;
}

void ir_tick(void)
{
    handle_rx(E_AHI_UART_1); /* hack */
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
