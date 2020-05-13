/**
 * @file uart.h
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _UART_H_
#define _UART_H_

#include <semaphore.h>
#include <stddef.h>

/* UART Buffer lengths */
#define UART_IBLEN      1024
#define UART_OBLEN      1024

#define UART_BAUD       115200  /**< Default console baud rate.         */

#define UART_CSREG      0x3F201000

/**
 * UART control block
 */
struct uart
{
    /* Pointers to associated structures */
    volatile void *csr;                  /**< Control & status registers         */

    /* UART input fields */
    uchar iflags;               /**< Input flags                        */
    semaphore isema;            /**< Count of input bytes ready         */
    ushort istart;              /**< Index of first byte                */
    ushort icount;              /**< Bytes in buffer                    */
    uchar in[UART_IBLEN];       /**< Input buffer                       */

    /* UART output fields */
    uchar oflags;               /**< Output flags                       */
    semaphore osema;            /**< Count of buffer space free         */
    ushort ostart;              /**< Index of first byte                */
    ushort ocount;              /**< Bytes in buffer                    */
    uchar out[UART_OBLEN];      /**< Output buffer                      */
    volatile bool oidle;        /**< UART transmitter idle              */
    spinlock_t olock;           /**< Spinlock to guard counters         */
};

extern struct uart serial_port;

#endif                          /* _UART_H_ */
