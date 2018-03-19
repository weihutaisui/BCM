/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>
 *****************************************************************************
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ****************************************************************************

*/

#include "cms.h"
#include "cms_util.h"
#include "cms_log.h"
#include "ril_access.h"
#include "cms_access.h"
#include "uicc_controller.h"
#include "icc_file_handler.h"
#include "adn_record_loader.h"
#include "cellular_msg.h"
#include "cellular_internal.h"

#include <assert.h>
#include <stdlib.h>

static UiccCardStatus mIccCardStatus;

static void on_query_facility_lock(AsyncResult *ar); 
static void on_query_fdn_enabled(AsyncResult* ar);
static void on_get_icc_card_status_done(AsyncResult * ar);
static void on_get_imsi_done(AsyncResult *ar);
static void on_get_msisdn_done(Object *o);
static void query_ef_items(int index);
static void query_fdn(int index);
static void query_pin1_state(int index);
static void fetch_records(int index);
static void reset_icc_card_status(UiccCardStatus *cardStatus);
static char* bcd_to_string(UINT8* data, int offset, int length);
static void on_get_iccid_done(AsyncResult *ar);
static void on_new_sms_on_sim(AsyncResult *ar);
static void handle_sms(Object* result);

static const char* MCCMNC_CODES_HAVING_3DIGITS_MNC[] = 
{
    "302370", "302720", "310260",
    "405025", "405026", "405027", "405028", "405029", "405030", "405031", "405032",
    "405033", "405034", "405035", "405036", "405037", "405038", "405039", "405040",
    "405041", "405042", "405043", "405044", "405045", "405046", "405047", "405750",
    "405751", "405752", "405753", "405754", "405755", "405756", "405799", "405800",
    "405801", "405802", "405803", "405804", "405805", "405806", "405807", "405808",
    "405809", "405810", "405811", "405812", "405813", "405814", "405815", "405816",
    "405817", "405818", "405819", "405820", "405821", "405822", "405823", "405824",
    "405825", "405826", "405827", "405828", "405829", "405830", "405831", "405832",
    "405833", "405834", "405835", "405836", "405837", "405838", "405839", "405840",
    "405841", "405842", "405843", "405844", "405845", "405846", "405847", "405848",
    "405849", "405850", "405851", "405852", "405853", "405875", "405876", "405877",
    "405878", "405879", "405880", "405881", "405882", "405883", "405884", "405885",
    "405886", "405908", "405909", "405910", "405911", "405912", "405913", "405914",
    "405915", "405916", "405917", "405918", "405919", "405920", "405921", "405922",
    "405923", "405924", "405925", "405926", "405927", "405928", "405929", "405930",
    "405931", "405932", "502142", "502143", "502145", "502146", "502147", "502148"
};

