/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _SVCGUID_
#define _SVCGUID_

#include <basetyps.h>

#define SVCID_HOSTNAME { 0x0002a800,0,0,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define SVCID_INET_HOSTADDRBYINETSTRING { 0x0002a801,0,0,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define SVCID_INET_SERVICEBYNAME { 0x0002a802,0,0,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define SVCID_INET_HOSTADDRBYNAME { 0x0002a803,0,0,{ 0xC0,0,0,0,0,0,0,0x46 } }

#define SVCID_TCP_RR(_Port,_RR) { (0x0009 << 16) | (_Port),0,_RR,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define SVCID_TCP(_Port) SVCID_TCP_RR(_Port,0)
#define SVCID_DNS(_RecordType) SVCID_TCP_RR(53,_RecordType)
#define IS_SVCID_DNS(_g) ((((_g)->Data1)==0x00090035) && (((_g)->Data2)==0) && (((_g)->Data4[0])==0xC0) && (((_g)->Data4[1])==0) && (((_g)->Data4[2])==0) && (((_g)->Data4[3])==0) && (((_g)->Data4[4])==0) && (((_g)->Data4[5])==0) && (((_g)->Data4[6])==0) && (((_g)->Data4[7])==0x46))
#define IS_SVCID_TCP(_g) (((((_g)->Data1) & 0xFFFF0000)==0x00090000) && (((_g)->Data2)==0) && (((_g)->Data4[0])==0xC0) && (((_g)->Data4[1])==0) && (((_g)->Data4[2])==0) && (((_g)->Data4[3])==0) && (((_g)->Data4[4])==0) && (((_g)->Data4[5])==0) && (((_g)->Data4[6])==0) && (((_g)->Data4[7])==0x46))
#define PORT_FROM_SVCID_TCP(_g) ((WORD)(_g->Data1 & 0xFFFF))
#define RR_FROM_SVCID(_RR) (_RR->Data3)
#define SET_TCP_SVCID_RR(_g,_Port,_RR) { (_g)->Data1 = (0x0009 << 16) | (_Port); (_g)->Data2 = 0; (_g)->Data3 = _RR; (_g)->Data4[0] = 0xC0; (_g)->Data4[1] = 0x0; (_g)->Data4[2] = 0x0; (_g)->Data4[3] = 0x0; (_g)->Data4[4] = 0x0; (_g)->Data4[5] = 0x0; (_g)->Data4[6] = 0x0; (_g)->Data4[7] = 0x46; }
#define SET_TCP_SVCID(_g,_Port) SET_TCP_SVCID_RR(_g,_Port,0)
#define SVCID_UDP_RR(_Port,_RR) { (0x000A << 16) | (_Port),0,_RR,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define SVCID_UDP(_Port) SVCID_UDP_RR(_Port,0)
#define IS_SVCID_UDP(_g) (((((_g)->Data1) & 0xFFFF0000)==0x000A0000) && (((_g)->Data2)==0) && (((_g)->Data4[0])==0xC0) && (((_g)->Data4[1])==0) && (((_g)->Data4[2])==0) && (((_g)->Data4[3])==0) && (((_g)->Data4[4])==0) && (((_g)->Data4[5])==0) && (((_g)->Data4[6])==0) && (((_g)->Data4[7])==0x46))
#define PORT_FROM_SVCID_UDP(_g) ((WORD)(_g->Data1 & 0xFFFF))
#define SET_UDP_SVCID_RR(_g,_Port,_RR) { (_g)->Data1 = (0x000A << 16) | (_Port); (_g)->Data2 = 0; (_g)->Data3 = _RR; (_g)->Data4[0] = 0xC0; (_g)->Data4[1] = 0x0; (_g)->Data4[2] = 0x0; (_g)->Data4[3] = 0x0; (_g)->Data4[4] = 0x0; (_g)->Data4[5] = 0x0; (_g)->Data4[6] = 0x0; (_g)->Data4[7] = 0x46; }
#define SET_UDP_SVCID(_g,_Port) SET_UDP_SVCID_RR(_g,_Port,0)
#define SVCID_NETWARE(_SapId) { (0x000B << 16) | (_SapId),0,0,{ 0xC0,0,0,0,0,0,0,0x46 } }
#define IS_SVCID_NETWARE(_g) (((((_g)->Data1) & 0xFFFF0000)==0x000B0000) && (((_g)->Data2)==0) && (((_g)->Data3)==0) && (((_g)->Data4[0])==0xC0) && (((_g)->Data4[1])==0) && (((_g)->Data4[2])==0) && (((_g)->Data4[3])==0) && (((_g)->Data4[4])==0) && (((_g)->Data4[5])==0) && (((_g)->Data4[6])==0) && (((_g)->Data4[7])==0x46))
#define SAPID_FROM_SVCID_NETWARE(_g) ((WORD)(_g->Data1 & 0xFFFF))
#define SET_NETWARE_SVCID(_g,_SapId) { (_g)->Data1 = (0x000B << 16) | (_SapId); (_g)->Data2 = 0; (_g)->Data3 = 0; (_g)->Data4[0] = 0xC0; (_g)->Data4[1] = 0x0; (_g)->Data4[2] = 0x0; (_g)->Data4[3] = 0x0; (_g)->Data4[4] = 0x0; (_g)->Data4[5] = 0x0; (_g)->Data4[6] = 0x0; (_g)->Data4[7] = 0x46; }

