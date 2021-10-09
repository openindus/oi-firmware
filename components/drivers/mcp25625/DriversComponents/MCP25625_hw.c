/****************************************************************************
* Title                 :   MCP25625 CLICK
* Filename              :   MCP25625_hw.c
* Author                :   MSV
* Origin Date           :   30/01/2016
* Notes                 :   Hardware layer
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  30/01/16     .1                  MSV     Interface Created.
*
*****************************************************************************/

/**
 * @file MCP25625_hw.c
 * @brief <h3> Hardware Layer </h3>
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "MCP25625_hw.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#if defined(__MIKROC_PRO_FOR_ESP32__)
#else
    #define BIT 1
#endif
/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

typedef struct {

    uint8_t     value;
    uint8_t     address;
    uint8_t     mask;
    uint8_t     buffer;

} reg_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static reg_t reg;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

//---------------------------------------
//      READ FROM REGISTERS
//---------------------------------------
//
// All whitout suffix function uses it.
//---------------------------------------
static void hw_read
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t count
);

//---------------------------------------
//      WRITE TO REGISTERS
//---------------------------------------
//
// All SET sufix function uses it.
//---------------------------------------
static void hw_write
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t count
);

//---------------------------------------
//      MODIFIES THE REGISTER
//---------------------------------------
//
// All UPDATE or ENABLE suffix functions uses it.
// Modifies only masked bits.
//---------------------------------------
static void hw_modify
(
        uint8_t reg,
        uint8_t mask,
        uint8_t *value
);

//---------------------------------------
//      FAST TX WRITE
//---------------------------------------
static void hw_tx
(
        uint8_t start,
        uint8_t *buffer,
        uint8_t len
);

//---------------------------------------
//      FAST RX READ
//---------------------------------------
static void hw_rx
(
        uint8_t start,
        uint8_t *buffer,
        uint8_t len
);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static void hw_read
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t count
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_read( CMD_READ ,reg, buffer, count );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_READ );
    MCP25625_hal_cmd( reg );
    MCP25625_hal_read( buffer, count );
    MCP25625_hal_cs( 1 );
    #endif
}

static void hw_write
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t count
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_write( CMD_WRITE, reg, buffer, count );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_WRITE );
    MCP25625_hal_cmd( reg );
    MCP25625_hal_write( buffer, count );
    MCP25625_hal_cs( 1 );
    #endif
}

static void hw_modify
(
        uint8_t reg,
        uint8_t mask,
        uint8_t *value
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    uint8_t data[2] = {mask, *value};
    MCP25625_hal_write(CMD_BIT_MODIFY, reg, &data[0], 2);
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_BIT_MODIFY );
    MCP25625_hal_cmd( reg );
    MCP25625_hal_cmd( mask );
    MCP25625_hal_cmd( *value );
    MCP25625_hal_cs( 1 );
    #endif
}

static void hw_tx
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t len
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_write( reg, reg, buffer, len );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( reg );
    MCP25625_hal_write( buffer, len );
    MCP25625_hal_cs( 1 );
    #endif
}

static void hw_rx
(
        uint8_t reg,
        uint8_t *buffer,
        uint8_t len
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_read( reg, reg, buffer, len);
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( reg );
    MCP25625_hal_read( buffer, len );
    MCP25625_hal_cs( 1 );
    #endif
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

int mcp25625_hw_init
(
        void
)
{
    reg.address = 0;
    reg.value = 0;

    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_init();
    mcp25625_pin_reset();
    MCP25625_hal_stb( 0 );
    #else
    MCP25625_hal_init();
    mcp25625_pin_reset();
    MCP25625_hal_cs( 1 );
    MCP25625_hal_stb( 0 );
    #endif

    return 0;
}

int mcp25625_pin_reset
(
        void
)
{
    MCP25625_hal_rst( 1 );
    //Delay_ms( 10 );
    vTaskDelay(pdMS_TO_TICKS(10));
    MCP25625_hal_rst( 0 );
    //Delay_ms( 10 );
    vTaskDelay(pdMS_TO_TICKS(10));
    MCP25625_hal_rst( 1 );
    //Delay_ms( 10 );
    vTaskDelay(pdMS_TO_TICKS(10));

    return 0;
}

int mcp25625_hw_reset
(
        void
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_cmd( CMD_RST );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_RST );
    MCP25625_hal_cs( 1 );
    #endif

    return 0;
}

int mcp25625_hw_rts_ctl
(
        TXB_t line
)
{   
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_cmd( CMD_RTS | MCP25625_RTS( line ) );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_RTS | MCP25625_RTS( line ) );
    MCP25625_hal_cs( 1 );
    #endif
    return 0;
}
can_fstatus mcp25625_hw_status
(
        void
)
{
    can_fstatus res;

    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_read( CMD_READ_STAT, CMD_READ_STAT, ( uint8_t* )&res, 1 );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_READ_STAT );
    MCP25625_hal_read( ( uint8_t* )&res, 1 );
    MCP25625_hal_cs( 1 );
    #endif

    return res;
}

void mcp25625_hw_rx_status
(
        rx_fstatus *status
)
{
    #if defined(__MIKROC_PRO_FOR_ESP32__)
    MCP25625_hal_read( CMD_RX_STAT, CMD_RX_STAT, ( uint8_t* )status, 1 );
    #else
    MCP25625_hal_cs( 0 );
    MCP25625_hal_cmd( CMD_RX_STAT );
    MCP25625_hal_read( ( uint8_t* )status, 1 );
    MCP25625_hal_cs( 1 );
    #endif
}

int mcp25625_hw_ctl_set
(
        void *value
)
{
    if( *( ( uint8_t* )value + 1 ) != 0x30 )
    {
        memcpy( ( void* )&reg, value, sizeof( reg_t ) );
        hw_write( reg.address, &reg.value, 1 );
    }
    else {
        memcpy( ( void* )&reg, value, sizeof( reg_t ) );
        hw_write( CTL_TXB( reg.buffer, reg.address ), &reg.value, 1 );
    }

    return 0;
}

int mcp25625_hw_ctl_update
(
        void *value
)
{
    if( *( ( uint8_t* )value + 1 ) != 0x30 )
    {
        memcpy( ( void* )&reg, value, sizeof( reg_t ) );
        hw_modify( reg.address, reg.mask, &reg.value );
    }
    else {
        memcpy( ( void* )&reg, value, sizeof( reg_t ) );
        hw_modify( CTL_TXB( reg.buffer, reg.address ), reg.mask, &reg.value );
    }

    return 0;
}

int mcp25625_hw_ctl_get
(
        void *result
)
{
    if( *( ( uint8_t* )result + 1 ) != 0x30 )
    {
        memcpy( ( void* )&reg, result, sizeof( reg_t ) );
        hw_read( reg.address, &reg.value, 1 );
    }
    else {

        memcpy( ( void* )&reg, result, sizeof( reg_t ) );
        hw_read( CTL_TXB( reg.buffer, reg.address ), &reg.value, 1 );
    }

    memcpy( result, &reg.value, 1 );

    return 0;
}

int mcp25625_hw_data_set
(
        TXB_t num,
        mcp25625_transfer *tx_data
)
{
    hw_tx( CMD_LOAD_TX + MCP25625_TX( num ), ( uint8_t* )tx_data, 13 );

    return 0;
}

int mcp25625_hw_data_get
(
        RXB_t num,
        mcp25625_transfer *rx_data
)
{
    hw_rx( CMD_READ_RX + MCP25625_RX( num ), ( uint8_t* )rx_data, 13 );

    return 0;
}

int mcp25625_hw_filter_set
(
        RXF_t reg,
        mcp25625_id *filter
)
{
    hw_write( MCP25625_FILTER( reg ), ( uint8_t* )filter, 4 );

    return 0;
}

int mcp25625_hw_mask_set
(
        RXB_t reg,
        mcp25625_id *mask
)
{
    hw_write( MCP25625_MASK( reg ), ( uint8_t* ) mask, 4 );

    return 0;
}

/*************** END OF FUNCTIONS *********************************************/