#ifndef TRACE_H
#define TRACE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* allow n different sets of debug messages to be independently enabled */

#if defined(DEBUG1) || defined(DEBUG2) || defined(DEBUG3) || defined(DEBUG4)
#   define DEBUGX
#   if defined(NATFEATS)
#       include <mint/arch/nf_ops.h>
#       define TRPRINT(...) nf_debugprintf(__VA_ARGS__)
#   else
extern FILE	*xyz_trace_file; 
#       define TROUT xyz_trace_file
#       define TRPRINT(...) \
        do { \
            if (TROUT != NULL) { \
                fprintf(TROUT, __VA_ARGS__); \
                fflush(TROUT); \
            } \
        } while (0);

#       define TRFILEW   (20)
#       define TRFUNCW   (20)
#   endif

#   define TRACEV(fmt, ...) \
    do { \
        TRPRINT("%-*s ", TRFILEW, __FILE__); \
        TRPRINT("%-*s ", TRFUNCW, __func__); \
        TRPRINT("%-4d: ", __LINE__); \
        TRPRINT(fmt "\n", __VA_ARGS__); \
    } while(0);
#   define TRACE(msg) TRACEV("%s", msg "")
#endif

#if defined(DEBUG1)
#   define tracev1(fmt, ...)   TRACEV(fmt, __VA_ARGS__)
#   define trace1(msg)         TRACEV("%s", msg "")
#else
#   define tracev1(fmt, ...)
#   define trace1(msg)
#endif

#if defined(DEBUG2)
#   define tracev2(fmt, ...)   TRACEV(fmt, __VA_ARGS__)
#   define trace2(msg)         TRACEV("%s", msg "")
#else
#   define tracev2(fmt, ...)
#   define trace2(msg)
#endif

#if defined(DEBUG3)
#   define tracev3(fmt, ...)   TRACEV(fmt, __VA_ARGS__)
#   define trace3(msg)         TRACEV("%s", msg "")
#else
#   define tracev3(fmt, ...)
#   define trace3(msg)
#endif

#if defined(DEBUG4)
#   define tracev4(fmt, ...)   TRACEV(fmt, __VA_ARGS__)
#   define trace4(msg)         TRACEV("%s", msg "")
#else
#   define tracev4(fmt, ...)
#   define trace4(msg)
#endif

#define NOT_IMPLEMENTED(reason)     trace1("NOT IMPLEMENTED: " reason)

#define abend(msg) \
    do { \
        tracev1("%s", strerror(errno)); \
        fprintf(stderr, "%s %s %4d: %s %s\n", __FILE__, __func__, __LINE__, msg "",  strerror(errno)); \
        exit(EXIT_FAILURE); \
    } while(0);

#endif
