
/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file sc16is750.h
 * @brief Functions for SC16IS750 SPI to UART bridge
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <string.h> 

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note See datasheet section 7.8 for configuring the
 * "Programmable baud rate generator"
 * 
 */

#define SC16IS750_XTAL_FREQ              		14745600UL // On-board crystal (New mid-2010 Version)

#define SC16IS750_PRESCALER_1                   1 // Default prescaler after reset
#define SC16IS750_PRESCALER_4                   4 // Selectable by setting MCR[7]
#define SC16IS750_PRESCALER						SC16IS750_PRESCALER_1  
#define SC16IS750_BAUDRATE_DIVISOR(baud)		((SC16IS750_XTAL_FREQ/SC16IS750_PRESCALER)/(baud*16UL))
#define SC16IS750_WRITE_CMD(reg)            (reg & 0x78)
#define SC16IS750_READ_CMD(reg)             ((reg | 0x80) & 0xF8)
#define SC16IS750_SPI_FREQ              		4000000UL

#define SC16IS750_DEFAULT_BAUDRATE           	115200 // Default baudrate

#define SC16IS750_IO0                         (0x01 << 0)
#define SC16IS750_IO1                         (0x01 << 1)
#define SC16IS750_IO2                         (0x01 << 2)
#define SC16IS750_IO3                         (0x01 << 3)
#define SC16IS750_IO4                         (0x01 << 4)
#define SC16IS750_IO5                         (0x01 << 5)
#define SC16IS750_IO6                         (0x01 << 6)
#define SC16IS750_IO7                         (0x01 << 7)

#define SC16IS750_IO0_OUTPUT                  (0x01 << 0)
#define SC16IS750_IO1_OUTPUT                  (0x01 << 1)
#define SC16IS750_IO2_OUTPUT                  (0x01 << 2)
#define SC16IS750_IO3_OUTPUT                  (0x01 << 3)
#define SC16IS750_IO4_OUTPUT                  (0x01 << 4)
#define SC16IS750_IO5_OUTPUT                  (0x01 << 5)
#define SC16IS750_IO6_OUTPUT                  (0x01 << 6)
#define SC16IS750_IO7_OUTPUT                  (0x01 << 7)
 
/** See section 8.3 of the datasheet for definitions
  * of bits in the FIFO Control Register (FCR)
  */
#define FCR_RX_IRQ_60                 (3 << 6)
#define FCR_RX_IRQ_56                 (2 << 6)
#define FCR_RX_IRQ_16                 (1 << 6)
#define FCR_RX_IRQ_8                  (0 << 6)
//TX Level only accessible when EFR[4] is set
#define FCR_TX_IRQ_56                 (3 << 4)
#define FCR_TX_IRQ_32                 (2 << 4)
#define FCR_TX_IRQ_16                 (1 << 4)
#define FCR_TX_IRQ_8                  (0 << 4)
//#define FCR_RESERVED                  (1 << 3)
#define FCR_TX_FIFO_RST               (1 << 2)
#define FCR_RX_FIFO_RST               (1 << 1)
#define FCR_ENABLE_FIFO               (1 << 0)
 
//FIFO size
#define SC16IS750_FIFO_RX                  64
#define SC16IS750_FIFO_TX                  64 
 
/** See section 8.4 of the datasheet for definitions
  * of bits in the Line Control Register (LCR)
  */
#define LCR_BITS5                      0x00
#define LCR_BITS6                      0x01
#define LCR_BITS7                      0x02
#define LCR_BITS8                      0x03
 
#define LCR_BITS1                      0x00
#define LCR_BITS2                      0x04
 
#define LCR_NONE                       0x00
#define LCR_ODD                        0x08
#define LCR_EVEN                       0x18
#define LCR_FORCED1                    0x28
#define LCR_FORCED0                    0x38
 
#define LCR_BRK_ENA                    0x40
#define LCR_BRK_DIS                    0x00
 
