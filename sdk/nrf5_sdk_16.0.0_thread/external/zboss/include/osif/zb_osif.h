/* ZBOSS Zigbee 3.0
 *
 * Copyright (c) 2012-2018 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
 * All rights reserved.
 *
 *
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
PURPOSE: Main header for OS and platform depenednt stuff
*/

#ifndef ZB_OSIF_H
#define ZB_OSIF_H 1


/**
 @internal

 \addtogroup ZB_OSIF */
/*! @{ */

/**
@par OS/HW abstraction
OS/HW platform abstraction is necessary to achieve high portability.
C language tool to separate abstraction layer is C preprocessor.

Main idea is to avoid number of ifdefs related to portability in the code, and
decrease number of ifdefs in the header files not related to the OS abstraction
layer.

Platform abstraction is implemented as C functions placed into OS abstraction
layers and platform-dependent global typedefs and definitions placed into header
files.  All platform-related stuff is in osif/ directory.  Global definitions
and typedefs can be used anywhere - that is why on the architecture picture OS
abstraction layer depicted as global.

Following things are platform-dependent:
*   typedefs for base types (8-bit controller vs 32-bit Linux device)
*   definitions for different 8051 compilers (SDCC and Keil)
*   transceiver i/o (interrupts handling for 8051 vs file i/o in Linux);
*   wait for i/o (device sleep for 8051, wait in select() in Linux)
*   trace i/o (UART for 8051, file in Linux);
*   MAC traffic dump (UART for 8051, file in Linux);
*   Timer (8051 timer at device, select() timeout in Linux)

 */



#ifdef __IAR_SYSTEMS_ICC__
#ifndef ZB_IAR
#define ZB_IAR
#endif
#endif
 /**
OSIF platform selection. One of pre-defined platform should be selected in
zb_config.h configurations
*/
#if defined(ZB_PLATFORM_LINUX) && !defined(ZB_WINDOWS)  /*OSIF for Linux*/
  #include "zb_osif_unix.h"
#elif defined ZB_TI_CC13XX
  #include "zb_osif_ti_cc13xx.h"

#elif (defined ZB_PLATFORM_NRF52)
  #include "zb_osif_nrf52.h"

#else
  #error Port me!
#endif

#include "zb_ringbuffer.h"

#ifndef ZB_PLATFORM_INIT
#error 1
//#define ZB_PLATFORM_INIT()
#endif

#ifndef DOXYGEN

#if   defined ZB_TI_CC13XX

#define MAIN() int zb_ti_cc13xx_main(void)
#define FAKE_ARGV
#define ARGV_UNUSED char **argv = NULL; ZVUNUSED(argv)
#define MAIN_RETURN(v) return (v)

#elif !defined KEIL

#define MAIN() int main(int argc, char *argv[])
#define FAKE_ARGV
#define ARGV_UNUSED ZVUNUSED(argc) ; ZVUNUSED(argv)
#define MAIN_RETURN(v) return (v)

#else

/* 04/03/2018 EE CR:MINOR Am I right this is for Keil?
   If main() returns value, why MAIN_RETURN(v) does not return?
 */
#define MAIN() int main(void)
#define FAKE_ARGV char **argv = NULL
#define ARGV_UNUSED
#define MAIN_RETURN(v)

#endif  /* KEIL */

#endif /* ndef DOXYGEN */

#ifndef ZB_SET_TRACE_TRANSPORT
/*
 * If platform supports choosing transport at runtime
 * (i.e. Telink can choose between UART and USB debug interface)
 * define this macro.
 * Note that this is intended to be called before ZB_TRACE_INIT is called as
 * only one interface will be configured and the rest of them will be left as is
 * so that it would be usable from non-Zigbee purposes (application) if needed.
 */
#define ZB_SET_TRACE_TRANSPORT(transport)
#endif

#if !defined ZB_KICK_HW_WATCHDOG
#define ZB_KICK_HW_WATCHDOG()
#endif

/*! @} */


/* common osif API */
zb_uint32_t zb_random_seed(void);
zb_uint32_t zb_get_utc_time(void);

zb_uint32_t osif_get_time_ms(void);

/* note: that api is only for some platforms */
zb_ret_t osif_set_transmit_power(zb_uint8_t channel, zb_int8_t power);
void osif_set_default_trasnmit_powers(zb_int8_t *tx_powers);

#if defined ZB_MACSPLIT_TRANSPORT_SERIAL
void zb_osif_serial_transport_init();
void zb_osif_serial_transport_put_bytes(zb_uint8_t *buf, zb_short_t len);
#endif

