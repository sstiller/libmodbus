/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>
#include <default_mapping.h>

int main(void)
{
    int s = -1;
    modbus_t *ctx;
    modbus_storage_backend_t *mb_storage_be;

    mb_storage_be = modbus_default_mapping_new(500, 500, 500, 500);
    if (mb_storage_be == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        return -1;
    }
    ctx = modbus_new_tcp("127.0.0.1", 1502, mb_storage_be);
    if (ctx == NULL) {
        fprintf(stderr, "Failed to allocate the ctx: %s\n",
                modbus_strerror(errno));
        return -1;
    }
    /* modbus_set_debug(ctx, TRUE); */


    s = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &s);

    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;

        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc);
        } else if (rc == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (s != -1) {
        close(s);
    }
    modbus_default_mapping_free(mb_storage_be);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
