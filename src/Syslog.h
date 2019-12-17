#ifndef SYSLOG_H
#define SYSLOG_H

#include <cstdarg>
#include <cinttypes>
#include <WString.h>
#include <IPAddress.h>
#include <Udp.h>

// undefine ugly logf macro from avr's math.h
// this fix compilation errors on AtmelAVR platforms
#if defined(logf)
#undef logf
#endif

// compatibility with other platforms
// add missing vsnprintf_P method
#if !defined(ARDUINO_ARCH_AVR) && !defined(ARDUINO_ARCH_ESP8266) && !defined(vsnprintf_P) && !defined(ESP8266)
#define vsnprintf_P(buf, len, fmt, args) vsnprintf((buf), (len), (fmt), (args))
#endif

#define SYSLOG_NILVALUE "-"

// Syslog protocol format
#define SYSLOG_PROTO_IETF 0  // RFC 5424
#define SYSLOG_PROTO_BSD 1   // RFC 3164

/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
#define LOG_EMERG       0U /* system is unusable */
#define LOG_ALERT       1U /* action must be taken immediately */
#define LOG_CRIT        2U /* critical conditions */
#define LOG_ERR         3U /* error conditions */
#define LOG_WARNING     4U /* warning conditions */
#define LOG_NOTICE      5U /* normal but significant condition */
#define LOG_INFO        6U /* informational */
#define LOG_DEBUG       7U /* debug-level messages */

#define LOG_PRIMASK 0x07U  /* mask to extract priority part (internal) */
/* extract priority */
#define LOG_PRI(p)  ((p) & LOG_PRIMASK)
#define LOG_MAKEPRI(fac, pri) (((fac) << 3U) | (pri))

/* facility codes */
#define LOG_KERN        (0U<<3U)  /* kernel messages */
#define LOG_USER        (1U<<3U)  /* random user-level messages */
#define LOG_MAIL        (2U<<3U)  /* mail system */
#define LOG_DAEMON      (3U<<3U)  /* system daemons */
#define LOG_AUTH        (4U<<3U)  /* security/authorization messages */
#define LOG_SYSLOG      (5U<<3U)  /* messages generated internally by syslogd */
#define LOG_LPR         (6U<<3U)  /* line printer subsystem */
#define LOG_NEWS        (7U<<3U)  /* network news subsystem */
#define LOG_UUCP        (8U<<3U)  /* UUCP subsystem */
#define LOG_CRON        (9U<<3U)  /* clock daemon */
#define LOG_AUTHPRIV    (10<<3U) /* security/authorization messages (private) */
#define LOG_FTP         (11U<<3U) /* ftp daemon */

/* other codes through 15 reserved for system use */
#define LOG_LOCAL0      (16U<<3U) /* reserved for local use */
#define LOG_LOCAL1      (17U<<3U) /* reserved for local use */
#define LOG_LOCAL2      (18U<<3U) /* reserved for local use */
#define LOG_LOCAL3      (19U<<3U) /* reserved for local use */
#define LOG_LOCAL4      (20U<<3U) /* reserved for local use */
#define LOG_LOCAL5      (21U<<3U) /* reserved for local use */
#define LOG_LOCAL6      (22U<<3U) /* reserved for local use */
#define LOG_LOCAL7      (23U<<3U) /* reserved for local use */

#define LOG_NFACILITIES 24U  /* current number of facilities */
#define LOG_FACMASK 0x03f8U  /* mask to extract facility part */
/* facility of pri */
#define LOG_FAC(p)  (((p) & LOG_FACMASK) >> 3U)

#define LOG_MASK(pri)  (1U << (pri))    /* mask for one priority */
#define LOG_UPTO(pri)  ((1U << ((pri)+1)) - 1)    /* all priorities through pri */

class Syslog {
private:
    UDP *_client;
    uint8_t _protocol;
    IPAddress _ip;
    const char *_server;
    uint16_t _port;
    const char *_deviceHostname;
    const char *_appName;
    uint16_t _priDefault;
    uint8_t _priMask = 0xff;
    bool _useFakeDatetime = false;

    bool _sendLog(uint16_t pri, const char *message);
    bool _sendLog(uint16_t pri, const __FlashStringHelper *message);

public:
    explicit Syslog(UDP &client, uint8_t protocol = SYSLOG_PROTO_IETF);
    Syslog(UDP &client, const char *server, uint16_t port, const char *deviceHostname = SYSLOG_NILVALUE,
           const char *appName = SYSLOG_NILVALUE, uint16_t priDefault = LOG_KERN, uint8_t protocol = SYSLOG_PROTO_IETF);
    Syslog(UDP &client, const IPAddress& ip, uint16_t port, const char *deviceHostname = SYSLOG_NILVALUE, const char *appName = SYSLOG_NILVALUE,
           uint16_t priDefault = LOG_KERN, uint8_t protocol = SYSLOG_PROTO_IETF);

    Syslog &server(const char *server, uint16_t port);
    Syslog &server(const IPAddress& ip, uint16_t port);
    Syslog &deviceHostname(const char *deviceHostname);
    Syslog &appName(const char *appName);
    Syslog &defaultPriority(uint16_t pri = LOG_KERN);
    Syslog &setUseFakeDatetime(bool used);

    Syslog &logMask(uint8_t priMask);

    bool log(uint16_t pri, const __FlashStringHelper *message);
    bool log(uint16_t pri, const String &message);
    bool log(uint16_t pri, const char *message);

    bool vlogf(uint16_t pri, const char *fmt, va_list args) __attribute__((format(printf, 3, 0)));
    bool vlogf_P(uint16_t pri, PGM_P fmt_P, va_list args) __attribute__((format(printf, 3, 0)));

    bool logf(uint16_t pri, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
    bool logf(const char *fmt, ...) __attribute__((format(printf, 2, 3)));

    bool logf_P(uint16_t pri, PGM_P fmt_P, ...) __attribute__((format(printf, 3, 4)));
    bool logf_P(PGM_P fmt_P, ...) __attribute__((format(printf, 2, 3)));

    bool log(const __FlashStringHelper *message);
    bool log(const String &message);
    bool log(const char *message);
};

#endif