#define SVCID_ECHO_TCP SVCID_TCP(7)
#define SVCID_DISCARD_TCP SVCID_TCP(9)
#define SVCID_SYSTAT_TCP SVCID_TCP(11)
#define SVCID_SYSTAT_TCP SVCID_TCP(11)
#define SVCID_DAYTIME_TCP SVCID_TCP(13)
#define SVCID_NETSTAT_TCP SVCID_TCP(15)
#define SVCID_QOTD_TCP SVCID_TCP(17)
#define SVCID_CHARGEN_TCP SVCID_TCP(19)
#define SVCID_FTP_DATA_TCP SVCID_TCP(20)
#define SVCID_FTP_TCP SVCID_TCP(21)
#define SVCID_TELNET_TCP SVCID_TCP(23)
#define SVCID_SMTP_TCP SVCID_TCP(25)
#define SVCID_TIME_TCP SVCID_TCP(37)
#define SVCID_NAME_TCP SVCID_TCP(42)
#define SVCID_WHOIS_TCP SVCID_TCP(43)
#define SVCID_DOMAIN_TCP SVCID_TCP(53)
#define SVCID_NAMESERVER_TCP SVCID_TCP(53)
#define SVCID_MTP_TCP SVCID_TCP(57)
#define SVCID_RJE_TCP SVCID_TCP(77)
#define SVCID_FINGER_TCP SVCID_TCP(79)
#define SVCID_LINK_TCP SVCID_TCP(87)
#define SVCID_SUPDUP_TCP SVCID_TCP(95)
#define SVCID_HOSTNAMES_TCP SVCID_TCP(101)
#define SVCID_ISO_TSAP_TCP SVCID_TCP(102)
#define SVCID_DICTIONARY_TCP SVCID_TCP(103)
#define SVCID_X400_TCP SVCID_TCP(103)
#define SVCID_X400_SND_TCP SVCID_TCP(104)
#define SVCID_CSNET_NS_TCP SVCID_TCP(105)
#define SVCID_POP_TCP SVCID_TCP(109)
#define SVCID_POP2_TCP SVCID_TCP(109)
#define SVCID_POP3_TCP SVCID_TCP(110)
#define SVCID_PORTMAP_TCP SVCID_TCP(111)
#define SVCID_SUNRPC_TCP SVCID_TCP(111)
#define SVCID_AUTH_TCP SVCID_TCP(113)
#define SVCID_SFTP_TCP SVCID_TCP(115)
#define SVCID_PATH_TCP SVCID_TCP(117)
#define SVCID_UUCP_PATH_TCP SVCID_TCP(117)
#define SVCID_NNTP_TCP SVCID_TCP(119)
#define SVCID_NBSESSION_TCP SVCID_TCP(139)
#define SVCID_NEWS_TCP SVCID_TCP(144)
#define SVCID_TCPREPO_TCP SVCID_TCP(158)
#define SVCID_PRINT_SRV_TCP SVCID_TCP(170)
#define SVCID_VMNET_TCP SVCID_TCP(175)
#define SVCID_VMNET0_TCP SVCID_TCP(400)
#define SVCID_EXEC_TCP SVCID_TCP(512)
#define SVCID_LOGIN_TCP SVCID_TCP(513)
#define SVCID_SHELL_TCP SVCID_TCP(514)
#define SVCID_PRINTER_TCP SVCID_TCP(515)
#define SVCID_EFS_TCP SVCID_TCP(520)
#define SVCID_TEMPO_TCP SVCID_TCP(526)
#define SVCID_COURIER_TCP SVCID_TCP(530)
#define SVCID_CONFERENCE_TCP SVCID_TCP(531)
#define SVCID_NETNEWS_TCP SVCID_TCP(532)
#define SVCID_UUCP_TCP SVCID_TCP(540)
#define SVCID_KLOGIN_TCP SVCID_TCP(543)
#define SVCID_KSHELL_TCP SVCID_TCP(544)
#define SVCID_REMOTEFS_TCP SVCID_TCP(556)
#define SVCID_GARCON_TCP SVCID_TCP(600)
#define SVCID_MAITRD_TCP SVCID_TCP(601)
#define SVCID_BUSBOY_TCP SVCID_TCP(602)
#define SVCID_KERBEROS_TCP SVCID_TCP(750)
#define SVCID_KERBEROS_MASTER_TCP SVCID_TCP(751)
#define SVCID_KRB_PROP_TCP SVCID_TCP(754)
#define SVCID_ERLOGIN_TCP SVCID_TCP(888)
#define SVCID_KPOP_TCP SVCID_TCP(1109)
#define SVCID_INGRESLOCK_TCP SVCID_TCP(1524)
#define SVCID_KNETD_TCP SVCID_TCP(2053)
#define SVCID_EKLOGIN_TCP SVCID_TCP(2105)
#define SVCID_RMT_TCP SVCID_TCP(5555)
#define SVCID_MTB_TCP SVCID_TCP(5556)
#define SVCID_MAN_TCP SVCID_TCP(9535)
#define SVCID_W_TCP SVCID_TCP(9536)
#define SVCID_MANTST_TCP SVCID_TCP(9537)
#define SVCID_BNEWS_TCP SVCID_TCP(10000)
#define SVCID