static struct {
   int mcc;
   char *iso;
   int smallestDigitsMCC;
   char *language;

} MccTable[] = {
   {202,"gr",2},	//Greece
   {204,"nl",2,"nl"},	//Netherlands (Kingdom of the)
   {206,"be",2},	//Belgium
   {208,"fr",2,"fr"},	//France
   {212,"mc",2},	//Monaco (Principality of)
   {213,"ad",2},	//Andorra (Principality of)
   {214,"es",2,"es"},	//Spain
   {216,"hu",2},	//Hungary (Republic of)
   {218,"ba",2},	//Bosnia and Herzegovina
   {219,"hr",2},	//Croatia (Republic of)
   {220,"rs",2},	//Serbia and Montenegro
   {222,"it",2,"it"},	//Italy
   {225,"va",2,"it"},	//Vatican City State
   {226,"ro",2},	//Romania
   {228,"ch",2,"de"},	//Switzerland (Confederation of)
   {230,"cz",2,"cs"},	//Czech Republic
   {231,"sk",2},	//Slovak Republic
   {232,"at",2,"de"},	//Austria
   {234,"gb",2,"en"},	//United Kingdom of Great Britain and Northern Ireland
   {235,"gb",2,"en"},	//United Kingdom of Great Britain and Northern Ireland
   {238,"dk",2},	//Denmark
   {240,"se",2,"sv"},	//Sweden
   {242,"no",2},	//Norway
   {244,"fi",2},	//Finland
   {246,"lt",2},	//Lithuania (Republic of)
   {247,"lv",2},	//Latvia (Republic of)
   {248,"ee",2},	//Estonia (Republic of)
   {250,"ru",2},	//Russian Federation
   {255,"ua",2},	//Ukraine
   {257,"by",2},	//Belarus (Republic of)
   {259,"md",2},	//Moldova (Republic of)
   {260,"pl",2},	//Poland (Republic of)
   {262,"de",2,"de"},	//Germany (Federal Republic of)
   {266,"gi",2},	//Gibraltar
   {268,"pt",2},	//Portugal
   {270,"lu",2},	//Luxembourg
   {272,"ie",2,"en"},	//Ireland
   {274,"is",2},	//Iceland
   {276,"al",2},	//Albania (Republic of)
   {278,"mt",2},	//Malta
   {280,"cy",2},	//Cyprus (Republic of)
   {282,"ge",2},	//Georgia
   {283,"am",2},	//Armenia (Republic of)
   {284,"bg",2},	//Bulgaria (Republic of)
   {286,"tr",2},	//Turkey
   {288,"fo",2},	//Faroe Islands
   {289,"ge",2},    //Abkhazia (Georgia)
   {290,"gl",2},	//Greenland (Denmark)
   {292,"sm",2},	//San Marino (Republic of)
   {293,"si",2},	//Slovenia (Republic of)
   {294,"mk",2},   //The Former Yugoslav Republic of Macedonia
   {295,"li",2},	//Liechtenstein (Principality of)
   {297,"me",2},    //Montenegro (Republic of)
   {302,"ca",3,"en"},	//Canada
   {308,"pm",2},	//Saint Pierre and Miquelon (Collectivit territoriale de la Rpublique franaise)
   {310,"us",3,"en"},	//United States of America
   {311,"us",3,"en"},	//United States of America
   {312,"us",3,"en"},	//United States of America
   {313,"us",3,"en"},	//United States of America
   {314,"us",3,"en"},	//United States of America
   {315,"us",3,"en"},	//United States of America
   {316,"us",3,"en"},	//United States of America
   {330,"pr",2},	//Puerto Rico
   {332,"vi",2},	//United States Virgin Islands
   {334,"mx",3},	//Mexico
   {338,"jm",3},	//Jamaica
   {340,"gp",2},	//Guadeloupe (French Department of)
   {342,"bb",3},	//Barbados
   {344,"ag",3},	//Antigua and Barbuda
   {346,"ky",3},	//Cayman Islands
   {348,"vg",3},	//British Virgin Islands
   {350,"bm",2},	//Bermuda
   {352,"gd",2},	//Grenada
   {354,"ms",2},	//Montserrat
   {356,"kn",2},	//Saint Kitts and Nevis
   {358,"lc",2},	//Saint Lucia
   {360,"vc",2},	//Saint Vincent and the Grenadines
   {362,"ai",2},	//Netherlands Antilles
   {363,"aw",2},	//Aruba
   {364,"bs",2},	//Bahamas (Commonwealth of the)
   {365,"ai",3},	//Anguilla
   {366,"dm",2},	//Dominica (Commonwealth of)
   {368,"cu",2},	//Cuba
   {370,"do",2},	//Dominican Republic
   {372,"ht",2},	//Haiti (Republic of)
   {374,"tt",2},	//Trinidad and Tobago
   {376,"tc",2},	//Turks and Caicos Islands
   {400,"az",2},	//Azerbaijani Republic
   {401,"kz",2},	//Kazakhstan (Republic of)
   {402,"bt",2},	//Bhutan (Kingdom of)
   {404,"in",2},	//India (Republic of)
   {405,"in",2},	//India (Republic of)
   {410,"pk",2},	//Pakistan (Islamic Republic of)
   {412,"af",2},	//Afghanistan
   {413,"lk",2},	//Sri Lanka (Democratic Socialist Republic of)
   {414,"mm",2},	//Myanmar (Union of)
   {415,"lb",2},	//Lebanon
   {416,"jo",2},	//Jordan (Hashemite Kingdom of)
   {417,"sy",2},	//Syrian Arab Republic
   {418,"iq",2},	//Iraq (Republic of)
   {419,"kw",2},	//Kuwait (State of)
   {420,"sa",2},	//Saudi Arabia (Kingdom of)
   {421,"ye",2},	//Yemen (Republic of)
   {422,"om",2},	//Oman (Sultanate of)
   {423,"ps",2},    //Palestine
   {424,"ae",2},	//United Arab Emirates
   {425,"il",2},	//Israel (State of)
   {426,"bh",2},	//Bahrain (Kingdom of)
   {427,"qa",2},	//Qatar (State of)
   {428,"mn",2},	//Mongolia
   {429,"np",2},	//Nepal
   {430,"ae",2},	//United Arab Emirates
   {431,"ae",2},	//United Arab Emirates
   {432,"ir",2},	//Iran (Islamic Republic of)
   {434,"uz",2},	//Uzbekistan (Republic of)
   {436,"tj",2},	//Tajikistan (Republic of)
   {437,"kg",2},	//Kyrgyz Republic
   {438,"tm",2},	//Turkmenistan
   {440,"jp",2,"ja"},	//Japan
   {441,"jp",2,"ja"},	//Japan
   {450,"kr",2,"ko"},	//Korea (Republic of)
   {452,"vn",2},	//Viet Nam (Socialist Republic of)
   {454,"hk",2},	//"Hong Kong, China"
   {455,"mo",2},	//"Macao, China"
   {456,"kh",2},	//Cambodia (Kingdom of)
   {457,"la",2},	//Lao People's Democratic Republic
   {460,"cn",2,"zh"},	//China (People's Republic of)
   {461,"cn",2,"zh"},	//China (People's Republic of)
   {466,"tw",2},	//"Taiwan, China"
   {467,"kp",2},	//Democratic People's Republic of Korea
   {470,"bd",2},	//Bangladesh (People's Republic of)
   {472,"mv",2},	//Maldives (Republic of)
   {502,"my",2},	//Malaysia
   {505,"au",2,"en"},	//Australia
   {510,"id",2},	//Indonesia (Republic of)
   {514,"tl",2},	//Democratic Republic of Timor-Leste
   {515,"ph",2},	//Philippines (Republic of the)
   {520,"th",2},	//Thailand
   {525,"sg",2,"en"},	//Singapore (Republic of)
   {528,"bn",2},	//Brunei Darussalam
   {530,"nz",2, "en"},	//New Zealand
   {534,"mp",2},	//Northern Mariana Islands (Commonwealth of the)
   {535,"gu",2},	//Guam
   {536,"nr",2},	//Nauru (Republic of)
   {537,"pg",2},	//Papua New Guinea
   {539,"to",2},	//Tonga (Kingdom of)
   {540,"sb",2},	//Solomon Islands
   {541,"vu",2},	//Vanuatu (Republic of)
   {542,"fj",2},	//Fiji (Republic of)
   {543,"wf",2},	//Wallis and Futuna (Territoire franais d'outre-mer)
   {544,"as",2},	//American Samoa
   {545,"ki",2},	//Kiribati (Republic of)
   {546,"nc",2},	//New Caledonia (Territoire franais d'outre-mer)
   {547,"pf",2},	//French Polynesia (Territoire franais d'outre-mer)
   {548,"ck",2},	//Cook Islands
   {549,"ws",2},	//Samoa (Independent State of)
   {550,"fm",2},	//Micronesia (Federated States of)
   {551,"mh",2},	//Marshall Islands (Republic of the)
   {552,"pw",2},	//Palau (Republic of)
   {602,"eg",2},	//Egypt (Arab Republic of)
   {603,"dz",2},	//Algeria (People's Democratic Republic of)
   {604,"ma",2},	//Morocco (Kingdom of)
   {605,"tn",2},	//Tunisia
   {606,"ly",2},	//Libya (Socialist People's Libyan Arab Jamahiriya)
   {607,"gm",2},	//Gambia (Republic of the)
   {608,"sn",2},	//Senegal (Republic of)
   {609,"mr",2},	//Mauritania (Islamic Republic of)
   {610,"ml",2},	//Mali (Republic of)
   {611,"gn",2},	//Guinea (Republic of)
   {612,"ci",2},	//Cte d'Ivoire (Republic of)
   {613,"bf",2},	//Burkina Faso
   {614,"ne",2},	//Niger (Republic of the)
   {615,"tg",2},	//Togolese Republic
   {616,"bj",2},	//Benin (Republic of)
   {617,"mu",2},	//Mauritius (Republic of)
   {618,"lr",2},	//Liberia (Republic of)
   {619,"sl",2},	//Sierra Leone
   {620,"gh",2},	//Ghana
   {621,"ng",2},	//Nigeria (Federal Republic of)
   {622,"td",2},	//Chad (Republic of)
   {623,"cf",2},	//Central African Republic
   {624,"cm",2},	//Cameroon (Republic of)
   {625,"cv",2},	//Cape Verde (Republic of)
   {626,"st",2},	//Sao Tome and Principe (Democratic Republic of)
   {627,"gq",2},	//Equatorial Guinea (Republic of)
   {628,"ga",2},	//Gabonese Republic
   {629,"cg",2},	//Congo (Republic of the)
   {630,"cg",2},	//Democratic Republic of the Congo
   {631,"ao",2},	//Angola (Republic of)
   {632,"gw",2},	//Guinea-Bissau (Republic of)
   {633,"sc",2},	//Seychelles (Republic of)
   {634,"sd",2},	//Sudan (Republic of the)
   {635,"rw",2},	//Rwanda (Republic of)
   {636,"et",2},	//Ethiopia (Federal Democratic Republic of)
   {637,"so",2},	//Somali Democratic Republic
   {638,"dj",2},	//Djibouti (Republic of)
   {639,"ke",2},	//Kenya (Republic of)
   {640,"tz",2},	//Tanzania (United Republic of)
   {641,"ug",2},	//Uganda (Republic of)
   {642,"bi",2},	//Burundi (Republic of)
   {643,"mz",2},	//Mozambique (Republic of)
   {645,"zm",2},	//Zambia (Republic of)
   {646,"mg",2},	//Madagascar (Republic of)
   {647,"re",2},	//Reunion (French Department of)
   {648,"zw",2},	//Zimbabwe (Republic of)
   {649,"na",2},	//Namibia (Republic of)
   {650,"mw",2},	//Malawi
   {651,"ls",2},	//Lesotho (Kingdom of)
   {652,"bw",2},	//Botswana (Republic of)
   {653,"sz",2},	//Swaziland (Kingdom of)
   {654,"km",2},	//Comoros (Union of the)
   {655,"za",2,"en"},	//South Africa (Republic of)
   {657,"er",2},	//Eritrea
   {702,"bz",2},	//Belize
   {704,"gt",2},	//Guatemala (Republic of)
   {706,"sv",2},	//El Salvador (Republic of)
   {708,"hn",3},	//Honduras (Republic of)
   {710,"ni",2},	//Nicaragua
   {712,"cr",2},	//Costa Rica
   {714,"pa",2},	//Panama (Republic of)
   {716,"pe",2},	//Peru
   {722,"ar",3},	//Argentine Republic
   {724,"br",2},	//Brazil (Federative Republic of)
   {730,"cl",2},	//Chile
   {732,"co",3},	//Colombia (Republic of)
   {734,"ve",2},	//Venezuela (Bolivarian Republic of)
   {736,"bo",2},	//Bolivia (Republic of)
   {738,"gy",2},	//Guyana
   {740,"ec",2},	//Ecuador
   {742,"gf",2},	//French Guiana (French Department of)
   {744,"py",2},	//Paraguay (Republic of)
   {746,"sr",2},	//Suriname (Republic of)
   {748,"uy",2},	//Uruguay (Eastern Republic of)
   {750,"fk",2},	//Falkland Islands (Malvinas)	
};

