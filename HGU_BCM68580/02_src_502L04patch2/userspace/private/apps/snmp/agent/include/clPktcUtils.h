/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/
#ifndef CLPKTCUTILS_H
#define CLPKTCUTILS_H

#include <dal_voice.h>

#define CLPKTCUTILS_LOCALBUF_SIZE 6000 /* Max size of MDM read/write */
extern char localBuf[];    /* size: CLPKTCUTILS_LOCALBUF_SIZE */
extern long localInt;
extern char localAddr[];        /* size: CLPKTCUTILSINETADDRESSDNS_SIZE */

/*
InetPortNumber ::= TEXTUAL-CONVENTION
    DISPLAY-HINT "d"
    STATUS       current
    DESCRIPTION
        "Represents a 16 bit port number of an Internet transport
         layer protocol.  Port numbers are assigned by IANA.  A
         current list of all assignments is available from
         <http://www.iana.org/>.

         The value zero is object-specific and must be defined as
         part of the description of any object that uses this
         syntax.  Examples of the usage of zero might include
         situations where a port number is unknown, or when the
         value zero is used as a wildcard in a filter."
    REFERENCE   "STD 6 (RFC 768), STD 7 (RFC 793) and RFC 2960"
    SYNTAX       Unsigned32 (0..65535)
 */
#define CLPKTCUTILSINETPORTNUMBER_MIN 0
#define CLPKTCUTILSINETPORTNUMBER_MAX 65535

/*
    InetAddressDNS ::= TEXTUAL-CONVENTION
    DISPLAY-HINT "255a"
    STATUS       current
    DESCRIPTION
        "Represents a DNS domain name.  The name SHOULD be fully
         qualified whenever possible.

         The corresponding InetAddressType is dns(16).

         The DESCRIPTION clause of InetAddress objects that may have
         InetAddressDNS values MUST fully describe how (and when)
         these names are to be resolved to IP addresses.

         The resolution of an InetAddressDNS value may require to
         query multiple DNS records (e.g., A for IPv4 and AAAA for
         IPv6).  The order of the resolution process and which DNS
         record takes precedence depends on the configuration of the
         resolver.

         This textual convention SHOULD NOT be used directly in object
         definitions, as it restricts addresses to a specific format.
         However, if it is used, it MAY be used either on its own or in
         conjunction with InetAddressType, as a pair."
    SYNTAX       OCTET STRING (SIZE (1..255))
 */
#define CLPKTCUTILSINETADDRESSDNS_SIZE 256

/*
TruthValue ::= TEXTUAL-CONVENTION
    STATUS       current
    DESCRIPTION
            "Represents a boolean value."
    SYNTAX       INTEGER { true(1), false(2) }
*/
typedef enum {
   CLPKTCUTILSTRUTHVALUE_TRUE    = 1,
   CLPKTCUTILSTRUTHVALUE_FALSE   = 2,

   CLPKTCUTILSTRUTHVALUE_MAX     /* INTERNAL */
} clPktcUtilsTruthValue;

/*
  PktcEUETCID ::= TEXTUAL-CONVENTION
      STATUS  current
      DESCRIPTION
          " This TEXTUAL CONVENTION is being defined
            to contain identities that can be used
            within the PacketCable eUE data models.

            It specifies a hex string that can be
            used to represent the various identities.

            The types of possible identities are
            specified by the TEXTUAL CONVENTION
            'PktcEUETCIDType'.

            The following rules apply:
              - All identities, except macaddress refer
                to either UEs or Users.
                Mac addresses are UE specific
              - When used as a pair, the public and
                private identities MUST be separated
                by a '#', with the private identity
                following the public identity."
      SYNTAX   OCTET STRING(SIZE(0..1023))
*/
#define CLPKTCEUETCID_SIZE 1024

