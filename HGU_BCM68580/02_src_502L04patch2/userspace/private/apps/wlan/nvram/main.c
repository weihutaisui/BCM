/***********************************************************************
 *
 *  Copyright (c) 2005  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2005:proprietary:standard
 *
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 *
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 *
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 ************************************************************************/

/*
 * Frontend command-line utility for Linux NVRAM layer
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcmnvram.h>
#include <typedefs.h>
#include <wlcsm_lib_api.h>
#include <unistd.h>
#include <ctype.h>


#define TEMP_KERNEL_NVRM_FILE "/var/.temp.kernel.nvram"
#define PRE_COMMIT_KERNEL_NVRM_FILE "/var/.kernel_nvram.setting.prec"
#define TEMP_KERNEL_NVRAM_FILE_NAME "/var/.kernel_nvram.setting.temp"
#define MFG_NVRAM_FILE_NAME_PATH "/mnt/nvram/nvram.nvm"
#define NVRAM_LINE_MAX (1024)
#define STR_MAX_LEN(a,b) (a>b?a:b)
#define ARG_VAL (**p_argv)
char g_line_buffer[NVRAM_LINE_MAX];
char *g_buf=NULL;

static int
isnumber(const char *src)
{
    char *iter = (char *)src;
    while (*iter) {
        if (! isdigit(*iter))
            return 0;
        iter++;
    }
    return 1;
}

#define NEXT_ARG(argc, argv) do { (argv)++; if (--(argc) <= 0 || !*(argv)) usage(NULL); } while (0)
#define ADVANCE_ARGCV(argc, argv,step) do { (argv)+=step; (argc)-=step; } while (0)
#define NEXT_IS_NUMBER(argc, argv) do { if (((argc) - 1 <= 0) ||!*(argv) || !isnumber((argv)[1])) { \
		usage(NULL); } } while (0)
#define NEXT_IS_VALID(argc) do { if ((argc - 1 <= 0)) usage(NULL); }  while (0)

enum {
    NVRAM_NORMAL_CMD=0,
    NVRAM_HIDDEN_CMD
};

void usage(char *cmd);
typedef int (*NVRAM_CMD_FUNC)(int *argc,char ***p_argv);

typedef struct nvram_cmd_item {
    char *cmd;
    char *prompt;
    char is_hidden_cmd;
    NVRAM_CMD_FUNC func;
    char align_tab;
} NVRAM_CMD_ITEM;


#ifdef NAND_SYS

/**** following functions are for kernel nvram handling ****
* The defaul kernel file * will be /data/.kernel_nvram.setting, when build image, the
* file kernel_nvram.setting under nvram will be burned to image and be available under
* /data directory after board bootup. when "nvram set" is used, the variable will save
* to a PRE_COMMIT_KERNEL_NVRM_FILE and then saved back to KERNEL_NVRAM_FILE_NAME when
* "nvram commit" is issued.
*/

/* internal function to update nvram in the existing file  */
static int  _assign_value(FILE *fp,FILE *to_fp,char *nm,char*vl,int is_commit)
{
    int is_handled=0,has_change=0;
    char *name,*value;
    if(!vl) vl="*DEL*";
    while(fgets(g_line_buffer,NVRAM_LINE_MAX,fp)!=NULL) {
        if(g_line_buffer[0]=='#') continue;
        value=g_line_buffer;
        name=strsep(&value,"=");
        if(name && value) {
            name=wlcsm_trim_str(name);
            value=wlcsm_trim_str(value);
            if(!strncmp(name,nm,STR_MAX_LEN(strlen(name),strlen(nm)))) {
                is_handled=1;
                if(is_commit && !strncmp(vl,"*DEL*",5)) {
                    has_change=1;
                    if(is_commit) continue;
                } else if(strncmp(value,vl,STR_MAX_LEN(strlen(value),strlen(vl)))) {
                    has_change=1;
                    sprintf(g_line_buffer,"%s=%s\n",nm,vl);
                }
            } else
                sprintf(g_line_buffer,"%s=%s\n",name,value);
            fputs(g_line_buffer,to_fp);
        }
    }
    if(!is_handled) {
        if(!(is_commit && !strncmp(vl,"*DEL*",5))) {
            sprintf(g_line_buffer,"%s=%s\n",nm,vl);
            fputs(g_line_buffer,to_fp);
            has_change=1;
        }
    }
    return has_change;
}

