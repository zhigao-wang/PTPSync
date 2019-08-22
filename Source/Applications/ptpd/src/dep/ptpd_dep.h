/**
 * @file   ptpd_dep.h
 * 
 * @brief  External definitions for inclusion elsewhere.
 * 
 * 
 */

#ifndef PTPD_DEP_H_
#define PTPD_DEP_H_

 /** \name System messages*/
 /**\{*/

#define ERROR(x, ...)  message(LOG_ERR, x, ##__VA_ARGS__)

#if defined(__WIN32__)
#define PERROR(x, ...) do { \
    TCHAR errbuf[200+1]; \
    DWORD errcode = GetLastError(); \
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode, 0, errbuf, 200, NULL); \
    message(LOG_ERR, x ": %s\n", ##__VA_ARGS__, errbuf); \
} while(0)
#else
#define PERROR(x, ...) message(LOG_ERR, x ": %m\n", ##__VA_ARGS__)
#endif


#define NOTIFY(x, ...) message(LOG_NOTICE, x, ##__VA_ARGS__)
#define INFO(x, ...)   message(LOG_INFO, x, ##__VA_ARGS__)

/** \}*/

/** \name Debug messages*/
 /**\{*/

extern BOOL show_debug_messages;

#ifndef PTPD_DBG
#define PTPD_DBG
#define PTPD_DBGV
#define DBG(x, ...) if (show_debug_messages) message(LOG_DEBUG, x, ##__VA_ARGS__)
#define DBGV(x, ...) if (show_debug_messages) message(LOG_DEBUG, x, ##__VA_ARGS__)
#endif

//#ifdef PTPD_DBGV
//#define PTPD_DBG
//#define DBGV(x, ...) message(LOG_DEBUG, x, ##__VA_ARGS__)
//#else
//#define DBGV(x, ...)
//#endif

//#ifdef PTPD_DBG
//#define DBG(x, ...) message(LOG_DEBUG, x, ##__VA_ARGS__)
//#else
//#define DBG(x, ...)
//#endif

/** \}*/

/** \name Endian corrections*/
 /**\{*/

#if defined(PTPD_MSBF)
#define shift8(x,y)   ( (x) << ((3-y)<<3) )
#define shift16(x,y)  ( (x) << ((1-y)<<4) )
#elif defined(PTPD_LSBF)
#define shift8(x,y)   ( (x) << ((y)<<3) )
#define shift16(x,y)  ( (x) << ((y)<<4) )
#endif

#define flip16(x) htons(x)
#define flip32(x) htonl(x)

/* i don't know any target platforms that do not have htons and htonl,
   but here are generic funtions just in case */
/*
#if defined(PTPD_MSBF)
#define flip16(x) (x)
#define flip32(x) (x)
#elif defined(PTPD_LSBF)
static inline Integer16 flip16(Integer16 x)
{
   return (((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00);
}

static inline Integer32 flip32(x)
{
  return (((x) >> 24) & 0x000000ff) | (((x) >> 8 ) & 0x0000ff00) |
         (((x) << 8 ) & 0x00ff0000) | (((x) << 24) & 0xff000000);
}
#endif
*/

/** \}*/


/** \name Bit array manipulations*/
 /**\{*/

#define getFlag(x,y)  !!( *(UInteger8*)((x)+((y)<8?1:0)) &   (1<<((y)<8?(y):(y)-8)) )
#define setFlag(x,y)    ( *(UInteger8*)((x)+((y)<8?1:0)) |=   1<<((y)<8?(y):(y)-8)  )
#define clearFlag(x,y)  ( *(UInteger8*)((x)+((y)<8?1:0)) &= ~(1<<((y)<8?(y):(y)-8)) )
/** \}*/

/** \name msg.c
 *-Pack and unpack PTP messages */
 /**\{*/

void msgUnpackHeader(Octet*,MsgHeader*);
void msgUnpackAnnounce (Octet*,MsgAnnounce*);
void msgUnpackSync(Octet*,MsgSync*);
void msgUnpackFollowUp(Octet*,MsgFollowUp*);
void msgUnpackPDelayReq(Octet*,MsgPDelayReq*);
void msgUnpackPDelayResp(Octet*,MsgPDelayResp*);
void msgUnpackPDelayRespFollowUp(Octet*,MsgPDelayRespFollowUp*);
void msgUnpackManagement(Octet*,MsgManagement*);
UInteger8 msgUnloadManagement(Octet*,MsgManagement*,PtpClock*,RunTimeOpts*);
void msgUnpackManagementPayload(Octet *buf, MsgManagement *manage);
void msgPackHeader(Octet*,PtpClock*);
void msgPackAnnounce(Octet*,PtpClock*);
void msgPackSync(Octet*,Timestamp*,PtpClock*);
void msgPackFollowUp(Octet*,Timestamp*,PtpClock*);
void msgPackPDelayReq(Octet*,Timestamp*,PtpClock*);
void msgPackPDelayResp(Octet*,MsgHeader*,Timestamp*,PtpClock*);
void msgPackPDelayRespFollowUp(Octet*,MsgHeader*,Timestamp*,PtpClock*);
UInteger16 msgPackManagement(Octet*,MsgManagement*,PtpClock*);
UInteger16 msgPackManagementResponse(Octet*,MsgHeader*,MsgManagement*,PtpClock*);