#define LCR_ENABLE_DIV                 0x80
#define LCR_DISABLE_DIV                0x00
 
#define LCR_ENABLE_ENHANCED_FUNCTIONS (0xBF)
 
/** See section 8.5 of the datasheet for definitions
  * of bits in the Line status register (LSR)
  */
#define LSR_DR   (0x01) /* Data ready in RX FIFO                       */
#define LSR_OE   (0x02) /* Overrun error                               */
#define LSR_PE   (0x04) /* Parity error                                */
#define LSR_FE   (0x08) /* Framing error                               */
#define LSR_BI   (0x10) /* Break interrupt                             */
#define LSR_THRE (0x20) /* Transmitter holding register (FIFO empty)   */
#define LSR_TEMT (0x40) /* Transmitter empty (FIFO and TSR both empty) */
#define LSR_FFE  (0x80) /* At least one PE, FE or BI in FIFO           */
 
/** See section 8.6 of the datasheet for definitions
  * of bits in the Modem control register (MCR)
  */
#define MCR_MDTR                      (1 << 0) /* Data Terminal Ready pin control. */
#define MCR_MRTS                      (1 << 1) /* Request to Send pin control when not in Auto RTS mode.*/
//MCR[2] only accessible when EFR[4] is set
#define MCR_ENABLE_TCR_TLR            (1 << 2)
#define MCR_ENABLE_LOOPBACK           (1 << 4)
//MCR[7:5] only accessible when EFR[4] is set
#define MCR_ENABLE_XON_ANY_CHAR       (1 << 5)
#define MCR_ENABLE_IRDA               (1 << 6)
#define MCR_PRESCALE_1                (0 << 7)
#define MCR_PRESCALE_4                (1 << 7)
 
/** See section 8.7 of the datasheet for definitions
  * of bits in the Modem status register (MSR)
  */
#define MSR_DCTS                      (1 << 0) /* Delta CTS - CTS Changed State      */
#define MSR_DDSR                      (1 << 1) /* Delta DSR - DSR Changed State      */
#define MSR_DDI                       (1 << 2) /* Delta DI  - DI  Changed State      */
#define MSR_DCD                       (1 << 3) /* Delta CD  - CD  Changed State      */
#define MSR_CTS                       (1 << 4) /* CTS State - Complement of NCTS pin */
//MSR[7:5] only accessible when GPIO[7:4] are set as modem pin
#define MSR_DSR                       (1 << 5) /* DSR State - Complement of NDSR pin */
#define MSR_RI                        (1 << 6) /* RI State  - Complement of  NRI pin */
#define MSR_CD                        (1 << 7) /* CD State  - Complement of  NCD pin */
 
/** See section 8.8 of the datasheet for definitions
  * of bits in the Interrupt enable register (IER)
  */
#define IER_ERHRI (0x01) /* Enable received data available interrupt            */
#define IER_ETHRI (0x02) /* Enable transmitter holding register empty interrupt */
#define IER_ELSI  (0x04) /* Enable receiver line status interrupt               */
#define IER_EMSI  (0x08) /* Enable modem status interrupt                       */
//IER[7:5] only accessible when EFR[4] is set
#define IER_SLEEP (0x10) /* Enable sleep mode                                   */
#define IER_XOFFI (0x20) /* Enable XOFF interrupt                               */
#define IER_RTSI  (0x40) /* Enable RTS interrupt                                */
#define IER_CTSI  (0x80) /* Enable CTS interrupt                                */
 
/** See section 8.9 of the datasheet for definitions
  * of bits in the Interrupt identification register (IIR)
  * Bit 0 is set to 0 if an IRQ is pending.
  * Bits 1..5 are used to identify the IRQ source.
  */