/*
  PktcEUETCCreds ::= TEXTUAL-CONVENTION
      STATUS  current
      DESCRIPTION
          " This TEXTUAL CONVENTION allows for the definition
            of a credential.

            A PktcEUETCCreds value must always be associated with
            and interpreted within the context of a corresponding
            PktcEUETCCredsType.

            The value of a PktcEUETCCreds object must be consistent
            with the value of its associated PktcEUETCCredsType
            object. Any attempt to SET an object when these values
            are not consistent must fail with an inconsistentValue
            error.

            An object of this type MUST be interpreted as follows
            (in network byte order):

            Bytes 0-1:  Reserved. The application must define the
                        usage of these bytes, otherwise, ignored.
            Bytes 2-3:  Indicate the length of the credential value.
            Bytes 4-8191: Contain the credential value."
      SYNTAX   OCTET STRING (SIZE (0..8192))
 */
#define CLPKTCEUETCCREDS_SIZE 8192

/*
SnmpAdminString ::= TEXTUAL-CONVENTION
    DISPLAY-HINT "255t"
    STATUS       current
    DESCRIPTION "An octet string containing administrative
                 information, preferably in human-readable form.

                 To facilitate internationalization, this
                 information is represented using the ISO/IEC
                 IS 10646-1 character set, encoded as an octet
                 string using the UTF-8 transformation format
                 described in [RFC2279].

                 Since additional code points are added by
                 amendments to the 10646 standard from time
                 to time, implementations must be prepared to
                 encounter any code point from 0x00000000 to
                 0x7fffffff.  Byte sequences that do not
                 correspond to the valid UTF-8 encoding of a
                 code point or are outside this range are
                 prohibited.

                 The use of control codes should be avoided.

                 When it is necessary to represent a newline,
                 the control code sequence CR LF should be used.

                 The use of leading or trailing white space should
                 be avoided.

                 For code points not directly supported by user
                 interface hardware or software, an alternative
                 means of entry and display, such as hexadecimal,
                 may be provided.

                 For information encoded in 7-bit US-ASCII,
                 the UTF-8 encoding is identical to the
                 US-ASCII encoding.

                 UTF-8 may require multiple bytes to represent a
                 single character / code point; thus the length
                 of this object in octets may be different from
                 the number of characters encoded.  Similarly,
                 size constraints refer to the number of encoded
                 octets, not the number of characters represented
                 by an encoding.

                 Note that when this TC is used for an object that
                 is used or envisioned to be used as an index, then
                 a SIZE restriction MUST be specified so that the
                 number of sub-identifiers for any object instance
                 does not exceed the limit of 128, as defined by
                 [RFC3416].

                 Note that the size of an SnmpAdminString object is
                 measured in octets, not characters.
                "
    SYNTAX       OCTET STRING (SIZE (0..255))
*/
#define SNMPADMINSTRING_SIZE 256

/*
InetAddressType ::= TEXTUAL-CONVENTION
    STATUS      current
    DESCRIPTION
        "A value that represents a type of Internet address.

         unknown(0)  An unknown address type.  This value MUST
                     be used if the value of the corresponding
                     InetAddress object is a zero-length string.
                     It may also be used to indicate an IP address
                     that is not in one of the formats defined
                     below.

         ipv4(1)     An IPv4 address as defined by the
                     InetAddressIPv4 textual convention.

         ipv6(2)     An IPv6 address as defined by the
                     InetAddressIPv6 textual convention.

         ipv4z(3)    A non-global IPv4 address including a zone
                     index as defined by the InetAddressIPv4z
                     textual convention.

         ipv6z(4)    A non-global IPv6 address including a zone
                     index as defined by the InetAddressIPv6z
                     textual convention.

         dns(16)     A DNS domain name as defined by the
                     InetAddressDNS textual convention.

         Each definition of a concrete InetAddressType value must be
         accompanied by a definition of a textual convention for use
         with that InetAddressType.

         To support future extensions, the InetAddressType textual
         convention SHOULD NOT be sub-typed in object type definitions.
         It MAY be sub-typed in compliance statements in order to
         require only a subset of these address types for a compliant
         implementation.

         Implementations must ensure that InetAddressType objects
         and any dependent objects (e.g., InetAddress objects) are
         consistent.  An inconsistentValue error must be generated
         if an attempt to change an InetAddressType object would,
         for example, lead to an undefined InetAddress value.  In
         particular, InetAddressType/InetAddress pairs must be
         changed together if the address type changes (e.g., from
         ipv6(2) to ipv4(1))."
    SYNTAX       INTEGER {
                     unknown(0),
                     ipv4(1),
                     ipv6(2),
                     ipv4z(3),
                     ipv6z(4),
                     dns(16)
                 }
 */