static char* mIccId = NULL;
static char* mMsisdn = NULL;

void uicc_controller_init()
{
    reset_icc_card_status(&mIccCardStatus);    
}


void uicc_controller_process(struct internal_message *msg)
{
    AsyncResult *ar;
    Object* o = internal_message_getObject(msg);
    switch(msg->what)
    {
        /* 
         * For now, the vendorRil would not send 
         * RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED to us. Thus we would depend on
         * radio status change to send out sim query.
         */
        case EVENT_RADIO_STATE_CHANGED:
            if(ril_access_get_radio_state() == RADIO_ON)
            {
                ril_access_get_icc_card_status(internal_message_obtain(
                            EVENT_GET_ICC_STATUS_DONE, UICC_CONTROLLER));
            }
            break;

        case EVENT_ICC_STATUS_CHANGED:
            ril_access_get_icc_card_status(internal_message_obtain(
                        EVENT_GET_ICC_STATUS_DONE, UICC_CONTROLLER));
            break;

        case EVENT_GET_ICC_STATUS_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_get_icc_card_status_done(ar);
            break;

        case EVENT_QUERY_FACILITY_FDN_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_query_fdn_enabled(ar);
            break;
        
        case EVENT_QUERY_FACILITY_LOCK_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_query_facility_lock(ar);
            break;

        case EVENT_GET_IMSI_DONE:
            //retrieve imsi code information;
            //and notify dc_tracker of record load completion?
            ar = (AsyncResult *)object_getData(o);
            on_get_imsi_done(ar);
            break;

        case EVENT_GET_ICCID_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_get_iccid_done(ar);
            break;

        case EVENT_NEW_SMS_ON_SIM:
            /* try to get SMS raw data using the index. */
            ar = (AsyncResult *)object_getData(o);
            on_new_sms_on_sim(ar);
            break;

        case EVENT_GET_SMS_DONE:
            ar = (AsyncResult *)object_getData(o);
            if(ar->error == 0)
            {
                handle_sms(ar->result);
            }
            else
            {
                cmsLog_error("get sms error %d", ar->error);
            }
            break;

        case EVENT_GET_MSISDN_DONE:
            ar = (AsyncResult *)object_getData(o);
            if(ar->error == 0)
            {
                on_get_msisdn_done(ar->result);
            }
            else
            {
                cmsLog_error("get sms error %d", ar->error);
            }
            break;

        default:
            break;
    }
}