/*! @cond ZBOSS_INTERNAL */
/*! \addtogroup uart */
/*! @{ */

#if defined ZB_HAVE_SERIAL || defined DOXYGEN

/* Serial interface (trace, traffic dump, serial transport) */

/**
   Initialize UART low level.

   If ZBOSS uses UART for trace or traffic dump, it calls zb_osif_serial_init()
   itself.
   If UART is used by application, application must call zb_osif_serial_init().
 */
void zb_osif_serial_init(void);

/**
   Setup callback to be called when single byte reecived over UART

   @param hnd user's rx callback
 */
void zb_osif_set_uart_byte_received_cb(void (*hnd)(zb_uint8_t));

/**
   Set user's buffer to be used by UART TX logic.

   ZBOSS normally uses its internal UART buffer. The buffer is not too big -
   about 200 bytes which is enough for its usage by ZBOSS (trace).
   Some applications needs large io buffers. So declare there type placeholder for use ringbuffer zb_byte_array_t;
   by default application will set user tx buffer to stack tx buffer and access it through pointer;
   User can override this pointer to use it's own serial buffer.

   @param buf_ptr user's buffer
   @param capacity buffer capacity
 */
void zb_osif_set_user_io_buffer(zb_byte_array_t *buf_ptr, zb_ushort_t capacity);

/**
   Set user callback that will be called from uart IRQ handler.
   Replaces zboss uart irq handler.

   Note: that call is to be used when application initializes and uses UART
   itself.
   Do not use this call if use ZBOSS serial port API.
   Call to that functions disables ZBOSS debug trace and traffic dump.
*/
void zb_osif_set_uart_user_irq_handler(void (*irq_hnd)());

/**
   Prepare UART to a sleep. Shall be invoked before putting ZBOSS to a sleep.
   Perform OS-dependent actions.
*/
void zb_osif_uart_sleep(void);

/**
   Restore UART from a sleep. Shall be invoked after wake up.
   Perform OS-dependent actions.
*/
void zb_osif_uart_wake_up(void);

#endif
#if defined ZB_BINARY_TRACE || defined ZB_HAVE_SERIAL || defined ZB_TRACE_OVER_USART || defined DOXYGEN || defined ZB_NSNG

/**
   TX data over UART

   Put data to internal buffer to be transmitted over UART.
   It is guaranteed that all data will be sent.
   Block is no space in the buffer waiting for previous TX complete.

   Note: this is low level routine. Its direct usage may conflict with ZBOSS
   debug trace and traffic dump (if enabled).

   @param buf data buffer
   @param len data length.
 */
void zb_osif_serial_put_bytes(zb_uint8_t *buf, zb_short_t len);
#endif

/*! @} */
/*! @endcond */


#ifdef ZB_TRACE_OVER_SIF
void zb_osif_sif_put_bytes(zb_uint8_t *buf, zb_short_t len);
void zb_osif_sif_init(void);
zb_void_t zb_osif_sif_debug_trace(zb_uint8_t param);
#endif

#ifdef ZB_HAVE_FILE
/* File  */
zb_uint32_t zb_osif_get_file_size(zb_char_t *name);
zb_bool_t zb_osif_check_file_exist(const zb_char_t *name, const zb_uint8_t mode);
void zb_osif_file_copy(const zb_char_t *name_src, const zb_char_t *name_dst);
zb_osif_file_t *zb_osif_file_open(const zb_char_t *name, const zb_char_t *mode);
zb_osif_file_t *zb_osif_init_trace(zb_char_t *name);
zb_osif_file_t *zb_osif_file_stdout(void);
void zb_osif_file_close(zb_osif_file_t *f);
int zb_osif_file_remove(const zb_char_t *name);
void zb_osif_trace_printf(zb_osif_file_t *f, zb_char_t *format, ...);
void zb_osif_trace_vprintf(zb_osif_file_t *f, zb_char_t *format, va_list arglist);
void zb_osif_trace_lock(void);
void zb_osif_trace_unlock(void);
zb_osif_file_t *zb_osif_init_dump(zb_char_t *name);
int zb_osif_file_read(zb_osif_file_t *f, zb_uint8_t *buf, zb_uint_t len);
int zb_osif_file_write(zb_osif_file_t *f, zb_uint8_t *buf, zb_uint_t len);
int zb_osif_file_flush(zb_osif_file_t *f);
int zb_osif_file_seek(zb_osif_file_t *f, zb_uint32_t off, zb_uint8_t mode);
int zb_osif_file_truncate(zb_osif_file_t *f, zb_uint32_t off);
void zb_osif_trace_get_time(zb_uint_t *sec, zb_uint_t *msec);
zb_osif_file_t *zb_osif_popen(zb_char_t *arg);