typedef enum {
   CLPKTCUTILSINETADDRTYPE_UNKNOWN = 0,
   CLPKTCUTILSINETADDRTYPE_IPV4    = 1,
   CLPKTCUTILSINETADDRTYPE_IPV6    = 2,
   CLPKTCUTILSINETADDRTYPE_IPV4Z   = 3,
   CLPKTCUTILSINETADDRTYPE_IPV6Z   = 4,
   CLPKTCUTILSINETADDRTYPE_DNS     = 16,
} clPktcUtilsInetAddressType;

/*
  PktcEUETCIDType ::= TEXTUAL-CONVENTION
      STATUS  current
      DESCRIPTION
          " This TEXTUAL CONVENTION is being defined
            as a way of indicating an identity
            specified by MIB Objects utilizing the
            TEXTUAL CONVENTION 'PktcEUETCID'.

            The defined types include:
              - other(1)
                  for types not described by the options
                  provided below
              - gruu(2)
                  for Globally Routable User Agent (UA) URIs
              - publicIdentity(3)
                  for Public Identities as defined by PacketCable
              - privateIdentity(4)
                  for Private Identities as defined by PacketCable
              - publicPrivatePair(5)
                  for Public and Private Identity pairs
                  as defined by PacketCable
              - username(6)
                  for username and password as defined by PacketCable
              - macaddress(7)
                  for mac addresses
              - packetcableIdentity(8)
                  for PacketCable specific types

            UE implementations must ensure that
            PktcEUETCIDType objects and any dependent
            objects (e.g., PktcEUETCID objects) are
            consistent.

            In general, the UE MUST generate an
            'inconsistentValue' error if an attempt
            to change a PktcEUETCIDType object would,
            for example, lead to an undefined PktcEUETCID
            value.
            In particular, PktcEUETCIDType/PktcEUEID pairs
            MUST be changed together."
      SYNTAX   INTEGER {
                        other(1),
                        gruu(2),
                        publicIdentity(3),
                        privateIdentity(4),
                        publicPrivatePair(5),
                        username(6),
                        macaddress(7),
                        packetcableIdentity(8)
                       }
*/
typedef enum {
   CLPKTCEUETCIDTYPE_MIN = 0,        /* INTERNAL */

   CLPKTCEUETCIDTYPE_OTHER                = 1,
   CLPKTCEUETCIDTYPE_GRUU                 = 2,
   CLPKTCEUETCIDTYPE_PUBLICIDENTITY       = 3,
   CLPKTCEUETCIDTYPE_PRIVATEIDENTITY      = 4,
   CLPKTCEUETCIDTYPE_PUBLICPRIVATEPAIR    = 5,
   CLPKTCEUETCIDTYPE_USERNAME             = 6,
   CLPKTCEUETCIDTYPE_MACADDRESS           = 7,
   CLPKTCEUETCIDTYPE_PACKETCABLEIDENTITY  = 8,

   CLPKTCEUETCIDTYPE_MAX,            /* INTERNAL */
} clPktcEUETCIDType;

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsAsnDataToInt()
**
** PURPOSE:     Extracts integer from integer data type.
**
** PARAMETERS:  pData   - buffer pointer
**              len     - length of integer data
**              pResult - Integer result
**
** RETURNS:     Returns SNMP error code
**
******************************************************************************/
int clPktcUtilsAsnDataToInt(char *pData, int len, long *pResult);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsGetInetAddressSize()
**
** PURPOSE:     Get the address size for the address type.
**
** PARAMETERS:  type - Address type
**
** RETURNS:     Address size
**
******************************************************************************/
int clPktcUtilsGetInetAddressSize(clPktcUtilsInetAddressType type);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsInetPortNumberWrite()
**
** PURPOSE:     Generic InetPortNumber write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsInetPortNumberWrite(int action,
                                   unsigned char *var_val,
                                   unsigned char var_val_type,
                                   int var_val_len,
                                   long *dest);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsInetAddressTypeWrite()