/**
 * internal function to set nvram value into file, either permenant
 * file or a temporary file based on b_commit
 */
static int nvram_kset(char *nm,char *vl,int is_commit)
{
    FILE *to_fp,*fp;
    char *target_file;
    int has_change=0;
    if(!is_commit) target_file=PRE_COMMIT_KERNEL_NVRM_FILE;
    else target_file=TEMP_KERNEL_NVRAM_FILE_NAME;

    if(!(fp=fopen(target_file,"r")))
        fp=fopen(target_file,"a+");
    if(!fp) return -1;

    to_fp=fopen(TEMP_KERNEL_NVRM_FILE,"w+");
    if(!to_fp) {
        fclose(fp);
        return -1;
    }

    has_change=_assign_value(fp,to_fp,nm,vl,is_commit);

    fclose(to_fp);
    fclose(fp);
    if(has_change) {
        unlink(target_file);
        rename(TEMP_KERNEL_NVRM_FILE,target_file);
    } else
        unlink(TEMP_KERNEL_NVRM_FILE);
    return has_change;
}

static int copy_file(char *from_file, char *to_file)
{
    FILE *from_fp,*fp;
    from_fp=fopen(from_file,"r");
    if(!from_fp)
        return 0;
    fp=fopen(to_file,"w+");
    if(fp) {
        while(fgets(g_line_buffer,NVRAM_LINE_MAX,from_fp)!=NULL)
            fputs(g_line_buffer,fp);
        fclose(fp);
    } else {
        fprintf(stderr,"%s:%d  Could not open file:%s \r\n",__FUNCTION__,__LINE__,to_file );
        fclose(from_fp);
        return -1;
    }
    fclose(from_fp);
    return 0;
}
/**
 * move tempary set nvram to kernel nvram file,then it will be used
 * by kernel with next boot up
 */
static int nvram_kcommit(void)
{
    int ret=0,has_change=0;
    char *name,*value;
    FILE *fp;
    static char line_buffer[NVRAM_LINE_MAX];
    fp=fopen(PRE_COMMIT_KERNEL_NVRM_FILE,"r");
    /* if there is no new nvram in temporary file, quit */
    if(!fp) return 0;
    /* temporary copy kernel nvram from /data to /var for merging */
    if(copy_file(KERNEL_NVRAM_FILE_NAME,TEMP_KERNEL_NVRAM_FILE_NAME)) {
        fprintf(stderr,"%s:%d temporary file error  \r\n",__FUNCTION__,__LINE__ );
        fclose(fp);
        return -1;
    }

    while(fgets(line_buffer,NVRAM_LINE_MAX,fp)!=NULL) {
        if(line_buffer[0]=='#') continue;
        value=line_buffer;
        name=strsep(&value,"=");
        if(name && value) {
            has_change+=nvram_kset(wlcsm_trim_str(name),wlcsm_trim_str(value),1);
        }
    }
    fclose(fp);
    /*copy the final file to /data partition */
    if(has_change)
        ret=copy_file(TEMP_KERNEL_NVRAM_FILE_NAME,KERNEL_NVRAM_FILE_NAME);
    unlink(TEMP_KERNEL_NVRAM_FILE_NAME);
    unlink(PRE_COMMIT_KERNEL_NVRM_FILE);
    return ret;

}
#endif
/**
 * read from nvram files and populate it to the system
 */
void kernel_nvram_populate(char *file_name)
{
    char *name,*value;
    if(file_name) {
        FILE *fp=fopen(file_name,"r+");
        if(fp) {
            while(fgets(g_line_buffer,NVRAM_LINE_MAX,fp)!=NULL) {
                if(g_line_buffer[0]=='#') continue;
                value=g_line_buffer;
                name=strsep(&value,"=");
                if(name && value) {
                    name=wlcsm_trim_str(name);
                    value=wlcsm_trim_str(value);
                    wlcsm_nvram_set(name,value);
                }
            }
            fclose(fp);
        }
    }
}

/**************END OF KERNEL NVRAM HANDLING SECTION **************/



/******* NVRAM_CMDS implementations *********/

/** nvram set wl0_ssid="12345" **/
static int nvram_cmd_set(int *argc, char ***p_argv) {
    char  *name,*value=NULL;
    NEXT_ARG(*argc,*p_argv);
    strncpy(value = g_buf, ARG_VAL, strlen(ARG_VAL)+1);
    name = strsep(&value, "=");
    wlcsm_nvram_set(name, value);
#ifdef NAND_SYS
    /*save to temporary file under var in
     *case it wil be commited */
    nvram_kset(name, value,0);
#endif
    return 0;
}