#define IIR_IRQ_NOT_PENDING             (0x01)  /* IRQ Not Pending              */
#define IIR_TX_EMPTY                    (0x02)  /* THR Interrupt                */
#define IIR_RX_DATA                     (0x04)  /* RHR Interrupt                */
#define IIR_RX_ERROR                    (0x06)  /* Line Status Error Interrupt  */
#define IIR_RX_TIMEOUT                  (0x0B)  /* RX Timeout Interrupt         */
#define IIR_RX_XOFF                     (0x10)  /* RX XOff Interrupt            */
#define IIR_DCTS_DRTS                   (0x20)  /* Delta CTS or RTS Interrupt   */
#define IIR_DIO                         (0x30)  /* Delta GPIO pin Interrupt     */
 
#define IIR_BITS_USED                   (0x07)
 
/** See section 8.10 of the datasheet for definitions
  * of bits in the Enhanced Features Register (EFR)
  */
#define EFR_ENABLE_CTS                  (1 << 7)
#define EFR_ENABLE_RTS                  (1 << 6)
#define EFR_ENABLE_XOFF2_CHAR_DETECT    (1 << 5)
#define EFR_ENABLE_ENHANCED_FUNCTIONS   (1 << 4)

// EFR[3:0] are used to define Software Flow Control mode
// See section 7.3
#define EFR_DISABLE_TX_FLOW_CTRL        (0x0 << 2)
#define EFR_TX_XON2_XOFF2               (0x1 << 2)
#define EFR_TX_XON1_XOFF1               (0x2 << 2)
#define EFR_TX_XON2_1_XOFF2_1           (0x3 << 2)
 
#define EFR_DISABLE_RX_FLOW_CTRL        (0x0 << 0)
#define EFR_RX_XON2_XOFF2               (0x1 << 0)
#define EFR_RX_XON1_XOFF1               (0x2 << 0)
#define EFR_RX_XON2_1_XOFF2_1           (0x3 << 0)
 
#define EFR_TX_XON2_XOFF2_RX_FLOW       (0x1 << 2) | (0x3 << 0)
#define EFR_TX_XON1_XOFF1_RX_FLOW       (0x2 << 2) | (0x3 << 0)
#define EFR_TX_XON2_1_XOFF2_1_RX_FLOW   (0x3 << 2) | (0x3 << 0)
 
/** See section 8.12 of the datasheet for definitions
  * of bits in the Transmission Control Register (TCR)
  * These levels control when RTS is asserted or de-asserted and auto RTS is enabled. Note that XON/XOFF is not supported in this lib.
  *   Trigger level to halt transmission to the device   : 0..15 (meaning 0-60 with a granularity of 4) 
  *     RTS is de-asserted when RX FIFO is above the set trigger level (i.e. buffer is getting full)  
  *   Trigger level to resume transmission to the device : 0..15 (meaning 0-60 with a granularity of 4) 
  *     RTS is asserted again when RX FIFO drops below the set trigger level (i.e. buffer has room again)    
  */
#define TCR_HALT_DEFAULT                (0x0E) //Warning do not modify
#define TCR_RESUME_DEFAULT              (0x08)  
 
/** See section 8.12 of the datasheet for definitions
  * Note: The device will stop transmissions from the TX FIFO when CTS is de-asserted by external receiver and 
  *       auto CTS is enabled. Note that XON/XOFF is not supported in this lib.
  */
  
/** See section 7.5 and 8.13 of the datasheet for definitions
  * of bits in the Trigger Level Register (TLR) control when an IRQ is generated.
  *   Trigger level for TX interrupt: 0..15 (meaning 0-60 with a granularity of 4) 
  *     IRQ when TX FIFO is above the set trigger level (i.e. buffer is getting full)
  *   Trigger level for RX interrupt: 0..15 (meaning 0-60 with a granularity of 4) 
  *     IRQ when RX FIFO is above the set trigger level (i.e. data is waiting to be read)
  */
#define TLR_TX_DEFAULT                  (0x0E)
#define TLR_RX_DEFAULT                  (0x01)  
 
