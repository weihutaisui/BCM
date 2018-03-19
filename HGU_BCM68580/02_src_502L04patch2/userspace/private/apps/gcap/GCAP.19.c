#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include <mocaint.h>

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.19 [-h]\n\
Set the Golden Node to default configuration and restart the Golden Node.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_19_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;

    uint32_t config_flags;
    uint32_t def;
    struct moca_max_constellation max_const;
    struct moca_taboo_channels taboo_channels;
    struct moca_rtr_config rtr;
    struct moca_const_tx_params ctx_parm;
    struct moca_password password;
    int i;

    chipId = NULL;

    // ----------- Parse parameters

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
      if (strcmp(argv[i], "-i") == 0)
      {
         i++;
         chipId = argv[i];
      }
      else if (strcmp(argv[i], "?") == 0)
      {
         printf( "Error!  Invalid option - %s\n", argv[i]);
         return(-1);
      }
      else if (strcmp(argv[i], "-h") == 0)
      {
         showUsage();
         return(0); 
      }
   }
#else

    opterr = 0;
    
    while((ret = getopt(argc, argv, "hi:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case '?':
            printf( "Error!  Invalid option - %c\n", optopt);
            return(-1);
            break;
        case 'h':
        default:
            showUsage();
            return(0); 
        }
    }
#endif
    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    ret = __moca_get_config_flags(ctx, &config_flags);

    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal error -5\n");
        return(-5);
    }

#define setdefault(x,def) \
    moca_set_##x##_defaults(&def, config_flags); \
    ret = moca_set_##x (ctx, def); \
    if (ret != MOCA_API_SUCCESS)\
    { \
        moca_close(ctx);\
        printf( "Error!  Internal error -16:" #x "\n");\
        return(-16);\
    }
 
#define setdefault_ptr(x,def) \
    moca_set_##x##_defaults(&def, config_flags); \
    ret = moca_set_##x (ctx, &def); \
    if (ret != MOCA_API_SUCCESS)\
    { \
        moca_close(ctx);\
        printf( "Error!  Internal error -16:" #x "\n");\
        return(-16);\
    }   
    // ----------- Set Defaults 
    setdefault(policing_en,def);
    setdefault_ptr(taboo_channels,taboo_channels);
    setdefault(beacon_pwr_reduction_en,def);
    setdefault(beacon_pwr_reduction,def);
    setdefault_ptr(const_tx_params,ctx_parm);
    setdefault(secondary_ch_offset,(*((int32_t *)&def)));
    setdefault(continuous_power_tx_mode,def);
    setdefault(bandwidth,def);
    setdefault(preferred_nc,def);
    setdefault(lof,def);
    setdefault(turbo_en,def);
    setdefault(lof_update,def);
    setdefault(max_tx_power,(*((int32_t *)&def)));
    setdefault(wom_mode,def);
    setdefault(per_mode,def);
    setdefault(primary_ch_offset,(*((int32_t *)&def)));
    setdefault(single_channel_operation,def);

    for (i = 0; i < MOCA_MAX_NODES; i++)
    {
       moca_set_max_constellation_defaults (&max_const, config_flags);
       max_const.node_id = i;
       moca_set_max_constellation(ctx, &max_const);
    }

    setdefault(min_aggr_waiting_time,def);
    setdefault(tpc_en,def);
    setdefault_ptr(rtr_config,rtr);
    setdefault(tlp_mode,def);
    setdefault(nc_mode,def);
    setdefault(privacy_en,def);
    setdefault_ptr(password,password);
    setdefault(mcfilter_enable,def);

    ret = moca_set_mr_seq_num(ctx, MOCA_MR_SEQ_NUM_DEF);
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  mr_seq_num failed\n");
        return(-12);
    }


    ret = moca_set_restart(ctx);

    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Restart failed-4\n");
        return(-4);
    }

    ret = moca_set_persistent(ctx);
    if (ret != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error!  Unable to save persistent parameters\n");
       return(-10);
    }

    // ----------- Finish

    moca_close(ctx);

    return(0);
}
