/***********************************************************************
 *
 * <:copyright-BRCM:2011:DUAL/GPL:standard
 * 
 *    Copyright (c) 2011 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 *
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <simio_def_common.h>
#include <simcard_ctl_api.h>

static void sim_app_insert (SIMIO_ID_t sim_id, void *app_ctx)
{
    printf("SIM ID %d Inserted\n", sim_id);
}

static void sim_app_removed (SIMIO_ID_t sim_id, void *app_ctx)
{
    printf("SIM ID %d Removed\n", sim_id);
}

static void sim_app_signal (SIMIO_ID_t sim_id, SIMIO_SIGNAL_t sim_sig, void *app_ctx)
{
    printf("SIM ID %d Signal %d \n", sim_id, sim_sig);
}

static void sim_app_change (SIMIO_ID_t sim_id, void *app_ctx, int change)
{
    printf("SIM ID %d Change %d \n", sim_id, change);
}

static bcm_simio_app_cbs_t sim_app_cb = {
    sim_app_insert,
    sim_app_removed,
    sim_app_signal,
    sim_app_change
};

static void dump_data(uint8_t *data, size_t size, bool as_char)
{
    int i, j, k=0;
    printf("Dump data %d bytes: \n", (unsigned int)size);

    for (i = 0, j = 0; i < size; i++, j++)
    {
        j %=16;
        if (j == 0)
            printf("%d : ", k++);
        
        if (!as_char)
            printf("0x%02x%s", data[i], j == 15 ? "\n" : " ");
        else
            printf("%c%s", data[i], j == 15 ? "\n" : "");

    }

    printf("\n====== End of data ========\n");
}

int main(int argc, char **argv)
{
    int rc = 0;
    void *lib_ctx = NULL;
    uint8_t data[300] = {};
    uint32_t is_online = 0;
    size_t len = 300;
    int fd;
    uint16_t filesize = 0;
    void *file_data = NULL;

    rc = bcm_simio_lib_init();
    if (rc)
    {
        printf("Failed to initialized library %d \n", rc);
        goto Exit;
    }

    rc = bcm_simio_lib_register_app(SIMIO_ID_0, NULL, &sim_app_cb, &lib_ctx);
    if (rc)
    {
        printf("Failed to register application %d \n", rc);
        goto Exit1;
    }

    rc = bcm_simio_lib_activate(lib_ctx, &len, data);
    if (rc)
    {
        printf("Failed to activate sim %d \n", rc);
        goto Exit2;
    }

    dump_data(data, len, 0); 

    rc = bcm_simio_lib_is_online(lib_ctx, &is_online);
    if (rc)
    {
        printf("Failed to get sim online status%d \n", rc);
        goto Exit2;
    }
    printf("SIM is %sonline\n", is_online ? "": "not ");

    printf("Going to select 3f00->7F20->6f46\n");

    fd = bcm_simio_lib_open(lib_ctx, 0x3f00, NULL, NULL);
    if (fd < 0)
    {
        printf("Failed to open Master file result %d \n", fd);
        goto Exit2;
    }

    fd = bcm_simio_lib_change_file(fd, 0x7f20, NULL, NULL);
    if (fd < 0)
    {
        printf("Failed to change to 0x7f20 file result %d \n", fd);
        goto Exit2;
    }

    fd = bcm_simio_lib_change_file(fd, 0x6f46, NULL, NULL);
    if (fd < 0)
    {
        printf("Failed to change to 0x6f46 file result %d \n", fd);
        goto Exit2;
    }

    len = 300;

    rc = bcm_simio_lib_get_select_response(fd, data, &len);
    if (rc)
    {
        printf("Failed to get select respoince %d \n", rc);
        goto Exit3;
    }
   
    printf("Get select responce \n");
    dump_data(data, len, 0); 

    filesize = data[2] << 8 | data[3];

    printf("Get filesize %u \n", filesize);

    file_data = malloc(filesize);
    if (!file_data)
    {
        printf("Failed to alloc %u bytes\n", filesize);
        goto Exit3;
    }

    rc = bcm_simio_lib_read(fd, file_data, 0, filesize, 0);
    if (rc > 0)
    {
        dump_data(file_data, rc, 1);
    }
    else
    {
        printf("Failed to read %u bytes %d\n", filesize, rc);
    }

    if (file_data)
        free(file_data);

Exit3:

    bcm_simio_lib_close(fd);

Exit2:
    bcm_simio_lib_unregister_app(lib_ctx);
    lib_ctx = NULL;

Exit1:
    bcm_simio_lib_uninit();

Exit:
    exit(rc);
}