static void handle_sms(Object* result)
{
    IccIoResult *data;
    UINT8 *ba;

    assert(result);
    data = (IccIoResult *)object_getData(result);
    ba = data->payload;

    if(ba[0] != 0)
    {
        cmsLog_debug("ENF: status: %d", ba[0]);
    }

    if(ba[3] == 3)
    {
        int n = data->length;
        cms_access_notify_newSms(TRUE, data->index, ba+1, n-1);
    }
    free(data->payload);
    data->payload = NULL;
}

static void on_get_imsi_done(AsyncResult *ar)
{
    struct internal_message* msg;
    Object *o;

    assert(ar);

    int index = *(int *)object_getData(ar->userObj);
    
    cmsLog_debug("index=%d", index);

    if (ar->error != 0) 
    {
        cmsLog_notice("Exception querying IMSI, error=%d", ar->error);
        return;
    }
    
    UiccCardApplication *s = &(mIccCardStatus.applications[index]);
    /*
     * Let's assume ril_access has done pre-processing to make sure the sImsi 
     * is of valid data
     */
    char *sImsi = (char *)object_getData(ar->result);
     
    // IMSI (MCC+MNC+MSIN) is at least 6 digits, but not more
    // than 15 (and usually 15).
    if ((strlen(sImsi) < 6) || (strlen(sImsi) > 15)) 
    {
        cmsLog_notice("invalid IMSI");
        return;
    }
    
    int s_len = strlen(sImsi) + 1;

    if(s->mImsi == NULL)
    {
        s->mImsi = malloc(s_len);
    }
    else
    {
        free(s->mImsi);
        s->mImsi = malloc(s_len);
    }

    assert(s->mImsi);

    strncpy(s->mImsi, sImsi, s_len);

    cmsLog_debug("IMSI: %s, mMncLength: %d", s->mImsi, s->mMncLength);
    
    /* Calculate the length of mnc in the event handling here. 
     * Note: the mncLength is supposed to be retrived from EF_AD icc io response.
     * Here we simplify the process to iterate the software development. 
     * Need to fix it later.
     * */ 
    if(((s->mMncLength == MNC_LENGTH_UNINITIALIZED) || 
        (s->mMncLength == MNC_LENGTH_UNKNOWN) || (s->mMncLength == 2)) &&
            ((s->mImsi != NULL) && (cmsUtl_strlen(s->mImsi) >= 6)))
    {
        char *a;
        int i;
        int size;

        a = malloc(7);
        assert(a);
        cmsUtl_strncpy(a, s->mImsi, 7);

        size = sizeof(MCCMNC_CODES_HAVING_3DIGITS_MNC)/sizeof(MCCMNC_CODES_HAVING_3DIGITS_MNC[0]);
        cmsLog_debug("%s compare mnc codes to %d elements", a, size);

        for(i=0; i<size; i++)
        {
            if(cmsUtl_strcmp(a, MCCMNC_CODES_HAVING_3DIGITS_MNC[i]) == 0)
            {
                cmsLog_debug("Hacking mncLength to be 3 for now...");
                s->mMncLength = 3;
                break;
            } 
        }
		
        free(a);
    }

     // Sarah: anyway guess using the mcc
    if (s->mMncLength == MNC_LENGTH_UNINITIALIZED || s->mMncLength == MNC_LENGTH_UNKNOWN) 
    {
        int i;
		char mccStr[BUFLEN_4];
		
        cmsUtl_strncpy(mccStr, s->mImsi, BUFLEN_4);
        cmsLog_debug("mccStr = %s", mccStr);
		
        s->mMncLength = 3;
		
        for (i = 0; i < sizeof(MccTable)/sizeof(MccTable[0]); i++)
        {
           if (MccTable[i].mcc == atoi(mccStr))
           	{
                s->mMncLength = MccTable[i].smallestDigitsMCC;
                cmsLog_debug("Adjust mncLength to be smallestDigitsMCC %d", MccTable[i].smallestDigitsMCC);
                break;
           	}
        }

    }

    /* A short cut to notify DC_TRACKER that the records have been loaded. 
     * Might need to further process OPERATOR_NUMERIC and OPERATOR_ISO_COUNTRY
     */ 
    msg = internal_message_obtain(EVENT_RECORD_LOADED, ALL); 
    o = object_obtain();
    object_setData(o, &index, sizeof(index));
    internal_message_setObject(msg, o);

    cellular_internal_process(msg);
    
    internal_message_freeObject(msg);
    free(msg);
}