/** nvram setflag wl0_bititem 5=1 **/
static int nvram_cmd_setflag(int *argc, char ***p_argv) {
    char *bit_value,*bit_pos,*name,*value;
    char **argv;
    NEXT_ARG(*argc,*p_argv);
    NEXT_IS_VALID(*argc);
    argv=*p_argv;

    bit_value = argv[1];
    bit_pos = strsep(&bit_value, "=");
    if (bit_value && bit_pos && isnumber(bit_value) && isnumber(bit_pos)) {
        if ((value = wlcsm_nvram_get(argv[0])))
            printf("value:%s->",value);
        else
            printf("value:NULL->");

        wlcsm_nvram_set_bitflag(argv[0], atoi(bit_pos), atoi(bit_value));
        if ((name = wlcsm_nvram_get(argv[0])))
            printf("%s\n",name);
    }
    ADVANCE_ARGCV(*argc,*p_argv,1);
    return 0;
}

/** nvram unset wl0_ssid **/
static int nvram_cmd_unset(int *argc, char ***p_argv) {
    NEXT_ARG(*argc,*p_argv);
    wlcsm_nvram_unset(ARG_VAL);
#ifdef NAND_SYS
    nvram_kset((ARG_VAL), NULL,0);
#endif
    return 0;
}

/** nvram get wl0_ssid **/
static int nvram_cmd_get(int *argc, char ***p_argv) {
    char *value=NULL;
    NEXT_ARG(*argc,*p_argv);
    if ((value = wlcsm_nvram_get(ARG_VAL)))
        puts(value);
    return 0;
}

/** nvram getflag wl0_ssid 6 */
static int nvram_cmd_getflag(int *argc, char ***p_argv) {
    char *value=NULL;
    char **argv;
    NEXT_ARG(*argc,*p_argv);
    NEXT_IS_NUMBER(*argc, *p_argv);
    argv=*p_argv;
    if ((value = wlcsm_nvram_get_bitflag(argv[0], atoi(argv[1]))))
        puts(value);
    ADVANCE_ARGCV(*argc,*p_argv,1);
    return 0;
}

/** nvram getall|dump|show **/
static int nvram_cmd_getall(int *argc, char ***p_argv) {
    char *name=NULL;
    int size;
    ADVANCE_ARGCV(*argc,*p_argv,1);
    wlcsm_nvram_getall(g_buf, MAX_NVRAM_SPACE );
    for (name = g_buf; *name; name += strlen(name) + 1)
        puts(name);
    size = sizeof(struct nvram_header) + (int) name - (int)g_buf;
    fprintf(stderr, "size: %d bytes (%d left)\n", size, MAX_NVRAM_SPACE - size);
    return 0;
}
/** nvram loadfile filename **/
/** populate nvram from a file on board **/
int nvram_cmd_loadfile(int *argc, char ***p_argv) {
    char *file_name;
    ADVANCE_ARGCV(*argc,*p_argv,1);
    file_name=(ARG_VAL?ARG_VAL:"/data/nvramdefault.txt");
    kernel_nvram_populate(file_name);
    printf("popuplate nvram from %s done!\n",file_name);
    return 0;
}


/** nvram savefile filename **/
/** save nvram to a file on board **/
int nvram_cmd_savefile(int *argc, char ***p_argv) {
    char *name,*file_name;
    FILE *ofp;
    ADVANCE_ARGCV(*argc,*p_argv,1);
    file_name=(ARG_VAL?ARG_VAL:"/data/nvramdefault.txt");
    ofp =fopen(file_name,"w");
    if(!ofp) {
        fprintf(stderr,"%s:%d open configuration file error \r\n",__FUNCTION__,__LINE__ );
        return -1;
    }
    wlcsm_nvram_getall(g_buf, MAX_NVRAM_SPACE );
    for (name = g_buf; *name; name += strlen(name) + 1) {
        fputs(name,ofp);
        fputs("\n",ofp);
    }
    fclose(ofp);
    printf("save nvram to %s done!\n",file_name);
    return 0;
    }

