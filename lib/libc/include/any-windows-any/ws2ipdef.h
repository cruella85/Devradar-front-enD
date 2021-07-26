/**
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 */
#ifndef _WS2IPDEF_
#define _WS2IPDEF_

#include <_mingw_unicode.h>
#include <winapifamily.h>

#ifdef __LP64__
#pragma push_macro("u_long")
#undef u_long
#define u_long __ms_u_long
#endif

#include <in6addr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* options at IPPROTO_IP level */
#define IP_OPTIONS                 1
#define IP_HDRINCL                 2
#define IP_TOS                     3
#define IP_TTL                     4
#define IP_MULTICAST_IF            9
#define IP_MULTICAST_TTL          10
#define IP_MULTICAST_LOOP         11
#define IP_ADD_MEMBERSHIP         12
#define IP_DROP_MEMBERSHIP        13
#define IP_DONTFRAGMENT           14
#define IP_ADD_SOURCE_MEMBERSHIP  15
#define IP_DROP_SOURCE_MEMBERSHIP 16
#define IP_BLOCK_SOURCE           17
#define IP_UNBLOCK_SOURCE         18
#define IP_PKTINFO                19
#define IP_HOPLIMIT               21
#define IP_RECVTTL                21
#define IP_RECEIVE_BROADCAST      22
#define IP_RECVIF                 24
#define IP_RECVDSTADDR            25
#define IP_IFLIST                 28
#define IP_ADD_IFLIST             29
#define IP_DEL_IFLIST             30
#define IP_UNICAST_IF             31
#define IP_RTHDR                  32
#define IP_GET_IFLIST             33
#define IP_RECVRTHDR              38
#define IP_TCLASS                 39
#define IP_RECVTCLASS             40
#define IP_RECVTOS                40
#define IP_ORIGINAL_ARRIVAL_IF    47
#define IP_ECN                    50
#define IP_PKTINFO_EX             51
#define IP_WFP_REDIRECT_RECORDS   60
#define IP_WFP_REDIRECT_CONTEXT   70
#define IP_MTU_DISCOVER           71
#define IP_MTU                    73
#define IP_NRT_INTERFACE          74
#define IP_RECVERR                75
#define IP_USER_MTU               76

#define IP_UNSPECIFIED_TYPE_OF_SERVICE -1
#define IP_UNSPECIFIED_USER_MTU MAXULONG

#define IPV6_ADDRESS_BITS RTL_BITS_OF(IN6_ADDR)

/* options at IPPROTO_IPV6 level */
#define IPV6_HOPOPTS              1
#define IPV6_HDRINCL              2
#define IPV6_UNICAST_HOPS         4
#define IPV6_MULTICAST_IF         9
#define IPV6_MULTICAST_HOPS       10
#define IPV6_MULTICAST_LOOP       11
#define IPV6_ADD_MEMBERSHIP       12
#define IPV6_JOIN_GROUP           IPV6_ADD_MEMBERSHIP
#define IPV6_DROP_MEMBERSHIP      13
#define IPV6_LEAVE_GROUP          IPV6_DROP_MEMBERSHIP
#define IPV6_DONTFRAG             14
#define IPV6_PKTINFO              19
#define IPV6_HOPLIMIT             21
#define IPV6_PROTECTION_LEVEL     23
#define IPV6_RECVIF               24
#define IPV6_RECVDSTADDR          25
#define IPV6_CHECKSUM             26
#define IPV6_V6ONLY               27
#define IPV6_IFLIST               28
#define IPV6_ADD_IFLIST           29
#define IPV6_DEL_IFLIST           30
#define IPV6_UNICAST_IF           31
#define IPV6_RTHDR                32
#define IPV6_GET_IFLIST           33
#define IPV6_RECVRTHDR            38
#define IPV6_TCLASS               39
#define IPV6_RECVTCLASS           40
#define IPV6_ECN                  50
#define IPV6_PKTINFO_EX           51
#define IPV6_WFP_REDIRECT_RECORDS 60
#define IPV6_WFP_REDIRECT_CONTEXT 70
#defin