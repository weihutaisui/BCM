/***********************************************************************
 * <:copyright-BRCM:2016:DUAL/GPL:standard
 *
 *    Copyright (c) 2016 Broadcom
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
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bcmTag.h"
#include "bcm_crc.h"
#include "bcm_imgutil.h"

#ifdef COMBOIMG_DEBUG
#define DUMP_HEX(b, l) dump_hex(b, l)
#else
#define DUMP_HEX(b, l)
#endif /* COMBOIMG_DEBUG */

void dump_combo_header_tag(Comboimg_header_tag *header_tag)
{
    if (!header_tag)
        return;

    printf("--------------------------------------------------------------\n");
    printf("Combo image header tag:\n");
    printf("image count %u\n",header_tag->image_count);
    printf("--------------------------------------------------------------\n");
}

void dump_individual_image_tag(Comboimg_individual_img_tag *image_tag_ptr)
{
    if (!image_tag_ptr)
        return;

    printf("--------------------------------------------------------------\n");
    printf("Combo header, individual image tag:\n");
    printf("\tchip_id 0x%08x\n",image_tag_ptr->chip_id);
    image_tag_ptr->board_id[BOARD_ID_LEN-1]='\0';
    printf("\tboard_id %s\n",image_tag_ptr->board_id);
    printf("\timage_len %u\n",image_tag_ptr->image_len);
    printf("\timage_offset %u\n",image_tag_ptr->image_offset);
    printf("\timage_flags %u\n",image_tag_ptr->image_flags);
    printf("\tnext_tag_offset %u\n",image_tag_ptr->next_tag_offset);
    printf("\textended_image_header %u\n",image_tag_ptr->extended_image_header);
    printf("--------------------------------------------------------------\n");
}

void dump_hex(const char *buf, UINT32 len)
{
    UINT32 i;

    for (i = 0; i < len; i++)
        printf("%02X ", (unsigned char)buf[i]);

    printf("\n");
}

UINT32 bcmImg_rollToImage(const char *imageBuf __attribute__((unused)),
  UINT32 imageLen __attribute__((unused)))
{
    return 0;
}

static int combo_parsing_state = 0;

UBOOL8 bcmImg_IsValidCombo(void)
{
    return (combo_parsing_state == combo_done);
}

void bcmImg_ComboImageInit(void)
{
    combo_parsing_state = combo_init;
}

UBOOL8 bcmImg_ComboImageParsingDone(void)
{
    if (combo_parsing_state == combo_done || combo_parsing_state == fail
        || combo_parsing_state == not_combo)
        return TRUE;
    else
        return FALSE;
}

imgutil_ret_e bcmImg_ComboImageIdentify(const char *imageBuf, UINT32 imageLen)
{
    const char magicnum[] = COMBOIMG_MAGIC_NUM;
    DUMP_HEX(magicnum, COMBOIMG_MAGIC_NUM_LEN);
    DUMP_HEX(imageBuf, imageLen);

    if (imageLen < COMBOIMG_MAGIC_NUM_LEN)
    {
        return IMGUTIL_PROC_MORE_DATA;
    }

    if (!memcmp(imageBuf, magicnum, COMBOIMG_MAGIC_NUM_LEN))
        return IMGUTIL_PROC_FOUND;
    else
        return IMGUTIL_PROC_NOT_FOUND;
}