static void on_get_iccid_done(AsyncResult *ar)
{
    Object *result;
    IccIoResult *data;

    assert(ar);
    if(ar->error != 0)
    {
        cmsLog_error("error=%d", ar->error);
        return;
    }
    result = ar->result;
    data = (IccIoResult *)object_getData(result);

    if(mIccId != NULL)
    {
        free(mIccId);
        mIccId = NULL;
    }
    mIccId = bcd_to_string(data->payload, 0, data->length);
    
    /* Take responsibility to free the memory allocated inside Object's data.*/
    free(data->payload);
    data->payload = NULL;
    data->length = 0;

    cmsLog_debug("mIccId = %s", mIccId);
}

/**
 * Many fields in GSM SIM's are stored as nibble-swizzled BCD
 *
 * Assumes left-justified field that may be padded right with 0xf
 * values.
 *
 * Stops on invalid BCD value, returning string so far
 */
static char* bcd_to_string(UINT8* data, int offset, int length)
{
    char *ret;
    int i, str_index;

    ret = malloc(length*2+1);
    assert(ret);
    memset(ret, 0x00, length*2+1);
    str_index = 0;

    for(i=offset; i<offset+length; i++)
    {
        char temp;
        int v;

        v = data[i] & 0xf;
        if(v > 9) 
        {
            break;
        }
        temp = '0' + v; 
        ret[str_index]  = temp;
        str_index++;

        v = (data[i] >> 4) & 0xf;
        // Some PLMNs have 'f' as high nibble, ignore it
        if(v == 0xf) 
        {
            continue;
        } 
        if(v > 9)
        {
            break;
        }
        temp = '0' + v;
        ret[str_index] = temp;
        str_index++;

    }
    // null terminate the string.
    ret[str_index] = '\0';
    return ret;
}

