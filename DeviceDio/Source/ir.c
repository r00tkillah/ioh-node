#include "ir.h"

#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
#include <os.h>
#include <Api.h>
#include <string.h>

/* needed for debug */
#include "DeviceDefs.h"


#define RXBUFSIZE  16
#define TXBUFSIZE  16

static uint8_t u1_rx_buf[RXBUFSIZE];
static uint8_t u1_tx_buf[TXBUFSIZE];

typedef struct message_parts message_parts;
struct __attribute__ ((__packed__)) message_parts {
    char magic[4];
    uint32_t number;
    uint16_t checksum;
};

typedef union message {
    uint8_t bytes[12];
    message_parts parts;
} message;

static struct send_info_t {
    bool sending;       /* set when sending */
    bool request_send;  /* set when you want to send */
    int8_t cur_bit;
    uint8_t cur_byte;
    message msg;
} send_info[2];


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

enum uart_bit {
    START_BIT = -1,
    BIT_0,
    BIT_1,
    BIT_2,
    BIT_3,
    BIT_4,
    BIT_5,
    BIT_6,
    BIT_7,
    STOP_BIT
};

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

static bool uart_signal(struct send_info_t *send) {
    bool signal = false;

    if (send->request_send) {
        send->sending = true;
        DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nrequest rcvd");
    }
    if (!send->sending) {
        return signal;
    }

    switch (send->cur_bit) {
    case START_BIT:
        signal = false;
        send->cur_bit++;
        break;
    case BIT_0:
    case BIT_1:
    case BIT_2:
    case BIT_3:
    case BIT_5:
    case BIT_6:
    case BIT_7:
        {
            const uint8_t byte = send->msg.bytes[send->cur_byte];
            const uint8_t mask = 1 << send->cur_bit;

            if (byte & mask) {
                signal = true;
            }
            send->cur_bit++;
        }
        break;
    case STOP_BIT:
        signal = true;
        send->cur_bit = START_BIT;
        send->cur_byte = (send->cur_byte + 1) % sizeof(message_parts);
        DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nstop bit");
        break;
    default:
        signal = true;
        send->cur_bit = START_BIT;
        send->cur_byte = 0;
        break;
    }
    return signal;
}

static void setup_send_info(struct send_info_t *send)
{
    send->sending = false;
    send->request_send = false;
    send->cur_byte = 0;
    send->cur_bit = START_BIT;
    strcpy((char*)(send->msg.bytes), "BOOP");
}

static void send(uint8_t i, uint32_t number)
{
    send_info[i].request_send = true;
    send_info[i].msg.parts.number = number;
    send_info[i].msg.parts.checksum = 0; /* FIXME */
}

static void on_timer0(uint32_t device_id, uint32_t item_bitmap)
{
    static uint8_t bitcounter = 0;
    static bool signal[2];

    if (device_id != E_AHI_DEVICE_TIMER0) {
        /* how did we get here, anyway? */
        return; 
    }

    if (bitcounter % 16 == 0) {
        /* this is a serial bit time slot */
        signal[0] = uart_signal(&(send_info[0]));
        signal[1] = uart_signal(&(send_info[1]));
    }
    bitcounter++;

    {
        /* just for now spit out raw serial */
        uint8_t on = 0;
        uint8_t off= 0;
        
        if (signal[0]) {
            on |= 1;
        } else {
            off |= 1;
        }
        if (signal[1]) {
            on |= 2;
        } else {
            off |= 2;
        }
        vAHI_DoSetDataOut(on, off);
    }
#if 0
    if (item_bitmap & E_AHI_TIMER_RISE_MASK) {
        /* rising edge */
    } else {
        /* there is a mask for the falling edge, but there really
           isn't another reason we should be here */
    }
#endif
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
    /* setup the UARTS */
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
    /* setup UART0 */
#if 0 /* uart0 still used for debug */
    vAHI_UartSetBaudRate(E_AHI_UART_0, E_AHI_UART_RATE_4800);
    vAHI_UartSetControl(E_AHI_UART_0, FALSE, FALSE, E_AHI_UART_WORD_LEN_8,
                        E_AHI_UART_1_STOP_BIT,
                        E_AHI_UART_RTS_LOW);
#endif


    setup_send_info(&(send_info[0]));
    setup_send_info(&(send_info[1]));
    send(0, 1234);
    /* setup DO */
    if (bAHI_DoEnableOutputs(TRUE) == TRUE) {
        DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nEnabled DO");
    } else {
        DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nEnabling DO FAIL!!");
    }

    /* setup the timer */
    vAHI_Timer0RegisterCallback(on_timer0);
    /* clock the timer from 16mhz internal clock */
    vAHI_TimerClockSelect(E_AHI_TIMER_0, FALSE, FALSE);
    /* disable clock gating */
    vAHI_TimerConfigureOutputs(E_AHI_TIMER_0, FALSE, TRUE);
    /* 16mhz, timer0, high/low interrupts, no output */
    vAHI_TimerEnable(E_AHI_TIMER_0, 0, TRUE, TRUE, FALSE);
    /* hit the go button */
    vAHI_TimerStartRepeat(E_AHI_TIMER_0, 211, 211*2);

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