/** nvram restart **/
static int nvram_cmd_restart(int *argc, char ***p_argv) {
    ADVANCE_ARGCV(*argc,*p_argv,1);
    wlcsm_mngr_restart(0,WLCSM_MNGR_RESTART_NVRAM,WLCSM_MNGR_RESTART_NOSAVEDM,1);
    return 0;
    }

#ifdef NAND_SYS
int nvram_cmd_kernelset(int *argc, char ***p_argv) {

    NEXT_ARG(*argc,*p_argv);
    kernel_nvram_populate(ARG_VAL);
    printf("restore done!\r\n");
    return 0;
}

/*
* -1 - error
*/
/*****************************************************************************
*  FUNCTION:  nvram_mfg_read_entry
*  PURPOSE:  Read next "name=value" entry from  from manufacturing NVRAM
*            file  (/mnt/nvram/nvram.nvm).
*  PARAMETERS:
*      f(IN) - manufacturing default NVRAM file handler.
*      ptr(IN) - buffer to read in.
*      len(IN) - max size of the buffer "ptr"
*  RETURNS:
*      strlen of string "name=value".
*      -1 if error occur.
*  NOTES:
*      Each couple "name=value" in /mnt/nvram/nvram.nvm is ending with '\0'.
*
*****************************************************************************/
static int nvram_mfg_read_entry(FILE *f, void* ptr, int len)
{
    int rv = -1;
    int i;
    char *p = (char*)ptr;

    for (i = 0; i < len; i++, p++) {
      if ((fread(p, 1, 1, f) != 1) ||
  	(*p == '\0')) {
        break;
      }
    }

    if (i < len)
      rv = i;

    return rv;
}

/*****************************************************************************
*  FUNCTION:  nvram_mfg_restore_default
*  PURPOSE:  Restore kernel NVRAM file (/data/.KERNEL_NVRAM_FILE_NAME)
*            from manufacturing default NVRAM setting (/mnt/nvram/nvram.nvm).
*  PARAMETERS:
*      pathname(IN) - the string of kernel NVRAM file name.
*  RETURNS:
*      0 - succeeded.
*      -1 error
*  NOTES:
*      /mnt/nvram/nvram.nvm is in binary format. Each "name=value" ending with '\0'
*
*****************************************************************************/
static int nvram_mfg_restore_default(char *pathname)
{
    FILE *f_mfg, *f_usr;
    int err = -1;

	
    if ((f_mfg = fopen(MFG_NVRAM_FILE_NAME_PATH, "rb")) != NULL) {
      clearerr(f_mfg);
      if ((f_usr = fopen(pathname, "a")) != NULL) {

        while ((err = nvram_mfg_read_entry(f_mfg, (void*)&g_line_buffer[0],
					 sizeof(g_line_buffer))) > 0) {
     	  fprintf(f_usr, "%s\n", g_line_buffer);
        }

        fclose(f_usr);
      }

      fclose(f_mfg);
    }

    return err;
}

int nvram_cmd_restore_mfg(int *argc, char ***p_argv) {
    NEXT_ARG(*argc,*p_argv);
    printf("Restoring NVRAM to manufacturing default ... ");
    if (nvram_mfg_restore_default(ARG_VAL) == 0)
        printf("done.\r\n");
    else
        printf("fail.\r\n");
    return 0;
}

#endif

/** nvram commit [restart] **/
static int nvram_cmd_commit(int *argc, char ***p_argv) {
    ADVANCE_ARGCV(*argc,*p_argv,1);
    //sends message to wldaemo to covers all the work nvram_commit does.
    if(ARG_VAL && !strncmp(ARG_VAL, "restart", 7)) {
        wlcsm_mngr_restart(0,WLCSM_MNGR_RESTART_NVRAM,WLCSM_MNGR_RESTART_SAVEDM,0);
    }
    else
        wlcsm_nvram_commit(); //no need restart for all other cases
#if defined(NAND_SYS)
    nvram_kcommit();
#endif
    return 0;
}

/** nvram kcommit **/
static int nvram_cmd_kcommit(int *argc, char ***p_argv) {
    ADVANCE_ARGCV(*argc,*p_argv,1);
#if defined(NAND_SYS)
    nvram_kcommit();
#endif
    return 0;
}
/** nvram godefault **/
static int nvram_cmd_wl_godefault(int *argc, char ***p_argv) {
    ADVANCE_ARGCV(*argc,*p_argv,1);
	//sends message to wldaemo to covers all the work nvram_commit does.
	//and remove kernel nvram configu.
	unlink(KERNEL_NVRAM_FILE_NAME);
	sync();
	//wlcsm_mngr_wl_godefault();
    return 0;
}