/**
  * See section 8.16, 8.17, 8.18 of the datasheet for definitions
  * of bits in the IO Direction (IODIR), IO State (IOSTATE) and IO Interrupt Enable register (IOINTENA)
  * 
  * Basically a direct mapping of register bits to GPIO pin.
  */
 
/**
  * See section 8.19 of the datasheet for definitions
  * of bits in the IO Control register (IOC)
  * 
  * Bit 0 is set to 0 to enable latch of IO inputs.
  * Bit 1 is set to enable GPIO[7-4] as /RI, /CD, /DTR, /DST.
  * Bit 2 is set to enable software reset.
  */
#define IOC_ENA_LATCH                   (0x01)
#define IOC_ENA_MODEM                   (0x02) /* Set GPIO[7:4] pins to modem functions */
#define IOC_SW_RST                      (0x04) 
 
/**
  * See section 8.20 of the datasheet for definitions
  * of bits in the Extra Features Control register (EFCR)
  * 
  */
#define EFCR_ENA_RS485                  (0x01)  
#define EFCR_DIS_RX                     (0x02)    
#define EFCR_DIS_TX                     (0x04)    
#define EFCR_ENA_TX_RTS                 (0x10)    
#define EFCR_INV_RTS_RS485              (0x20)    
#define EFCR_ENA_IRDA                   (0x80)

/**
 * @brief EXtra Feature Control Register (EFCR)
 * 
 */
#define EFCR_BIT_MODE	(1U << 0)
#define EFCR_RXDISABLE	(1U << 1)
#define EFCR_TXDISABLE	(1U << 2)
#define EFCR_RTSCON		(1U << 4)
#define EFCR_RTSINVER	(1U << 5)
#define EFCR_IRDA_MODE	(1U << 7)

// See Chapter 11 of datasheet
// #define SPI_READ_MODE_FLAG              (0x80)
#define SC16IS750_RX_QUEUE_SIZE 1024
// #define MAX_COUNT 10

//  SC16IS750 Register definitions (shifted to align)
typedef enum { 
    /*
    * 16750 addresses. Registers accessed when LCR[7] = 0.
    */   
    RHR         = 0x00 << 3, /* Rx buffer register     - Read access  */
    THR         = 0x00 << 3, /* Tx holding register    - Write access */
    IER         = 0x01 << 3, /* Interrupt enable reg   - RD/WR access */

    /*
    * 16750 addresses. Registers accessed when LCR[7] = 1.
    */       
    DLL         = 0x00 << 3, /* Divisor latch (LSB)    - RD/WR access */
    DLH         = 0x01 << 3, /* Divisor latch (MSB)    - RD/WR access */

    /*
    * 16750 addresses. IIR/FCR is accessed when LCR[7:0] <> 0xBF.
    *                  Bit 5 of the FCR register is accessed when LCR[7] = 1.
    */       
    IIR         = 0x02 << 3, /* Interrupt id. register - Read only    */
    FCR         = 0x02 << 3, /* FIFO control register  - Write only   */
    /*
    * 16750 addresses. EFR is accessed when LCR[7:0] = 0xBF.
    */       
    EFR         = 0x02 << 3, /* Enhanced features reg  - RD/WR access */     

    /*
    * 16750 addresses.
    */       
    LCR         = 0x03 << 3, /* Line control register  - RD/WR access */
    /*
    * 16750 addresses. MCR/LSR is accessed when LCR[7:0] <> 0xBF.
    *                  Bit 7 of the MCR register is accessed when EFR[4] = 1.
    */       
    MCR         = 0x04 << 3, /* Modem control register - RD/WR access */
    LSR         = 0x05 << 3, /* Line status register   - Read only    */

    /*
    * 16750 addresses. MSR/SPR is accessed when LCR[7:0] <> 0xBF.
    *                  MSR, SPR register is accessed when EFR[1]=0 and MCR[2]=0.
    */       
    MSR         = 0x06 << 3, /* Modem status register  - Read only    */
    SPR         = 0x07 << 3, /* Scratchpad register    - RD/WR access */
    /*
    * 16750 addresses. TCR/TLR is accessed when LCR[7:0] <> 0xBF.
    *                  TCR, TLR register is accessed when EFR[1]=1 and MCR[2]=1.
    */       
    TCR         = 0x06 << 3, /* Transmission control register - RD/WR access */
    TLR         = 0x07 << 3, /* Trigger level register        - RD/WR access */

    /*
    * 16750 addresses. XON, XOFF is accessed when LCR[7:0] = 0xBF.
    */       
    XON1        = 0x04 << 3, /* XON1 register          - RD/WR access */
    XON2        = 0x05 << 3, /* XON2 register          - RD/WR access */
    XOFF1       = 0x06 << 3, /* XOFF1 register         - RD/WR access */
    XOFF2       = 0x07 << 3, /* XOFF2 register         - RD/WR access */

    /*
    * 16750 addresses.
    */       
    TXLVL       = 0x08 << 3, /* TX FIFO Level register - Read only    */
    RXLVL       = 0x09 << 3, /* RX FIFO Level register - Read only    */
    IODIR       = 0x0A << 3, /* IO Pin Direction reg   - RD/WR access */
    IOSTATE     = 0x0B << 3, /* IO Pin State reg       - RD/WR access */
    IOINTENA    = 0x0C << 3, /* IO Interrupt Enable    - RD/WR access */
    //        reserved    = 0x0D << 3,
    IOCTRL      = 0x0E << 3, /* IO Control register    - RD/WR access */
    EFCR        = 0x0F << 3, /* Extra features reg     - RD/WR access */

} SC16IS750_Register_t;