const char* uicc_controller_getImsi(int application_index)
{
    UiccCardApplication *u;

    if((application_index < 0) || (application_index > RIL_CARD_MAX_APPS))
    {
        return NULL;
    }

    u = &(mIccCardStatus.applications[application_index]);
    return u->mImsi;
}

/**
 * Returns the 5 or 6 digit MCC/MNC of the operator that
 * provided the SIM card. Returns null of SIM is not yet ready
 * or is not valid for the type of IccCard. Generally used for
 * GSM/UMTS and the like SIMS
 */
char* uicc_controller_getOperatorNumeric(int application_index)
{
    UiccCardApplication *u;
    char *s;
    int s_len;

    if((application_index < 0) || (application_index > RIL_CARD_MAX_APPS))
    {
        return NULL;
    }

    u = &(mIccCardStatus.applications[application_index]);
    if(u->mImsi == NULL)
    {
        return NULL;
    }

    if(u->mMncLength == MNC_LENGTH_UNINITIALIZED || 
       u->mMncLength == MNC_LENGTH_UNKNOWN)
    {
        cmsLog_notice("getSIMOperatorNumeric: bad mncLength.");
        return NULL;
    }
	    
    s_len = 3 + u->mMncLength + 1;
    s = malloc(s_len);
    assert(s);
    memset(s, 0, s_len);	
    memcpy(s, u->mImsi, s_len - 1);

    return s;
}

const char* uicc_controller_getIccId()
{
    return mIccId;
}

/* Free the strings allocated for the structure and memset other fields. */
static void reset_icc_card_status(UiccCardStatus *cardStatus)
{
    int i; 
    UiccCardApplication *u;
    RIL_AppStatus *a;

    assert(cardStatus);

    cardStatus->card_state = RIL_CARDSTATE_ABSENT;
    cardStatus->universal_pin_state = RIL_PINSTATE_UNKNOWN;
    cardStatus->gsm_umts_subscription_app_index = -1;
    cardStatus->cdma_subscription_app_index = -1;
    cardStatus->ims_subscription_app_index = -1;
    cardStatus->num_applications = 0; 

    for(i=0; i<RIL_CARD_MAX_APPS; i++)
    {
        u = &(cardStatus->applications[i]);
        if(u->mImsi != NULL)
        {
            free(u->mImsi);
            u->mImsi = NULL;
        }
        
        a = &(u->mAppStatus);

        if(a->aid_ptr != NULL)
        {
            free(a->aid_ptr);
            a->aid_ptr = NULL;
        }
        if(a->app_label_ptr != NULL)
        {
            free(a->app_label_ptr);
            a->app_label_ptr = NULL;    
        }
        
        u->mIccFdnEnabled = FALSE;
        u->mIccFdnAvailable = FALSE;
        u->mIccLockEnabled = FALSE;
        u->mMncLength = MNC_LENGTH_UNINITIALIZED;
        memset(&(u->mAppStatus), 0x00, sizeof(RIL_AppStatus)); 
    }

} 


/* Need to update mUiccCardStatus based on the result.
 * Also update the UiccCardApplications accordingly. 
 */