NVRAM_CMD_ITEM  nvram_cmd_items[]= {
    { "set", 		"set name with value",NVRAM_NORMAL_CMD,nvram_cmd_set,2},
    { "setflag",	"set bit value",NVRAM_NORMAL_CMD,nvram_cmd_setflag ,1},
    { "unset", 		"remove nvram entry",NVRAM_NORMAL_CMD,nvram_cmd_unset,2},
    { "get", 		"get nvram value with name",NVRAM_NORMAL_CMD,nvram_cmd_get,2},

    { "getflag",		"get bit value",NVRAM_NORMAL_CMD,nvram_cmd_getflag,1 },
    /* for cmd alias to use colon to seperate them */
    { "show:dump:getall", "show all nvrams",NVRAM_NORMAL_CMD,nvram_cmd_getall,0},
    { "loadfile", 	"populate nvram value from files",NVRAM_NORMAL_CMD,nvram_cmd_loadfile,1},
    { "savefile", 	"save all nvram value to file",NVRAM_NORMAL_CMD,nvram_cmd_savefile,1},
#ifdef NAND_SYS
    { "restore_mfg", "restore mfg nvrams",NVRAM_HIDDEN_CMD,nvram_cmd_restore_mfg,0},
    { "kernelset","populate nvram from kernel configuration file",NVRAM_HIDDEN_CMD,nvram_cmd_kernelset,0},
#endif
    { "commit","save nvram [optional] to restart wlan when following restart", NVRAM_NORMAL_CMD, nvram_cmd_commit,1},
    { "restart"		,"restart wlan",NVRAM_NORMAL_CMD, nvram_cmd_restart,1},
    { "kcommit", 	"only save knvrams",NVRAM_HIDDEN_CMD,nvram_cmd_kcommit,0},
    { "godefault", 	"reset wireless nvram to default and restart",NVRAM_HIDDEN_CMD,nvram_cmd_wl_godefault,1}
};

#define NVRAM_CMDS_NUM  (sizeof(nvram_cmd_items)/sizeof(NVRAM_CMD_ITEM))
#define FM_RESET "\033[0m"
#define FM_BOLDBLACK "\033[1m\033[30m"

void usage(char *cmd) {

    int i=0,j=0,line=0;
    if(cmd)
        printf("\n=====\nInput cmd:%s is not supported \n======\n",cmd);
    printf("\n======== NVRAM CMDS ========\n");
    for(i=0; i<NVRAM_CMDS_NUM; i++) {
        if(line%2) printf(FM_BOLDBLACK);
        else printf(FM_RESET);
        if(!nvram_cmd_items[i].is_hidden_cmd) {
            printf("[%s]\t",nvram_cmd_items[i].cmd);
            for(j=0; j<nvram_cmd_items[i].align_tab; j++)
                printf("\t");
            printf(": %s\n",nvram_cmd_items[i].prompt);
            line++;
}
    }
    printf(FM_RESET);
    printf("============================\n");
    if(g_buf) free(g_buf);
    exit(0);
    }


/* Main function to go through cmd list and execute cmd handler */

int main(int argc, char ** argv)
{
    int i=0,ret=0;
    char cmd_found=0;
    char *next;
    --argc;
    ++argv;
    if (!*argv) {
        usage(NULL);
    } else {
    WLCSM_SET_TRACE("nvram");
        g_buf=malloc(MAX_NVRAM_SPACE);
        if(!g_buf) {
        fprintf(stderr,"Could not allocate memory\n");
        return -1;
    }
        do {
            for(i=0; i<NVRAM_CMDS_NUM; i++) {
                foreachcolon(g_line_buffer,nvram_cmd_items[i].cmd,next) {
                    if(!strncmp(g_line_buffer,*argv,strlen(g_line_buffer)+1)) {
                        cmd_found=1;
            }
            }
                if(cmd_found) {
                    ret=nvram_cmd_items[i].func(&argc,&argv);
                    break;
            }
            }
            if(!cmd_found)
                usage(*argv);
            else
                cmd_found=0;
            if(!*argv) break;
            ADVANCE_ARGCV(argc,argv,1);
        } while(*argv);
        }
    free(g_buf);
    return ret;
}
