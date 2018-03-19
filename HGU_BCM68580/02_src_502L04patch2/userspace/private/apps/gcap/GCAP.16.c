#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>

static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static int privacy=0;
static char pswrd[18];

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.16 [-p <enable/disable>] [--Pswd <17 digit password>] [-M] [-r] [-h]\n\
Enable/disable privacy and set the password.\n\
Default values will be used if parameters are not supplied.\n\
Note that resetting SoC is required for configuration to take effect.\n\
If multiple configuration parameters are being changed, -r option can be\n\
used in the last command.\n\
\n\
Options:\n\
  -p      To 'enable' OR 'disable' Privacy (default 'disable')\n\
  --Pswd  Set privacy password up to 17 digits (default '99999999988888888')\n\
  -M      Make configuration changes permanent\n\
  -r      Reset SoC to make configuration changes effective\n\
  -h      Display this help and exit\n");
}

GCAP_GEN int GCAP_16_main(int argc, char **argv)
{
    void *ctx;
    int ret =0;
    int i;
    struct moca_password password;

    chipId = NULL;
    persistent = 0;
    reset = 0;
    privacy=0;
    strcpy(pswrd,"99999999988888888");

    // ----------- Parse parameters

    for(i=1;i<argc;i++)
    {
        if (argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
            case 'i':
                if (i+1 < argc)
                {
                    i++;
                    chipId = argv[i];
                }
                else
                {
                    chipId = "";
                }
                break;
            case 'M':
                persistent = 1;
                break;
            case 'r':
                reset = 1;
                break;
            case 'p':
                if ((i+1 < argc) && (argv[i+1][0] != '-'))
                {
                    if (strcmp(argv[i+1],"enable")==0)
                    {
                        privacy = 1;
                    }
                    else if (strcmp(argv[i+1],"disable")==0)
                    {
                        privacy = 0;
                    }
                    else
                    {
                        printf( "Error!  Invalid parameter\n");
                        return(-1);                     
                    }

                    i++;
                }
                else
                {
                    printf( "Error!  Missing parameter %s\n", &argv[i][2]);
                    return(-2);                     
                }
                break;        
            case '-':
                if (argv[i][2] != 'P')
                {
                    printf( "Error!  Invalid option - %s\n", &argv[i][2]);
                    return(-3);
                }

                if (i+1 < argc)
                {
                    i++;
                    strncpy(pswrd,argv[i], sizeof(pswrd)-1);
                    pswrd[sizeof(pswrd)-1] = '\0';
                }
                else
                {
                    pswrd[0] = '\0';
                }
                break;           
            case 'h':
                showUsage();
                return(0); 
                break;
            default:
                printf( "Error!  Invalid option - %s\n", &argv[i][1]);
                showUsage();
                return(-4);
            }
        }
    }


    if ((strlen(pswrd) < 12) ||
        (strlen(pswrd) > 17)) {
        printf( "Error!  Invalid password\n");
        return(-5);
    }
#if 0
    /* The password must be all decimal characters */
    for ( i = 0; i < strlen(pswrd); i++ ) {
        if ( (pswrd[i] < '0') || (pswrd[i] > '9') ) {
            printf( "Error!  Invalid password\n");
            return(-6);
        }
    }
#endif
    // ----------- Initialize
    
    ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-7);
    }

    // ----------- Save Settings 

   ret = moca_set_privacy_en(ctx, privacy);

    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 1\n");
        return(-8);
    }

    if (privacy )
    {
        memset(&password, 0, sizeof(password));
        strcpy(&password.password[0], pswrd);
        ret = moca_set_password(ctx, &password);
        if (ret != MOCA_API_SUCCESS)
        {
          moca_close(ctx);
          printf( "Error!  Internal 2\n");
          return(-9);
        }
    }

    if (reset)
    {
        ret = moca_set_restart(ctx); 
    }  
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 4\n");
        return(-10);
    }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-11);
       }
    }


    // ----------- Finish

    moca_close(ctx);

    return(0);
}