/**Structure for flow control configuration.
 *
 *  
 */
typedef enum
{
    Disabled = 0,
    RTS,
    CTS,
    RTSCTS
} SC16IS750_Flow_t;

/**Structure for parity configuration.
 *
 *  
 */
typedef enum  
{
    None = 0,
    Odd,
    Even,
    Forced1,
    Forced0
} SC16IS750_Parity_t;

/**Structure for internal RX fifo.
 *
 *  
 */
typedef struct 
{
    uint16_t head, tail, count;
    uint8_t e[SC16IS750_RX_QUEUE_SIZE];
} SC16IS750_rxQueue_t;

/**Structure to initialise SPI bus.
 *
 *  
 */
typedef struct
{
	spi_host_device_t spi_host;
	int spi_freq;
	int spi_pin_cs;
	gpio_num_t spi_pin_int;
} SC16IS750_DeviceConfig_t;

/**
 * @brief Structure to store configuration after init.
 * 
 */
typedef struct 
{
	uint8_t baudrate;
	uint8_t dataformat;  
	uint8_t flowctrl;  
	uint8_t fifoformat;
	bool fifoenable;      
} SC16IS750_SerialConfig_t;

/**Structure to initialise internal registers.
 *
 *  
 */
typedef struct 
{
	int baudrate;
	int format_bits;  
	SC16IS750_Parity_t parity;  
	int format_stop_bits;
	SC16IS750_Flow_t flow_type;     
	int trigger_resume;
	int trigger_halt; 
} SC16IS750_SerialParams_t;

/**Read bytes from the internal queue.

 *
 *   @return -1 if queue is empty, the number of character specified by len otherwise
 */
int SC16IS750_queue_read_bytes(uint8_t *data, int len);

/**Read one character from the internal queue.

 *
 *   @return -1 if queue is empty, the firt in character otherwise
 */
int SC16IS750_queue_read();

/**Push one element in queue.

 *   
 *   @return -1 if queue is full
 */
int queue_push(uint8_t element);

/**Pop one element of queue.

 *
 *   @return -1 if queue is empty
 */
