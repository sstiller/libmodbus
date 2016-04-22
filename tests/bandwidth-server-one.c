/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>
#include <default_mapping.h>

#if defined(_WIN32)
#define close closesocket
#endif

enum {
    TCP,
    RTU
};

int main(int argc, char *argv[])
{
    int s = -1;
    modbus_t *ctx = NULL;
    modbus_storage_backend_t *mb_storage_be;
  
    int rc;
    int use_backend;

     /* TCP */
    if (argc > 1) {
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
        } else if (strcmp(argv[1], "rtu") == 0) {
            use_backend = RTU;
        } else {
            printf("Usage:\n  %s [tcp|rtu] - Modbus client to measure data bandwith\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        use_backend = TCP;
    }

    mb_storage_be = modbus_default_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                   MODBUS_MAX_READ_REGISTERS, 0);
    if (mb_storage_be == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        return -1;
    }

    if (use_backend == TCP) {
        ctx = modbus_new_tcp("127.0.0.1", 1502, mb_storage_be);
        s = modbus_tcp_listen(ctx, 1);
        modbus_tcp_accept(ctx, &s);

    } else {
        ctx = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1, mb_storage_be);
        modbus_set_slave(ctx, 1);
        modbus_connect(ctx);
    }
    if (ctx == NULL) {
        fprintf(stderr, "Failed to allocate the ctx: %s\n",
                modbus_strerror(errno));
        return -1;
    }


    for(;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            modbus_reply(ctx, query, rc);
        } else if (rc  == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    modbus_default_mapping_free(mb_storage_be);
    if (s != -1) {
        close(s);
    }
    /* For RTU, skipped by TCP (no TCP connect) */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
