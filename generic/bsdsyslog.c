
/* 
 * bsdsyslog.c --
 *
 *      Contains the Superconnect BSD syslog extensions for Tcl.
 *-----------------------------------------------------------------------------
 * Copyright 2006 Superconnect.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Superconnect makes
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 */

#include <tcl.h>

#include <string.h>

#define SYSLOG_NAMES
#include <sys/syslog.h>

#define TCLBSD_LOG_ERROR 1

/*-----------------------------------------------------------------------------
 * GetSyslogFacility --
 *  
 *  
 * Results:
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
static int
GetSyslogFacility (Tcl_Interp *interp, char *facilityName, int flags)
{
    CODE *codePtr;

    for (codePtr = facilitynames; codePtr->c_name != NULL; codePtr++) {
        if (strcmp (facilityName, codePtr->c_name) == 0) {
	    return codePtr->c_val;
	}
    }

    if (flags & TCLBSD_LOG_ERROR) {
       Tcl_AppendResult (interp, "bad value for facility: must be one of", NULL);

       for (codePtr = facilitynames; codePtr->c_name != NULL; codePtr++) {
           Tcl_AppendResult (interp, " '", codePtr->c_name, "'", NULL);
       }
    }
    return -1;
}

/* next COCI call april 5, 7 pm central
 * june 2, 3, 4 coronado island
 */

/*-----------------------------------------------------------------------------
 * GetSyslogPriority --
 *  
 *  
 * Results:
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
static int
GetSyslogPriority (Tcl_Interp *interp, char *priorityName, int flags)
{
    CODE *codePtr;

    for (codePtr = prioritynames; codePtr->c_name != NULL; codePtr++) {
        if (strcmp (priorityName, codePtr->c_name) == 0) {
	    return codePtr->c_val;
	}
    }

    if (flags & TCLBSD_LOG_ERROR) {
       Tcl_AppendResult (interp, "bad value for priority: must be one of", NULL);

       for (codePtr = prioritynames; codePtr->c_name != NULL; codePtr++) {
           Tcl_AppendResult (interp, " '", codePtr->c_name, "'", NULL);
       }
    }
    return -1;
}

/*-----------------------------------------------------------------------------
 * BSD_SyslogObjCmd --
 *  
 * Implements the `syslog' command:
 *    syslog open
 *    syslog close
 *    syslog logmask
 *    syslog log
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_SyslogObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
    int optIndex;

    static CONST84 char *options[] = { 
        "log", "open", "close", "logmask", (char *) NULL
    };

    enum options {
            OPT_LOG, OPT_OPEN, OPT_CLOSE, OPT_LOGMASK
    };

    if (objc < 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "option");
	return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], options, "option", TCL_EXACT,
        &optIndex) != TCL_OK) {
	    return TCL_ERROR;
    }

    switch ((enum options) optIndex) {
      case OPT_LOG: {
        int priority;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "priority message");
	    return TCL_ERROR;
	}

	priority = GetSyslogPriority (interp, Tcl_GetString (objv[2]), TCLBSD_LOG_ERROR);
	if (priority == -1) {
	   return TCL_ERROR;
	}

	syslog (priority, "%s", Tcl_GetString (objv[3]));
	break;
      }

      case OPT_OPEN: {
        int        facility;
        char      *ident;
	int        logopt = 0;
	int        logoptIndex;
	int        logoptObjc;
	int        i;
	Tcl_Obj  **logoptObjv;

	static CONST char *logopts[] = {
	    "console", "no_delay", "perror", "pid", (char *)NULL
	};

	int logOpts[] = {LOG_CONS, LOG_NDELAY, LOG_PERROR, LOG_PID};

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "ident logopt facility");
	    return TCL_ERROR;
	}

	/* OK, ident needs to be a const char *, i.e. it needs to not change
	 * behind openlog/syslog's back.  How shall we implement that?
	 * Should we malloc and copy?  Nah, let's just increment the
	 * reference count on the object so that Tcl will leave it the
	 * heck alone. -kl
	 */
	 Tcl_IncrRefCount (objv[2]);
	ident = Tcl_GetString (objv[2]);

	if (Tcl_ListObjGetElements (interp, objv[3], &logoptObjc, &logoptObjv) == TCL_ERROR) {
	    Tcl_AppendResult (interp, " while getting list of log options", NULL);
	    return TCL_ERROR;
	}

	for (i = 0; i < logoptObjc; i++) {
	    if (Tcl_GetIndexFromObj (interp, logoptObjv[i], logopts, "logopts", TCL_EXACT, &logoptIndex) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    logopt |= logOpts[logoptIndex];
	}

	facility = GetSyslogFacility (interp, Tcl_GetString (objv[4]), TCLBSD_LOG_ERROR);
	if (facility == -1) {
	   return TCL_ERROR;
	}

	openlog (ident, logopt, facility);
	break;
      }

      case OPT_CLOSE:
	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	closelog();
	break;


      case OPT_LOGMASK: {
        int priority;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 3, objv, "priority");
	    return TCL_ERROR;
	}

	priority = GetSyslogPriority (interp, Tcl_GetString (objv[2]), TCLBSD_LOG_ERROR);
	if (priority == -1) {
	   return TCL_ERROR;
	}
	setlogmask ( LOG_UPTO (priority));
	break;
      }
    }

    return TCL_OK;
}