enum zb_file_path_base_type_e
{
  ZB_FILE_PATH_BASE_ROMFS_BINARIES,    /* ROM FS */  /* elf binaries, etc */
  ZB_FILE_PATH_BASE_MNTFS_BINARIES,    /* RW FS */   /* prod config, etc */
  ZB_FILE_PATH_BASE_MNTFS_USER_DATA,   /* RW FS */   /* nvram. etc */
  ZB_FILE_PATH_BASE_RAMFS_UNIX_SOCKET, /* RAM FS */
  ZB_FILE_PATH_BASE_RAMFS_TRACE_LOGS,  /* RAM FS */
  ZB_FILE_PATH_BASE_RAMFS_TMP_DATA,    /* RAM FS */

  ZB_FILE_PATH_BASE_MAX_TYPE
};

#define ZB_MAX_FILE_PATH_SIZE 256

#ifdef ZB_FILE_PATH_MGMNT
#ifndef ZB_FILE_PATH_MAX_TYPES
#define ZB_FILE_PATH_MAX_TYPES (ZB_FILE_PATH_BASE_MAX_TYPE - 1)
#endif

typedef struct zb_file_path_base_type_s
{
  zb_bool_t declared;
  char      base[ZB_MAX_FILE_PATH_SIZE];
} zb_file_path_base_type_t;

void zb_file_path_init(void);
zb_ret_t zb_file_path_declare(zb_uint8_t base_type, const char *base);
const char* zb_file_path_get(zb_uint8_t base_type, const char *default_base);
void zb_file_path_get_with_postfix(zb_uint8_t base_type, const char *default_base, const char *postfix, char *file_path);
#define ZB_FILE_PATH_GET(base_type, default_base) \
  zb_file_path_get(base_type, default_base)
#define ZB_FILE_PATH_GET_WITH_POSTFIX(base_type, default_base, postfix, file_path) \
  zb_file_path_get_with_postfix(base_type, default_base, postfix, file_path)
#else
#define ZB_FILE_PATH_GET(base_type, default_base) default_base
#define ZB_FILE_PATH_GET_WITH_POSTFIX(base_type, default_base, postfix, file_path) \
  { \
    zb_uint_t sn_ret; \
    sn_ret = snprintf(file_path, ZB_MAX_FILE_PATH_SIZE, "%s", (default_base postfix)); \
    ZB_ASSERT(sn_ret > 0); \
    ZB_ASSERT(sn_ret < ZB_MAX_FILE_PATH_SIZE); \
  }
#endif  /* ZB_FILE_PATH_MGMNT */
#endif /* ZB_HAVE_FILE */

/*! \addtogroup zb_platform */
/*! @{ */

/**
   Platform dependent soft reset
*/
#if defined ZB_PLATFORM_CORTEX_M3 || defined ZB_PLATFORM_CORTEX_M4 || \
    defined ZB_PLATFORM_LCGW_REMOTE_SOLUTION
zb_void_t zb_reset_silently(zb_uint8_t param);
#endif /*#if defined ZB_PLATFORM_CORTEX_M3 || defined ZB_PLATFORM_CORTEX_M4*/
zb_void_t zb_reset(zb_uint8_t param);
zb_void_t zb_syslog_msg(const zb_char_t *msg);

/*! @} */

/**
 *
 *  @brief Get stack current parameters.
 *
 *  @param s_head      [OUT] - pointer to stack head.
 *  @param s_size      [OUT] - current size of stack.
 *  @param s_direction [OUT] - stack growing direction (ZB_TRUE - UP, ZB_FALSE - DOWN).
 */
void zb_osif_get_stack(zb_uint8_t **s_head, zb_uint32_t *s_size, zb_uint8_t *s_direction);

void osif_set_reset_at_crash(void);
void osif_handle_crash(void);

#if defined ZB_USE_NVRAM || defined doxygen
/**
 * @brief osif NVRAM initializer
 */
void zb_osif_nvram_init(const zb_char_t *name);

/**
 * @brief Get NVRAM page length
 *
 * @return NVRAM page length
 */
zb_uint32_t zb_get_nvram_page_length(void);

/**
 * @brief Get NVRAM page count
 *
 * @return NVRAM page count
 */
zb_uint8_t zb_get_nvram_page_count(void);