void msgDump(PtpClock *ptpClock);
void msgDebugHeader(MsgHeader *header);
void msgDebugSync(MsgSync *sync);
void msgDebugAnnounce(MsgAnnounce *announce);
void msgDebugDelayReq(MsgDelayReq *req);
void msgDebugFollowUp(MsgFollowUp *follow);
void msgDebugDelayResp(MsgDelayResp *resp);
void msgDebugManagement(MsgManagement *manage);

/** \}*/

/** \name net.c (OS API dependent)
 * -Init network stuff, send and receive datas*/
 /**\{*/

Boolean netInit(NetPath*,RunTimeOpts*,PtpClock*);
Boolean netShutdown(NetPath*);
int netSelect(TimeInternal*,NetPath*);
ssize_t netRecvEvent(Octet*,TimeInternal*,NetPath*);
ssize_t netRecvGeneral(Octet*,TimeInternal*,NetPath*);
ssize_t netSendEvent(Octet*,UInteger16,NetPath*);
ssize_t netSendGeneral(Octet*,UInteger16,NetPath*);
ssize_t netSendPeerGeneral(Octet*,UInteger16,NetPath*);
ssize_t netSendPeerEvent(Octet*,UInteger16,NetPath*);
#if defined(SOCKETS_WSA2)
Boolean netInitWSA2(NetPath * netPath);
Boolean netShutdownWSA2(NetPath * netPath);
int netSelectWSA2(TimeInternal * timeout, NetPath * netPath);
ssize_t netRecvEventWSA2(Octet *buf, TimeInternal *time, NetPath * netPath);
ssize_t netRecvGeneralWSA2(Octet * buf, NetPath * netPath);
#endif
#if defined(__WIN32__)
int inet_aton(const char* cp, struct in_addr* inp);
Boolean netFindInterfaceWin32(NetPath * netPath, Octet * ifaceName, UInteger8 * communicationTechnology,
    Octet * uuid, struct in_addr * ifaceAddr);
#endif
#if defined(TIMESTAMPING_PCAP)
Boolean netInitTimestampsPcap(NetPath *netPath, RunTimeOpts * rtOpts);
ssize_t netRecvEventPcap(Octet *buf, TimeInternal *time, NetPath * netPath);
#endif

/** \}*/

/** \name servo.c
 * -Clock servo*/
 /**\{*/

void initClock(RunTimeOpts*,PtpClock*);
void updatePeerDelay (one_way_delay_filter*, RunTimeOpts*,PtpClock*,TimeInternal*,Boolean);
void updateDelay (one_way_delay_filter*, RunTimeOpts*, PtpClock*,TimeInternal*);
void updateOffset(TimeInternal*,TimeInternal*,
  offset_from_master_filter*,RunTimeOpts*,PtpClock*,TimeInternal*);
void updateClock(RunTimeOpts*,PtpClock*);
/** \}*/

/** \name startup.c (OS API dependent)
 * -Handle with runtime options*/
 /**\{*/
int logToFile(void);
int recordToFile(void);
PtpClock * ptpdStartup(int,char**,Integer16*,RunTimeOpts*);
void ptpdShutdown(void);
/** \}*/

/** \name sys.c (OS API dependent)
 * -Manage timing system API*/
 /**\{*/
void message(int priority, const char *format, ...);
void displayStats(RunTimeOpts *rtOpts, PtpClock *ptpClock);
Boolean nanoSleep(TimeInternal*);
void getTime(TimeInternal*);
void setTime(TimeInternal*);
double getRand(void);
Boolean adjFreq(Integer32);
#if defined(__APPLE__)
void 	adjTime(Integer32);
#endif /* __APPLE__ */
#if defined(__WIN32__)
void getTimeWin32(TimeInternal * time);
void setTimeWin32(TimeInternal * time);
Boolean adjFreqWin32(Integer32 adj);
Boolean EnableSystemTimeAdjustment(void);
#endif

/** \}*/

/** \name timer.c (OS API dependent)
 * -Handle with timers*/
 /**\{*/
void initTimer(void);
void timerUpdate(IntervalTimer*);
void timerStop(UInteger16,IntervalTimer*);
void timerStart(UInteger16,UInteger16,IntervalTimer*);
Boolean timerExpired(UInteger16,IntervalTimer*);
#if defined(__WIN32__)
int setitimerWin32(const struct timeval * tv, void * callback);
#endif
/** \}*/


/*Test functions*/


#endif /*PTPD_DEP_H_*/