int queue_pop(uint8_t *element);

/**Return number of elements in queue.

 *
 *   @return 0 if queue is empty
 */
int queue_size();


/**Determine how many characters are in the internal queue.

 *
 *   @return 0 if queue is empty, the number of characters otherwise
 */
int SC16IS750_queue_available();

/** Determine if there is a character available to read.
 * This is data that's already arrived and stored in the receive
 * buffer (which holds 64 chars).
 *
 *   @return 1 if there is a character available to read, 0 otherwise
 */
uint8_t SC16IS750_readable();

/** Determine how many characters are available to read.
 * This is data that's already arrived and stored in the receive
 * buffer (which holds 64 chars).
 *
 *   @return int Characters available to read
 */ 
uint8_t SC16IS750_readableCount();

/** Determine if there is space available to write a character.    
 *   @return 1 if there is a space for a character to write, 0 otherwise
 */
uint8_t SC16IS750_writable();

/** Determine how much space available for writing characters.
 * This considers data that's already stored in the transmit
 * buffer (which holds 64 chars).
 *
 *   @return int character space available to write
 */  
uint8_t SC16IS750_writableCount();

/**
 * Read char from UART Bridge.
 * Acts in the same manner as 'Serial.read()'.  
 *   @param none    
 *   @return char read or -1 if no data available. 
 */ 
uint8_t SC16IS750_getc();  

/**
 * Write char to UART Bridge. Blocking when no free space in FIFO
 *   @param value char to be written    
 *   @return value written  
 */
void SC16IS750_putc(int c);

/**
 * @brief Write byte array to UART Bridge. Blocking when no free space in FIFO
 * 
 * @param buf byte array to be written
 * @param len number of bytes to write
 */
void SC16IS750_writeBytes(uint8_t *data, int len);

/** Set baudrate of the serial port.    
 *  @param  baud integer baudrate (4800, 9600 etc)
 *  @return none
 */
void SC16IS750_baud(int baudrate);   

/** Set the transmission format used by the serial port.   
 *   @param bits      The number of bits in a word (5-8; default = 8)
 *   @param parity    The parity used (None, Odd, Even, Forced1, Forced0)
 *   @param stop_bits The number of stop bits (1 or 2; default = 1) 
 *   @return none   
 */
void format(int bits, SC16IS750_Parity_t parity, int stop_bits);

/** Generate a break condition on the serial line
 *  @param none
 *  @return none 
 */
void SC16IS750_send_break();

/** Set a break condition on the serial line
 *  @param enable  break condition
 *  @return none   
 */
void SC16IS750_set_break(bool enable);

/** Set the flow control type on the serial port
 *  SC16IS750 supports only Flow, Pins can not be selected. 
 *  This method sets hardware flow control levels. SC16IS750 supports XON/XOFF, but this is not implemented.  
 *
 *  @param type the flow control type (Disabled, RTS, CTS, RTSCTS)     
 *  @return none   
 */
void set_flow_control(SC16IS750_Flow_t type);

/** Set the RX FIFO flow control levels
 *  This method sets hardware flow control levels. SC16IS750 supports XON/XOFF, but this is not implemented.   
 *  Should be called BEFORE Auto RTS is enabled.    
 *
 *  @param resume trigger level to resume transmission (0..15, meaning 0-60 with a granularity of 4)     
 *  @param halt trigger level to resume transmission (0..15, meaning 0-60 with granularity of 4)           
 *  @return none   
 */
void SC16IS750_set_flow_triggers(int resume, int halt);

/** Set the Modem Control register
 *  This method sets prescaler, enables TCR and TLR
 *
 *  @param none 
 *  @return none 
 */
void SC16IS750_set_modem_control();

/**
 * Check that UART is connected and operational.
 *   @param  none
 *   @return bool true when connected, false otherwise
 */
bool SC16IS750_connected();