/**
 * @brief Read from NVRAM directly, by address
 * Read some bytes from NVRAM use address
 *
 * @param address - NVRAM address
 * @param len - count bytes from read data
 * @param buf - (in) buffer for contains read data
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_read_memory(zb_uint32_t address, zb_uint16_t len, zb_uint8_t *buf);

/**
 * @brief Read from NVRAM page
 * Read some bytes from NVRAM
 *
 * @param page - NVRAM page
 * @param pos - Start position
 * @param buf - (in) buffer for contains read data
 * @param len - count bytes from read data
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_read(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len );

/**
 * @brief Read from NVRAM page with test
 * Read some bytes from NVRAM with test contents.
 * If all byte equal 0xFF then return RET_ERROR
 * Exists not for all platforms.
 *
 * @param page - NVRAM page
 * @param pos - Start position
 * @param buf - (in) buffer for contains read data
 * @param len - count bytes from read data
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_read_test(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len );

/**
 * @brief Write from NVRAM page
 * Write some bytes to NVRAM
 *
 * @param page - NVRAM page
 * @param pos - Start position
 * @param buf - buffer contains data for write
 * @param len - count bytes for write data
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_write(zb_uint8_t page, zb_uint32_t pos, void *buf, zb_uint16_t len );

/**
 * @brief Write to NVRAM directly, by address
 * Write bytes into NVRAM use address
 *
 * @param address - NVRAM address
 * @param len - count bytes for write data
 * @param buf - (in) buffer contains data to write
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_write_memory(zb_uint32_t address, zb_uint16_t len, zb_uint8_t *buf);

/**
 * @brief Erase NVRAM directly, by address
 *
 * @param address - NVRAM address
 * @param len - count bytes for erase, aligned
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_erase_memory(zb_uint32_t address, zb_uint32_t len);


/**
 * @brief Erase NVRAM page
 * Fill NVRAM page by 0xFF.
 *
 * @param page - page index
 *
 * @return RET_OK if success or code error
 */
zb_ret_t zb_osif_nvram_erase_async(zb_uint8_t page);

void zb_osif_nvram_wait_for_last_op(void);

/**
 * @brief Flush NVRAM page
 * Flish NVRAM page to file or NVRAM.
 * Different to hardware device
 *
 */
void zb_osif_nvram_flush(void);

#endif

#if defined ZB_PROFILE_STACK_USAGE
/* Both functions are platform-specific */

/**
 * @brief Prepare stack profiler.
 * Fill stack area with a predefined pattern.
 *
 */
zb_void_t zb_stack_profiler_pre(void);

/**
 * @brief Get stack usage by previously called function.
 *
 * @return Count of bytes in stack area used
 */
zb_uint16_t zb_stack_profiler_usage(void);

#endif  /* ZB_PROFILE_STACK_USAGE */


#ifdef ZB_USE_SLEEP
zb_uint32_t zb_osif_sleep(zb_uint32_t sleep_tmo);
zb_void_t zb_osif_wake_up(void);
#endif

#ifdef ZB_PRODUCTION_CONFIG

/* Check whether production configuration block is present in memory */
zb_bool_t zb_osif_production_configuration_check_presence(void);

zb_bool_t zb_osif_production_configuration_check_header(void);

/* Read data from production configuration header
 *
 * @param prod_cfg_hdr - pointer to production configarution header
 * @param hdr_len - size of production configarution header
 *
 * @return RET_OK is success, RET_ERROR otherwise
 */
zb_ret_t zb_osif_production_configuration_read_header(zb_uint8_t *prod_cfg_hdr, zb_uint16_t hdr_len);

/* Read data from production configuration block
 *
 * @param buffer - buffer to read into
 * @param len - length of block to read
 * @param offset - offset to read from
 *
 * @return
 */
zb_ret_t zb_osif_production_configuration_read(zb_uint8_t *buffer, zb_uint16_t len, zb_uint16_t offset);

/* Set transmit power of radio on selected channel
 *
 * @param channel - channle on which radio applies new transmit power
 * @param power - transmit power in dBm
 *
 * return RET_OK if power was set successfully, RET_ERROR otherwise
 */
zb_ret_t zb_osif_set_transmit_power(zb_uint8_t channel, zb_int8_t power);

#endif

/* Wait for a given number of empty cycle iterations. Timeout of 1 iteration is platform-specific
 *
 * @param count - number of empty wait cycles
 */
void zb_osif_busy_loop_delay(zb_uint32_t count);


/* Get OSIF timer counter value in microseconds
 *
 * @return Timer counter value in microseconds
 */
zb_uint16_t zb_osif_get_timer_reminder(void);

#endif /* ZB_OSIF_H */