static void on_get_icc_card_status_done(AsyncResult * ar)
{
    int i;
    RIL_CardStatus_v6 *status = (RIL_CardStatus_v6 *)object_getData(ar->result);
    UiccCardApplication *u;

    if(ar->error != 0)
    {
        cmsLog_error("abort.");
        return;
    }

    reset_icc_card_status(&mIccCardStatus); 

    mIccCardStatus.card_state = status->card_state;
    mIccCardStatus.universal_pin_state = status->universal_pin_state;
    mIccCardStatus.gsm_umts_subscription_app_index = 
        status->gsm_umts_subscription_app_index;
    mIccCardStatus.cdma_subscription_app_index = 
        status->cdma_subscription_app_index;
    mIccCardStatus.ims_subscription_app_index = 
        status->ims_subscription_app_index;

    assert(status->num_applications <= RIL_CARD_MAX_APPS);
    mIccCardStatus.num_applications = status->num_applications;


    for(i=0; i<status->num_applications; i++)
    {
        u = &(mIccCardStatus.applications[i]);
        /* The strings are copied over to uicc_controller's memory */
        memcpy(&(u->mAppStatus), &(status->applications[i]), sizeof(RIL_AppStatus));

        /* 
         * We have the RIL_AppStatus now, query other information for 
         * the ready application to fill other fields.
         */
        if(u->mAppStatus.app_state == RIL_APPSTATE_READY)
        {
            query_fdn(i);
            query_pin1_state(i);
            query_ef_items(i);
        }
    }
    
    
}

static void query_ef_items(int index)
{
    struct internal_message *msg;
    const char path_0[] = "3F00";
    const char path_2[] = "3F007FFF";

    RIL_AppStatus *s = &(mIccCardStatus.applications[index].mAppStatus);

    cmsLog_debug("index = %d", index);
    cmsLog_debug("aid = %s", s->aid_ptr);

    msg = internal_message_obtain(EVENT_GET_ICCID_DONE, UICC_CONTROLLER);
    ril_access_icc_io_for_app(COMMAND_GET_RESPONSE, EF_ICCID, (char *)&path_0, 
            0x0, 0x0, 15, NULL, NULL, NULL, msg);

    ril_access_icc_io_for_app(COMMAND_GET_RESPONSE, EF_AD, (char *)&path_2, 
            0x0, 0x0, 15, NULL, NULL, NULL, NULL);
    ril_access_icc_io_for_app(COMMAND_READ_BINARY, EF_AD, (char *)&path_2, 
            0x0, 0x0, 0x04, NULL, NULL, NULL, NULL);

    msg = internal_message_obtain(EVENT_GET_MSISDN_DONE, UICC_CONTROLLER);
    adn_record_LoadFromEF(EF_MSISDN, EF_EXT1, 1, s->aid_ptr, msg);
}

static void query_fdn(int index)
{
    //This shouldn't change run-time. So needs to be called only once.
    int serviceClassX;
    struct internal_message *msg;
    RIL_AppStatus *s = &(mIccCardStatus.applications[index].mAppStatus);
    
    cmsLog_debug("index = %d", index);

    msg = internal_message_obtain(EVENT_QUERY_FACILITY_FDN_DONE, UICC_CONTROLLER);
    
    Object *o = object_obtain();
    object_setData(o, &index, sizeof(int));
    internal_message_setObject(msg, o);
    o = NULL;

    serviceClassX = SERVICE_CLASS_VOICE + SERVICE_CLASS_DATA + 
                     SERVICE_CLASS_FAX;
    ril_access_query_facility_lock_for_app(CB_FACILITY_BA_FD, "", serviceClassX, 
                                       s->aid_ptr, msg);

}
    
static void query_pin1_state(int index) 
{
    RIL_AppStatus *s = &(mIccCardStatus.applications[index].mAppStatus);
    int serviceClassX = SERVICE_CLASS_VOICE + SERVICE_CLASS_DATA +
                        SERVICE_CLASS_FAX;
    struct internal_message *msg;

    cmsLog_debug("index to query = %d", index);
    msg = internal_message_obtain(EVENT_QUERY_FACILITY_LOCK_DONE, UICC_CONTROLLER);
    
    Object *o = object_obtain();
    object_setData(o, &index, sizeof(int));
    internal_message_setObject(msg, o);
    o = NULL;

    ril_access_query_facility_lock_for_app (CB_FACILITY_BA_SIM, "", serviceClassX,
            s->aid_ptr, msg);
}


/**
 * Interpret EVENT_QUERY_FACILITY_LOCK_DONE
 * @param ar is asyncResult of Query_Facility_Locked
 */
static void on_query_fdn_enabled(AsyncResult* ar)
{
    int* result; 
    int result_length;
    int index;
    UiccCardApplication *u;

    if(ar->error != 0)
    {
        cmsLog_error("abort.");
        return;
    }

    assert(ar->result);
    assert(ar->userObj);

    result = (int *)object_getData(ar->result);
    result_length = object_dataLength(ar->result);
    index = *(int *)object_getData(ar->userObj);
    cmsLog_debug("index = %d", index);

    if(result_length != 0)
    {
        u = &(mIccCardStatus.applications[index]);
        //0 - Available & Disabled, 1-Available & Enabled, 2-Unavailable.
        if (result[0] == 2)
        {
            u->mIccFdnEnabled = FALSE;
            u->mIccFdnAvailable = FALSE;
        }
        else
        {
            u->mIccFdnEnabled = (result[0] == 1) ? TRUE : FALSE;
            u->mIccFdnAvailable = TRUE;
        }
        cmsLog_debug("Query facility FDN : FDN service available:%d enabled:%d\n",
                u->mIccFdnAvailable, 
                u->mIccFdnEnabled);
    }
    else
    {
        cmsLog_error("Bogus facility lock response\n");
    }

    /*
     * Try to fetch the records here.  
     */
    fetch_records(index);
}