/** FIFO control, sets TX and RX IRQ trigger levels and enables FIFO and save in _config
 *  Note FCR[5:4] (=TX_IRQ_LVL) only accessible when EFR[4] is set (enhanced functions enable)
 *  Note TLR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set 
 *   @param  none
 *   @return none
 */
void SC16IS750_set_fifo_control();

/** Set transmitter auto RTS feature
 *   @param  none
 *   @return none
 */
void SC16IS750_setAutoRts();

/** Flush the UART FIFOs while maintaining current FIFO mode.
 *   @param  none
 *   @return none
 */
void SC16IS750_flush();

/** Set direction of I/O port pins.
 * This method is specific to the SPI-I2C UART and not found on the 16750
 * Note: The SC16IS752 does not have separate GPIOs for Channel_A and Channel_B.    
 *   @param  bits Bitpattern for I/O (1=output, 0=input)
 *   @return none
 */
void SC16IS750_ioSetDirection(uint8_t bits);

/** Set bits of I/O port pins.
 * This method is specific to the SPI-I2C UART and not found on the 16750
 * Note: The SC16IS752 does not have separate GPIOs for Channel_A and Channel_B.    
 *   @param  bits Bitpattern for I/O (1= set output bit, 0 = clear output bit)
 *   @return none
 */
void SC16IS750_ioSetState(uint8_t bits);

/** Get bits of I/O port pins.
 * This method is specific to the SPI-I2C UART and not found on the 16750
 * Note: The SC16IS752 does not have separate GPIOs for Channel_A and Channel_B.    
 *   @param  data       Pointer to char to store data
 *   @return none
 */
void SC16IS750_ioGetState(uint8_t *data);

/** Software Reset SC16IS750 device.
 * This method is specific to the SPI-I2C UART and not found on the 16750
 * Note: The SC16IS752 does not have separate Reset for Channel_A and Channel_B.    
 *   @param  none
 *   @return none
 */
void SC16IS750_swReset();

/** Initialise internal registers
 * If initialisation fails this method does not return.    
 *   @param none
 *   @return none 
 */
void SC16IS750_init(SC16IS750_SerialParams_t cfg);

/** Write value to internal register.
 *   @param reg   The address of the Register (enum RegisterName)
 *   @param data              The 8bit value to write
 *   @return none 
 */
void SC16IS750_writeRegister(SC16IS750_Register_t reg, uint8_t data);

/** Read value from internal register.
 *   @param reg   The address of the Register (enum RegisterName)
 *   @param data              Pointer to char to store data
 *   @return none             
 */
void SC16IS750_readRegister(SC16IS750_Register_t reg, uint8_t *data);

/** Read multiple datavalues of receive register.
 * 
 *   @param char* databytes   The pointer to the block of data
 *   @param len               The number of bytes to read
 *   @return none 
 */
void SC16IS750_readDataBlock(uint8_t* data, int len);

/** Hardware Reset SC16IS750 device.
 * This method is only functional when the Reset pin has been declared and is also connected
 *   @param  none
 *   @return none
 */
// void SC16IS750_hwReset();

/** Initialise SC16IS750 device.
 *   @param  none
 *   @return none
 */
esp_err_t SC16IS750_create(SC16IS750_DeviceConfig_t *config);

/** Delete SC16IS750 device.
 *   @param  none
 *   @return none
 */
esp_err_t SC16IS750_delete();

/** HAL layer SPI write.
 *   @param  reg SPI reg
 *   @param  frame SPI frame
 *   @param  len Frame length
 *   @return none
 */
void SC16IS750_hal_write(uint8_t reg, uint8_t * frame, int len);

/** HAL layer SPI read.
 *   @param  reg SPI reg
 *   @param  receive_frame SPI rx frame
 *   @param  len Frame length
 *   @return none
 */
void SC16IS750_hal_read(uint8_t reg, uint8_t * receive_frame, int len);
 
#ifdef __cplusplus
}
#endif