**
** PURPOSE:     Generic InetAddressType write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsInetAddressTypeWrite(int action,
                                    unsigned char *var_val,
                                    unsigned char var_val_type,
                                    int var_val_len,
                                    long *dest);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsInetAddressWrite()
**
** PURPOSE:     Generic InetAddress write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**             type           - (IN)  inet address type
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsInetAddressWrite(int action,
                                unsigned char *var_val,
                                unsigned char var_val_type,
                                int var_val_len,
                                char *dest,
                                clPktcUtilsInetAddressType type);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsTruthValueWrite()
**
** PURPOSE:     Generic TruthValue write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsTruthValueWrite(int action,
                               unsigned char *var_val,
                               unsigned char var_val_type,
                               int var_val_len,
                               long *dest);

/*****************************************************************************
**
** FUNCTION:    clPktcUtilsIntegerWrite()
**
** PURPOSE:     Generic Integer write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**             min            - (IN)  minimum value (inclusive)
**             max            - (IN)  maximum value (inclusive)
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsIntegerWrite(int action,
                            unsigned char *var_val,
                            unsigned char var_val_type,
                            int var_val_len,
                            long *dest,
                            int min,
                            int max);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsStringWrite()
**
** PURPOSE:    Generic String write function
**
** PARAMETERS: action         - (IN)  MIB parse phases
**             var_val        - (IN)  variable value
**             var_val_type   - (IN)  variable type
**             var_val_len    - (IN)  variable length
**             dest           - (OUT) destination to write to
**             dstSize        - (IN)  destination buffer size
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsStringWrite(int action,
                           unsigned char *var_val,
                           unsigned char var_val_type,
                           int var_val_len,
                           char *dest,
                           int dstSize);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsGetVoiceServiceParam()
**
** PURPOSE:    Function to fill DAL parms
**
** PARAMETERS: parms     - (OUT) Pointer to DAL parms struct
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsGetVoiceServiceParam(DAL_VOICE_PARMS *parms);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsGetSipNetworkParam()
**
** PURPOSE:    Function to fill DAL parms
**
** PARAMETERS: netIndex  - (IN)  Network index (0 indexed)
**             parms     - (OUT) Pointer to DAL parms struct
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsGetSipNetworkParam(int netIndex, DAL_VOICE_PARMS *parms);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsGetSipClientParam()
**
** PURPOSE:    Function to fill DAL parms
**
** PARAMETERS: clientIndex - (IN)  Client index (0 indexed)
**             parms       - (OUT) Pointer to DAL parms struct
**
** RETURNS:    SNMP error code
**
******************************************************************************/
int clPktcUtilsGetSipClientParam(int clientIndex, DAL_VOICE_PARMS *parms);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsGetNumSipClients()
**
** PURPOSE:    Function to get number of sip clients.
**
** PARAMETERS: None
**
** RETURNS:    Number of sip clients.  0 if an error occurred.
**
******************************************************************************/
int clPktcUtilsGetNumSipClients();

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsGetNumSipNetworks()
**
** PURPOSE:    Function to get number of sip networks.
**
** PARAMETERS: None
**
** RETURNS:    Number of sip networks.  0 if an error occurred.
**
******************************************************************************/
int clPktcUtilsGetNumSipNetworks();

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsInetAddressFromString()
**
** PURPOSE:    Convert address string to Inet Address type data
**
** PARAMETERS: string  - (IN)  Input string
**             type    - (IN)  Inet address type
**             addr    - (OUT) Inet Address type data
**
** RETURNS:    None
**
******************************************************************************/
void clPktcUtilsInetAddressFromString(const char *string,
                                      clPktcUtilsInetAddressType type,
                                      char *addr);

/*****************************************************************************
**
** FUNCTION:   clPktcUtilsInetAddressFromString()
**
** PURPOSE:    Convert address string to Inet Address type data
**
** PARAMETERS: type        - (IN)  Inet address type
**             addr        - (IN)  Inet Address type data
**             string      - (OUT) Output string
**             stringSize  - (IN)  Max string size
**
** RETURNS:    None
**
******************************************************************************/
void clPktcUtilsInetAddressToString(clPktcUtilsInetAddressType type,
                                    const char *addr,
                                    char *string,
                                    unsigned int stringSize);

#endif /* CLPKTCUTILS_H */
