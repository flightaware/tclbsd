
/* 
 * bsd.c --
 *
 *      Contains the Superconnect BSD extensions for Tcl.
 *-----------------------------------------------------------------------------
 * Copyright 1998 NeoSoft, 2005 Superconnect.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  NeoSoft makes
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 *-----------------------------------------------------------------------------
 * $Id: bsd.c,v 1.2 2005-08-07 06:39:44 karl Exp $
 *-----------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <sys/param.h>
#include <sys/mount.h>

#include <stdlib.h>
#include <string.h>

#include <tcl.h>

static int
AppendNameLong (interp, listObj, name, element)
    Tcl_Interp    *interp;
    Tcl_Obj       *listObj;
    char          *name;
    long           element;
{
    Tcl_Obj   *stringObj;
    Tcl_Obj   *elementObj;

    stringObj = Tcl_NewStringObj (name, -1);
    if (Tcl_ListObjAppendElement (interp, listObj, stringObj) != TCL_OK) {
	return TCL_ERROR;
    }

    elementObj = Tcl_NewLongObj (element);
    if (Tcl_ListObjAppendElement (interp, listObj, elementObj) != TCL_OK) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int
AppendNameDouble (interp, listObj, name, element)
    Tcl_Interp    *interp;
    Tcl_Obj       *listObj;
    char          *name;
    double         element;
{
    Tcl_Obj   *stringObj;
    Tcl_Obj   *elementObj;

    stringObj = Tcl_NewStringObj (name, -1);
    if (Tcl_ListObjAppendElement (interp, listObj, stringObj) != TCL_OK) {
	return TCL_ERROR;
    }

    elementObj = Tcl_NewDoubleObj (element);
    if (Tcl_ListObjAppendElement (interp, listObj, elementObj) != TCL_OK) {
	return TCL_ERROR;
    }

    return TCL_OK;
}

static int
AppendNameString (interp, listObj, name, element)
    Tcl_Interp    *interp;
    Tcl_Obj       *listObj;
    char          *name;
    char          *element;
{
    Tcl_Obj   *stringObj;
    Tcl_Obj   *elementObj;

    stringObj = Tcl_NewStringObj (name, -1);
    if (Tcl_ListObjAppendElement (interp, listObj, stringObj) != TCL_OK) {
	return TCL_ERROR;
    }

    elementObj = Tcl_NewStringObj (element, -1);
    if (Tcl_ListObjAppendElement (interp, listObj, elementObj) != TCL_OK) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int
AppendNameObj (interp, listObj, name, appendListObj)
    Tcl_Interp    *interp;
    Tcl_Obj       *listObj;
    char          *name;
    Tcl_Obj       *appendListObj;
{
    Tcl_Obj   *stringObj;

    stringObj = Tcl_NewStringObj (name, -1);
    if (Tcl_ListObjAppendElement (interp, listObj, stringObj) != TCL_OK) {
	return TCL_ERROR;
    }

    if (Tcl_ListObjAppendElement (interp, listObj, appendListObj) != TCL_OK) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*-----------------------------------------------------------------------------
 * BSD_RusageObjCmd --
 *  
 * Implements the `rusage' command:
 *    rusage self
 *    rusage children
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_RusageObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
    char         *subOption = NULL;
    Tcl_Obj      *resultObj = Tcl_GetObjResult (interp);
    int           who;
    struct rusage rusage;
    double	  utime;
    double	  stime;
    double        ixrssSeconds;
    double        idrssSeconds;
    double        isrssSeconds;

    static int    clockTick = -1;

    if (clockTick == -1) {
	clockTick = sysconf(_SC_CLK_TCK);
    }

    if (objc > 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "[self|children]");
	return TCL_ERROR;
    }

    if (objc == 2) {
	subOption = Tcl_GetStringFromObj (objv[1], NULL);
    }

    if (objc == 1 || strcmp (subOption, "self") == 0) {
	who = RUSAGE_SELF;
    } else if (strcmp (subOption, "children") == 0) {
	who = RUSAGE_CHILDREN;
    } else {
	Tcl_AppendStringsToObj (resultObj, 
				"subcommand was not \"self\" or \"children\"",
				(char *)NULL);
	return TCL_ERROR;
    }

    if (getrusage (who, &rusage) < 0) {
	Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
	return TCL_ERROR;
    }

    utime = 
        rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec / 1000000.0;

    stime =
	rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec / 1000000.0;

    ixrssSeconds = rusage.ru_ixrss / (clockTick + 0.0);
    idrssSeconds = rusage.ru_idrss / (clockTick + 0.0);
    isrssSeconds = rusage.ru_isrss / (clockTick + 0.0);

    if ((AppendNameDouble (interp, resultObj, "userTimeUsed", utime)
	  == TCL_ERROR)
      || (AppendNameDouble (interp, resultObj, "systemTimeUsed", stime) 
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "maxResidentSetSizeKbytes", 
	    rusage.ru_maxrss) == TCL_ERROR)
      || (AppendNameDouble (interp, resultObj, "sharedTextKbyteSeconds", 
	    ixrssSeconds) == TCL_ERROR)
      || (AppendNameDouble (interp, resultObj, "unsharedDataKbyteSeconds", 
	    idrssSeconds) == TCL_ERROR)
      || (AppendNameDouble (interp, resultObj, "stackKbyteSeconds", 
	    isrssSeconds) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "pageReclaims", 
	    rusage.ru_minflt) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "pageFaults", 
	    rusage.ru_majflt) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "nTimesSwapped", 
	    rusage.ru_nswap) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "fileSystemInputs", 
	    rusage.ru_inblock) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "fileSystemOutputs", 
	    rusage.ru_oublock) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "ipcMessagesSent", 
	    rusage.ru_msgsnd) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "ipcMessagesReceived", 
	    rusage.ru_msgrcv) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "nSignalsDelivered", 
	    rusage.ru_nsignals) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "voluntaryContextSwitches", 
	    rusage.ru_nvcsw) == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, "involuntaryContextSwitches", 
	   rusage.ru_nivcsw) 
	  == TCL_ERROR))
	      return TCL_ERROR;

    return TCL_OK;
}

/*-----------------------------------------------------------------------------
 * BSD_RlimitObjCmd --
 *  
 * Implements the `rlimit' command:
 *    rlimit...
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_RlimitObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
    static CONST84 char *getSetOptions[] = { "get", "set", (char *) NULL};
    enum GSSubOptIdx {
            GSGetIdx, GSSetIdx
    } getset;

    static CONST84 char *softHardOptions[] = { 
             "soft", "hard", "both",
             (char *) NULL};

    enum SHSubOptIdx {
            SHSoftIdx, SHHardIdx, SHBothIdx
    } hardsoft;

    static CONST84 char *limitOptions[] = { 
            "core", "cpu", "data", "fsize",
             "memlock", "nofile", "nproc", "rss", 
             "stack",
             (char *) NULL};

    enum ISubOptIdx {
            ICoreIdx, ICpuIdx, IDataIdx, IFsizeIdx,
            IMemlockIdx, INofileIdx, INprocIdx, IRssIdx,
            IStackIdx
    } limitID;

    Tcl_Obj        *resultObj = Tcl_GetObjResult (interp);
    int             result;
    int             resource;
    struct rlimit   rlimit;

    if (objc < 4 || objc > 5) {
	Tcl_WrongNumArgs(interp, 1, objv, "get|set soft|hard limitName ?val?");
	return TCL_ERROR;
    }

    result = Tcl_GetIndexFromObj(interp, objv[1], getSetOptions, 
	        "get/set option", 0, (int *) &getset);
    if (result != TCL_OK) {
        return result;
    }
 
    result = Tcl_GetIndexFromObj(interp, objv[2], softHardOptions, 
	    "hard/soft option", 0, (int *) &hardsoft);
    if (result != TCL_OK) {
        return result;
    }
  
    result = Tcl_GetIndexFromObj(interp, objv[3], limitOptions, "limit name", 0,
            (int *) &limitID);
    if (result != TCL_OK) {
        return result;
    }
 
    switch (limitID) {
        case ICoreIdx:
	    resource = RLIMIT_CORE;
	    break;

        case ICpuIdx:
	    resource = RLIMIT_CPU;
	    break;

        case IDataIdx:
	    resource = RLIMIT_DATA;
	    break;

        case IFsizeIdx:
	    resource = RLIMIT_FSIZE;
	    break;

        case IMemlockIdx:
	    resource = RLIMIT_MEMLOCK;
	    break;

        case INofileIdx:
	    resource = RLIMIT_NOFILE;
	    break;

        case INprocIdx:
	    resource = RLIMIT_NPROC;
	    break;

        case IRssIdx:
	    resource = RLIMIT_RSS;
	    break;

        case IStackIdx:
	    resource = RLIMIT_STACK;
	    break;
    }

    switch (getset) {
	char resultString[32];
	Tcl_Obj *stringObj;
	rlim_t limitVal;
	long longLimitVal;

	static Tcl_Obj *unlimitedResultObj = NULL;

	case GSGetIdx:

	    if (unlimitedResultObj == NULL) {
		unlimitedResultObj = Tcl_NewStringObj ("unlimited", -1);
	    }

	    if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "get soft|hard limitName");
		return TCL_ERROR;
	    }

	    if (getrlimit (resource, &rlimit) < 0) {
		Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
		return TCL_ERROR;
	    }
	   
            switch (hardsoft) {
		case SHSoftIdx:
		    limitVal = rlimit.rlim_cur;
		    break;

		case SHHardIdx:
		    limitVal = rlimit.rlim_max;
		    break;
	    }

	    if (limitVal == RLIM_INFINITY) {
		Tcl_ListObjAppendElement (interp, resultObj, unlimitedResultObj);
		Tcl_IncrRefCount (unlimitedResultObj);
	    } else {
		sprintf(resultString, "%qd", limitVal);
		stringObj = Tcl_NewStringObj (resultString, -1);
		Tcl_ListObjAppendElement (interp, resultObj, stringObj);
	    }

	    break;

	case GSSetIdx:
	    if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, 
		    "set soft|hard limitName value");
		return TCL_ERROR;
	    }

	    if (Tcl_GetLongFromObj (interp, objv[4], &longLimitVal) 
		  == TCL_ERROR) {
		return TCL_ERROR;
	    }

	    if (getrlimit (resource, &rlimit) < 0) {
		Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
		return TCL_ERROR;
	    }
	   
            switch (hardsoft) {
		case SHSoftIdx:
		    rlimit.rlim_cur = longLimitVal;
		    break;

		case SHHardIdx:
		    rlimit.rlim_max = longLimitVal;
		    break;
	    }

	    if (setrlimit (resource, &rlimit) < 0) {
		Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
		return TCL_ERROR;
	    }
	    break;
    }

    return TCL_OK;
}

/*-----------------------------------------------------------------------------
 * BSD_StatfsObjCmd --
 *  
 * Implements the `statfs' command:
 *    statfs path
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_StatfsObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
    char         *path;
    char          fstypename[MFSNAMELEN+1];
    char          mntonname[MNAMELEN+1];
    char          mntfromname[MNAMELEN+1];
    Tcl_Obj      *resultObj = Tcl_GetObjResult (interp);
    Tcl_Obj      *flagListObj = Tcl_NewObj ();
    struct statfs statfsbuf;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "path");
	return TCL_ERROR;
    }

    path = Tcl_GetStringFromObj (objv[1], NULL);

    if (statfs (path, &statfsbuf) < 0) {
	Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
	return TCL_ERROR;
    }

    strncpy (fstypename, statfsbuf.f_fstypename, MFSNAMELEN);
    strncpy (mntonname, statfsbuf.f_mntonname, MNAMELEN);
    strncpy (mntfromname, statfsbuf.f_mntfromname, MNAMELEN);

    if ((AppendNameLong (interp, resultObj, 
			 "fundamentalFileSystemBlockSize", statfsbuf.f_bsize)
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "optimalTransferBlockSize", statfsbuf.f_iosize) 
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "totalDataBlocks", statfsbuf.f_blocks)
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "freeBlocks", statfsbuf.f_bfree)
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "availableFreeBlocks", statfsbuf.f_bavail)
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "totalFileNodes", statfsbuf.f_files)
	  == TCL_ERROR)
      || (AppendNameLong (interp, resultObj, 
			  "freeFileNodes", statfsbuf.f_ffree)
	  == TCL_ERROR)
      || (AppendNameString (interp, resultObj, 
			  "fileSystemType", fstypename)
	  == TCL_ERROR)
      || (AppendNameString (interp, resultObj, 
			  "mountPoint", mntonname)
	  == TCL_ERROR)
      || (AppendNameString (interp, resultObj, 
			  "mountedFileSystem", mntfromname)
	  == TCL_ERROR))
	      return TCL_ERROR;

#define FLAGCHECK(x, y) if (statfsbuf.f_flags & x) {if (Tcl_ListObjAppendElement (interp, flagListObj, Tcl_NewStringObj (y, -1)) != TCL_OK) return TCL_ERROR;}

#ifdef MNT_RDONLY
    FLAGCHECK (MNT_RDONLY, "readOnly")
#endif

#ifdef MNT_SYNCHRONOUS
    FLAGCHECK (MNT_SYNCHRONOUS, "synchronous")
#endif

#ifdef MNT_NOEXEC
    FLAGCHECK (MNT_NOEXEC, "noExec")
#endif

#ifdef MNT_NOSUID
    FLAGCHECK (MNT_NOSUID, "noSUID")
#endif

#ifdef MNT_NODEV
    FLAGCHECK (MNT_NODEV, "noDev")
#endif

#ifdef MNT_UNION
    FLAGCHECK (MNT_UNION, "union")
#endif

#ifdef MNT_ASYNC
    FLAGCHECK (MNT_ASYNC, "asynchronous")
#endif

#ifdef MNT_SUIDDIR
    FLAGCHECK (MNT_SUIDDIR, "SUIDdir")
#endif

#ifdef MNT_SOFTDEP
    FLAGCHECK (MNT_SOFTDEP, "softUpdates")
#endif

#ifdef MNT_NOSYMFOLLOW
    FLAGCHECK (MNT_NOSYMFOLLOW, "noFollowSymlinks")
#endif

#ifdef MNT_NOATIME
    FLAGCHECK (MNT_NOATIME, "noAtime")
#endif

#ifdef MNT_NOCLUSTERR
    FLAGCHECK (MNT_NOCLUSTERR, "noClusterRead")
#endif

#ifdef MNT_NOCLUSTERW
    FLAGCHECK (MNT_NOCLUSTERW, "noClusterWrite")
#endif

#ifdef MNT_EXRDONLY
    FLAGCHECK (MNT_EXRDONLY, "exportedReadOnly")
#endif

#ifdef MNT_EXPORTED
    FLAGCHECK (MNT_EXPORTED, "exported")
#endif

#ifdef MNT_DEFEXPORTED
    FLAGCHECK (MNT_DEFEXPORTED, "worldExported")
#endif

#ifdef MNT_EXPORTANON
    FLAGCHECK (MNT_EXPORTANON, "anonUidMapping")
#endif

#ifdef MNT_EXKERB
    FLAGCHECK (MNT_EXKERB, "kerberosUidMapping")
#endif

#ifdef MNT_EXPUBLIC
    FLAGCHECK (MNT_EXPUBLIC, "webNFS")
#endif

#ifdef MNT_LOCAL
    FLAGCHECK (MNT_LOCAL, "filesystemStoredLocally")
#endif

#ifdef MNT_QUOTA
    FLAGCHECK (MNT_QUOTA, "quotasEnabled")
#endif

#ifdef MNT_ROOTFS
    FLAGCHECK (MNT_ROOTFS, "rootFilesystem")
#endif

#ifdef MNT_USER
    FLAGCHECK (MNT_USER, "userMounted")
#endif

    if (AppendNameObj (interp, resultObj, "flags", flagListObj) == TCL_ERROR) return TCL_ERROR;

    return TCL_OK;
}

/*-----------------------------------------------------------------------------
 * BSD_GetLoadAvgObjCmd --
 *  
 * Implements the `getloadavg' command:
 *    getloadavg
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_GetLoadAvgObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
#define LOADAVG_NELEM 3

    double        loadavg[LOADAVG_NELEM];
    Tcl_Obj      *loadavgObj;
    Tcl_Obj      *resultObj = Tcl_GetObjResult (interp);
    int           loadIndex;

    if (getloadavg (&loadavg[0], LOADAVG_NELEM) < 0) {
	Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
	return TCL_ERROR;
    }

    for (loadIndex = 0; loadIndex < LOADAVG_NELEM; loadIndex++) {
	loadavgObj = Tcl_NewDoubleObj (loadavg[loadIndex]);
	if (Tcl_ListObjAppendElement (interp, resultObj, loadavgObj) != TCL_OK) {
	    return TCL_ERROR;
	}
    }

    return TCL_OK;
}


/*-----------------------------------------------------------------------------
 * BSD_SetProcTitleObjCmd --
 *  
 * Implements the `setproctitle' command:
 *    setproctitle
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_SetProcTitleObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
    char *titleString;

    if (objc > 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "[string]");
	return TCL_ERROR;
    }

    if (objc == 1) {
	titleString = NULL;
    } else {
	titleString = Tcl_GetString (objv[1]);
    }

    setproctitle (titleString);

    return TCL_OK;
}