imgutil_ret_e bcmImg_ComboImageParseHeader(char *imageBuf, UINT32 imageLen,
  UINT32 *consumed, UINT32 *image_len, int *parsingState,
  Comboimg_header_tag *comboTagP, Comboimg_individual_img_tag *indvTagP)
{
    static Comboimg_header_tag header_tag_tmp, *header_tag_ptr;
    static Comboimg_individual_img_tag *image_tag_ptr;
    static Comboimg_individual_img_tag image_tag_tmp;
    UINT32 i = 0;

    //state variables must get reset
    static UINT32 offset_to_image = 0;
    static UINT32 stream_location = 0;
    static UINT32 header_len = 0;
    static char *combo_rcv_buf = 0;

    //todo: fix endianity to support little endian deployment
    //todo: what if first packet does not have complete header_tag
    // deal with extended header header and image tag

    //printf("in function combo parser\n");
    *consumed = 0;

    while (*consumed < imageLen)
    {
        switch (combo_parsing_state)
        {
        case combo_init:
            //reset all state variables
            stream_location = 0;
            offset_to_image = 0;
            combo_rcv_buf = 0;
            //assuming here that magic_number received in whole
            if (bcmImg_ComboImageIdentify(imageBuf, imageLen) != IMGUTIL_PROC_FOUND)
            {
                combo_parsing_state = not_combo;
                *parsingState = combo_parsing_state;
                return IMGUTIL_PROC_OK;
            }
            else
            {
                printf("found combo magic number, setting state:in_header\n");
                combo_parsing_state = get_header_tag;
            }
            //fall-through
        case get_header_tag:
            {
                //copy just what needed to get header tag
                UINT32 need = sizeof(Comboimg_header_tag) - stream_location;
                printf("in get header, need %d more to header tag\n",need);
                if (need > imageLen)
                {
                    memcpy(&header_tag_tmp,imageBuf, imageLen);
                    stream_location += imageLen;
                    *consumed += imageLen;
                }
                else
                {
                    printf("copying %d bytes to header_tmp for imageBuf\n",need);
                    memcpy(&header_tag_tmp,imageBuf, need);
                    stream_location += need;
                    *consumed += need;
                    header_len = be32toh(header_tag_tmp.header_len);
                    printf("flipped image len: %u, allocating rcvbuf\n",header_len);

                    //allocate memory pad, copy it and whatever remaning in imageBuf
                    //to pad, move state
                    combo_rcv_buf = calloc(header_len,1);
                    memcpy(combo_rcv_buf,&header_tag_tmp,sizeof(Comboimg_header_tag));
                    //memcpy(combo_rcv_buf + sizeof(Comboimg_header_tag)
                    //    ,imageBuf+need, imageLen-need);
                    //continue to debug here
                    combo_parsing_state = buffering_complete_header;
                    printf("got header_tag.\n");
                    printf("header_tag.header_len: %d\n",header_tag_tmp.header_len);
                    printf("stream location %d\n",stream_location);
                    printf("consumed %d\n",*consumed);

                    header_tag_ptr = (Comboimg_header_tag*)combo_rcv_buf;
                    printf("header_tag_ptr->header_len: %d\n",header_tag_ptr->header_len);

                    DUMP_HEX(combo_rcv_buf,sizeof(Comboimg_header_tag));
                }
            }
            break;
        case buffering_complete_header:
            {
                //must fix endian for reverse singular image header
                //buffering is required because receiving is not promised to be
                //aligned to combo header structure
                UINT32 need = header_len - stream_location;
                printf("in buffering complete header, need %d\n",need);
                if (need > imageLen - *consumed)
                {
                    //need to buffer more
                    memcpy(combo_rcv_buf + stream_location, imageBuf + *consumed
                        ,imageLen - *consumed);
                    stream_location += (imageLen - *consumed);
                    *consumed  += (imageLen - *consumed);
                }
                else
                {
                    memcpy(combo_rcv_buf + stream_location, imageBuf + *consumed
                        , need);
                    stream_location += need;
                    *consumed += need;

                    //validate CRC
                    UINT32 calculated_crc = CRC_INITIAL_VALUE;
                    UINT32 received_crc = be32toh(header_tag_ptr->header_crc);

                    header_tag_ptr->header_crc = 0 ;
                    DUMP_HEX(combo_rcv_buf, header_len);
                    calculated_crc = crc_getCrc32((unsigned char*)combo_rcv_buf,
                        header_len , calculated_crc);

                    if (calculated_crc != received_crc)
                    {
                        printf("received CRC is 0x%08X\n", received_crc);
                        printf("calculated CRC is 0x%08X\n", calculated_crc);
                        combo_parsing_state = fail;
                        free(combo_rcv_buf);
                        printf("Error in combo header CRC\n");
                        *parsingState = combo_parsing_state;
                        return IMGUTIL_PROC_INVALID_IMG;
                    }
                    else
                        printf("combo header CRC validated\n");

                    //reverse header endianess
                    UINT32* curr = 0;
                    printf("flipping header_tag endian\n");
                    for (curr = &header_tag_ptr->header_len;
                        curr <= &header_tag_ptr->extended_combo_header; curr++)
                    {
                        *curr = be32toh(*curr);
                    }

                    //todo: flip the extended header TLV entries

                    //flip endian for individual image headers
                    image_tag_ptr = (Comboimg_individual_img_tag*) (
                            (char*)&header_tag_ptr->extended_combo_header
                            + header_tag_ptr->next_tag_offset);

                    for (i = 0; i < header_tag_ptr->image_count; i++)
                    {
                        UINT32* curr = 0;

                        for(curr = &image_tag_ptr->chip_id;
                            curr <= &image_tag_ptr->extended_image_header; curr++)
                        {
                            *curr = be32toh(*curr);
                        }
                        //todo: flip the extended header TLV entries

                        image_tag_ptr = (Comboimg_individual_img_tag*) (
                            (char*)&image_tag_ptr->extended_image_header
                            + image_tag_ptr->next_tag_offset);
                    }

                    // in case of failue set state and  free(combo_rcv_buf);
                    // must deal with extended header here
                    combo_parsing_state = in_image;
                    printf("finished buffering combo_header, total received %d\n",stream_location);
                    dump_combo_header_tag(header_tag_ptr);
                }
            }
            break;
        case in_image:
            image_tag_ptr = (Comboimg_individual_img_tag*) (
                    (char*)&header_tag_ptr->extended_combo_header
                    + header_tag_ptr->next_tag_offset);

            for (i = 0; i<header_tag_ptr->image_count; i++)
            {
                printf("image %d\n",i);
                dump_individual_image_tag(image_tag_ptr);
                DUMP_HEX((char*)image_tag_ptr, sizeof(Comboimg_individual_img_tag));
                //todo: need to consider extended header length
                //*consumed += sizeof(Comboimg_individual_img_tag);
                printf("searching an image for %8X:\n", CHIP_FAMILY_ID_HEX);
                printf("header chipid value %8x: \n", image_tag_ptr->chip_id);
                if (image_tag_ptr->chip_id == CHIP_FAMILY_ID_HEX)
                {
                    printf("found chip_id match, calculating offset\n");
                    printf("image offset in header 0x%08x: \n", image_tag_ptr->image_offset);
                    //could be also image_offset -header length
                    offset_to_image = image_tag_ptr->image_offset - stream_location;
                    printf("offset to image: %u\n", offset_to_image);
                    *image_len = image_tag_ptr->image_len;
                    printf("image len is: %d\n",*image_len);
                    combo_parsing_state = rolling;
                    memcpy(&image_tag_tmp, image_tag_ptr, sizeof(Comboimg_individual_img_tag));
                    break;
                }
                image_tag_ptr = (Comboimg_individual_img_tag*) (
                    (char*)&image_tag_ptr->extended_image_header + image_tag_ptr->next_tag_offset);
                //todo: need to consider extended header length
                //dataptr += *used; //(char*)&image_tag[i].extended_image_header + image_tag[i].next_tag_offset;
            }
            if (combo_parsing_state != rolling)
            {
                printf("didn't find matching image\n");
                combo_parsing_state = fail;
                return IMGUTIL_PROC_NOT_FOUND;
            }
            break;
        case rolling:
            //printf("in rolling, offset %u\n",offset_to_image);
            //printf("imageLen value: %u\n",imageLen);
            //printf("consumed value: %u\n",*consumed);

            if (offset_to_image > imageLen - *consumed)
            {
                //printf("image starts next packets\n");
                offset_to_image -= (imageLen - *consumed);
                *consumed += (imageLen - *consumed);
                stream_location += (imageLen - *consumed);
            }
            else
            {
                printf("image starts in this packet\n");
                *consumed += offset_to_image;
                stream_location += offset_to_image;
                printf("rolling done. stream_location %d, consumed %d\n",stream_location,*consumed);
                combo_parsing_state = combo_done;
                memcpy(comboTagP, combo_rcv_buf, sizeof(Comboimg_header_tag));
                memcpy(indvTagP, &image_tag_tmp, sizeof(Comboimg_individual_img_tag));
                //free rcv_buffer_here
                free(combo_rcv_buf);
                *parsingState = combo_parsing_state;
                return IMGUTIL_PROC_OK;
            }
            break;
        }
    }

    *parsingState = combo_parsing_state;
    return IMGUTIL_PROC_OK;
}