/** REMOVE when mIccLockEnabled is not needed*/
static void on_query_facility_lock(AsyncResult *ar) 
{
    int ints_length;
    int* ints; 
    int index;
    UiccCardApplication *uiccCardApp; 
    
    assert(ar);
    if(ar->error != 0)
    {
        cmsLog_error("abort.");
        return;
    }

    ints_length = (object_dataLength)(ar->result);
    ints = (int *)(object_getData)(ar->result);
    index = *(int *)object_getData(ar->userObj);
    uiccCardApp = &(mIccCardStatus.applications[index]);

    cmsLog_debug("index = %d", index);

    if(ints_length != 0) 
    {
        cmsLog_debug("Query facility lock :%d \n", ints[0]);
        uiccCardApp->mIccLockEnabled = (ints[0] != 0);

        if (uiccCardApp->mIccLockEnabled) 
        {
            //mPinLockedRegistrants.notifyRegistrants();
        }
        // Sanity check: we expect mPin1State to match mIccLockEnabled.
        // When mPin1State is DISABLED mIccLockEanbled should be false.
        // When mPin1State is ENABLED mIccLockEnabled should be true.
        //
        // Here we validate these assumptions to assist in identifying which ril/radio's
        // have not correctly implemented GET_SIM_STATUS
        switch (uiccCardApp->mAppStatus.pin1) 
        {
            case RIL_PINSTATE_DISABLED:
                if (uiccCardApp->mIccLockEnabled) 
                {
                    cmsLog_notice("QUERY_FACILITY_LOCK:enabled GET_SIM_STATUS.Pin1:disabled.");
                    cmsLog_notice(" Fixme");
                }
                break;

            case RIL_PINSTATE_ENABLED_NOT_VERIFIED:
            case RIL_PINSTATE_ENABLED_VERIFIED:
            case RIL_PINSTATE_ENABLED_BLOCKED:
            case RIL_PINSTATE_ENABLED_PERM_BLOCKED:
                if (!uiccCardApp->mIccLockEnabled) 
                {
                    cmsLog_notice("QUERY_FACILITY_LOCK:disabled GET_SIM_STATUS.Pin1:enabled.");
                    cmsLog_notice(" Fixme");
                }
                break;

            case RIL_PINSTATE_UNKNOWN:
            default:
                cmsLog_notice("Ignoring: pin1state=%d", uiccCardApp->mAppStatus.pin1);
                break;
        }
    } 
    else 
    {
        cmsLog_notice("Bogus facility lock response\n");
    }
}

static void fetch_records(int index)
{
    RIL_AppStatus *uiccApp = &(mIccCardStatus.applications[index].mAppStatus);
    struct internal_message *msg;

    cmsLog_debug("index=%d", index);

    msg = internal_message_obtain(EVENT_GET_IMSI_DONE, UICC_CONTROLLER);

    Object *o = object_obtain();
    object_setData(o, &index, sizeof(int));
    internal_message_setObject(msg, o);
    o = NULL;

    ril_access_get_imsi_for_app(uiccApp->aid_ptr, msg);
}

static void on_new_sms_on_sim(AsyncResult *ar)
{
    assert(ar);

    if(ar->error == 0)
    {
        int ints_length = (object_dataLength)(ar->result)/sizeof(int);
        int* ints = (int *)(object_getData)(ar->result);
        struct internal_message* msg = internal_message_obtain(
                            EVENT_GET_SMS_DONE, UICC_CONTROLLER);
        if(ints_length == 1) 
        {
            icc_file_handler_loadEFLinearFixed(EF_SMS, ints[0], NULL, msg);
        }
    }
}

static void on_get_msisdn_done(Object *o)
{
    AdnRecord *adn;
    int str_len = 0;

    assert(o);

    adn = (AdnRecord *)object_getData(o);

    str_len = cmsUtl_strlen(adn->mNumber);
    if(str_len > 0)
    {
        if(mMsisdn != NULL)
        {
            free(mMsisdn);
            mMsisdn = NULL;
        }
        mMsisdn = malloc(str_len + 1);
        assert(mMsisdn);
        cmsUtl_strncpy(mMsisdn, adn->mNumber, str_len+1);
        cmsLog_debug("new msisdn is %s", mMsisdn);
    }
}


RIL_CardState uicc_controller_get_card_status()
{
    return mIccCardStatus.card_state; 
}

const char* uicc_controller_get_msisdn()
{
    return mMsisdn;
}
