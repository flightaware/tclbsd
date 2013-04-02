
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
 * $Id: bsd.c,v 1.13 2008-06-23 16:07:33 karl Exp $
 *-----------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <sys/param.h>
#include <sys/mount.h>

#ifdef HAVE_SYS_VFS_H
#  include <sys/vfs.h>
#endif

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
    Tcl_WideInt limitVal = 0;

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
            "virtual", "core", "cpu", "data", "fsize",
             "memlock", "nofile", "nproc", "rss", 
             "sockbuf", "stack", "swap", "ptys",
             (char *) NULL};

    enum ISubOptIdx {
            IVirtualMemoryIdx, ICoreIdx, ICpuIdx, IDataIdx, IFsizeIdx,
            IMemlockIdx, INofileIdx, INprocIdx, IRssIdx,
            ISockBufIdx, IStackIdx, ISwapIdx, IPtysIdx
    } limitID;

    Tcl_Obj        *resultObj = Tcl_GetObjResult (interp);
    int             result;
    int             resource = 0;
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
	case IVirtualMemoryIdx:
	    resource = RLIMIT_AS;
	    break;

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

	case ISockBufIdx:
#ifdef RLIMIT_SBSIZE
	    resource = RLIMIT_SBSIZE;
	    break;
#else
	    Tcl_SetStringObj (resultObj, "sockbuf size option is not available on this operating system", -1);
	    return TCL_ERROR;
#endif // RLIMIT_SBSIZE 

        case IStackIdx:
	    resource = RLIMIT_STACK;
	    break;

	case ISwapIdx:
#ifdef RLIMIT_SWAP
	    resource = RLIMIT_SWAP;
	    break;
#else
	    Tcl_SetStringObj (resultObj, "swap size option is not available on this operating system", -1);
	    return TCL_ERROR;
#endif // RLIMIT_SWAP 

	case IPtysIdx:
#ifdef RLIMIT_NPTS
	    resource = RLIMIT_NPTS;
	    break;
#else
	    Tcl_SetStringObj (resultObj, "maximum pseudoterminals option is not available on this operating system", -1);
	    return TCL_ERROR;
#endif // RLIMIT_NPTS 
    }

    switch (getset) {

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

		case SHBothIdx:
		    panic ("unexpected/impossible code path");
	    }

	    if (limitVal == RLIM_INFINITY) {
		if (Tcl_ListObjAppendElement (interp, resultObj, unlimitedResultObj) == TCL_ERROR) {
		    return TCL_ERROR;
		}
		Tcl_IncrRefCount (unlimitedResultObj);
	    } else {
		if (Tcl_ListObjAppendElement (interp, resultObj, Tcl_NewWideIntObj (limitVal)) == TCL_ERROR) {
		    return TCL_ERROR;
		}
	    }

	    break;

	case GSSetIdx:
	    if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, 
		    "set soft|hard limitName value");
		return TCL_ERROR;
	    }

	    if (strcmp (Tcl_GetString (objv[4]), "unlimited") == 0) {
		limitVal = RLIM_INFINITY;
	    } else {
		if (Tcl_GetWideIntFromObj (interp, objv[4], &limitVal) 
		      == TCL_ERROR) {
		    return TCL_ERROR;
		}
	    }

	    if (getrlimit (resource, &rlimit) < 0) {
		Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
		return TCL_ERROR;
	    }
	   
            switch (hardsoft) {
		case SHSoftIdx:
		    rlimit.rlim_cur = limitVal;
		    break;

		case SHHardIdx:
		    rlimit.rlim_max = limitVal;
		    break;

		case SHBothIdx:
		    panic ("unexpected/impossible code path");
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
 * StatfsBufToList --
 *  
 * Appends the contents of a statfs buffer to a Tcl list as key-value pairs.
 *  
 * Results:
 *      A standard Tcl result.
 *      
 *-----------------------------------------------------------------------------
 */     
static int
StatfsBufToList (Tcl_Interp *interp, Tcl_Obj *listObj, struct statfs *sp)
{
    Tcl_Obj      *flagListObj = Tcl_NewObj ();

#ifdef MFSNAMELEN
    char          fstypename[MFSNAMELEN+1];
    char          mntonname[MNAMELEN+1];
    char          mntfromname[MNAMELEN+1];

    strncpy (fstypename, sp->f_fstypename, MFSNAMELEN);
    strncpy (mntonname, sp->f_mntonname, MNAMELEN);
    strncpy (mntfromname, sp->f_mntfromname, MNAMELEN);
#endif

    if ((AppendNameLong (interp, listObj, 
			 "fundamentalFileSystemBlockSize", sp->f_bsize)
	  == TCL_ERROR)
#ifdef HAVE_STRUCT_STATFS_F_IOSIZE
      || (AppendNameLong (interp, listObj, 
			  "optimalTransferBlockSize", sp->f_iosize) 
	  == TCL_ERROR)
#endif
      || (AppendNameLong (interp, listObj, 
			  "totalDataBlocks", sp->f_blocks)
	  == TCL_ERROR)
      || (AppendNameLong (interp, listObj, 
			  "freeBlocks", sp->f_bfree)
	  == TCL_ERROR)
      || (AppendNameLong (interp, listObj, 
			  "availableFreeBlocks", sp->f_bavail)
	  == TCL_ERROR)
      || (AppendNameLong (interp, listObj, 
			  "totalFileNodes", sp->f_files)
	  == TCL_ERROR)
      || (AppendNameLong (interp, listObj, 
			  "freeFileNodes", sp->f_ffree)
	  == TCL_ERROR)
#ifdef MFSNAMELEN
      || (AppendNameString (interp, listObj, 
			  "fileSystemType", fstypename)
	  == TCL_ERROR)
      || (AppendNameString (interp, listObj, 
			  "mountPoint", mntonname)
	  == TCL_ERROR)
      || (AppendNameString (interp, listObj, 
			  "mountedFileSystem", mntfromname)
	  == TCL_ERROR)
#endif
    )
	return TCL_ERROR;

#define FLAGCHECK(x, y) if (sp->f_flags & x) {if (Tcl_ListObjAppendElement (interp, flagListObj, Tcl_NewStringObj (y, -1)) != TCL_OK) return TCL_ERROR;}

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

#ifdef MNT_GJOURNAL
    FLAGCHECK (MNT_GJOURNAL, "gjournal")
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

#ifdef MNT_MULTILABEL
    FLAGCHECK (MNT_MULTILABEL, "multiLabel")
#endif

#ifdef MNT_ACLS
    FLAGCHECK (MNT_ACLS, "aclsEnabled")
#endif

#ifdef MNT_JAILDEVFS
    FLAGCHECK (MNT_JAILDEVFS, "jailFriendlyDevfsBehavior")
#endif

#ifdef MNT_IGNORE
    FLAGCHECK (MNT_IGNORE, "doNotShowInDF")
#endif

    if (AppendNameObj (interp, listObj, "flags", flagListObj) == TCL_ERROR) return TCL_ERROR;

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
    Tcl_Obj      *resultObj = Tcl_GetObjResult (interp);
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

    return StatfsBufToList (interp, resultObj, &statfsbuf);
}

/*-----------------------------------------------------------------------------
 * BSD_GetfsstatObjCmd --
 *  
 * Implements the `getfsstat' command:
 *    getfsstat ?-wait|-nowait?
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_GetfsstatObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
#ifdef HAVE_GETFSSTAT
    Tcl_Obj       *resultObj = Tcl_GetObjResult (interp);
    struct statfs *statfsbufs;
    int            bufsize;
    int            i;
    int            nMountedFilesystems;
    int            waitOption = MNT_NOWAIT;
    char          *subOption;

    if (objc > 2) {
      usage:
	Tcl_WrongNumArgs(interp, 1, objv, "?-wait|-nowait?");
	return TCL_ERROR;
    }

    if (objc == 2) {
	subOption = Tcl_GetStringFromObj (objv[1], NULL);
	if (strcmp (subOption, "-nowait") == 0) {
	    waitOption = MNT_NOWAIT;
	} else if (strcmp (subOption, "-wait") == 0) {
	    waitOption = MNT_WAIT;
	} else {
	    goto usage;
	}
    }

    nMountedFilesystems = getfsstat (NULL, 0, waitOption);
    if (nMountedFilesystems < 0) {
	Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
	return TCL_ERROR;
    }

    bufsize = nMountedFilesystems * sizeof (struct statfs);

    statfsbufs = (struct statfs *)ckalloc (bufsize);

    if (getfsstat (statfsbufs, bufsize, MNT_NOWAIT) < 0) {
	Tcl_SetStringObj (resultObj, Tcl_PosixError (interp), -1);
	ckfree ((void *)statfsbufs);
	return TCL_ERROR;
    }

    for (i = 0; i < nMountedFilesystems; i++) {
	Tcl_Obj *listObj = Tcl_NewObj ();

        if (StatfsBufToList (interp, listObj, &statfsbufs[i]) == TCL_ERROR) {
	    ckfree ((void *)statfsbufs);
	    return TCL_ERROR;
	}

	if (Tcl_ListObjAppendElement (interp, resultObj, listObj) == TCL_ERROR) {
	    ckfree ((void *)statfsbufs);
	    return TCL_ERROR;
	}
    }

    ckfree ((void *)statfsbufs);
    return TCL_OK;
#else // HAVE_GETFSSTAT
    Tcl_SetObjResult (interp, Tcl_NewStringObj ("getfsstat is not available on this operating system", -1));
    return TCL_ERROR;
#endif// HAVE_GETFSSTAT
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

#ifdef HAVE_SETPROCTITLE
    if (objc == 1) {
	titleString = NULL;
    } else {
	titleString = Tcl_GetString (objv[1]);
    }

    setproctitle ("-%s", titleString);
    return TCL_OK;
#else
    Tcl_SetObjResult (interp, Tcl_NewStringObj ("setproctitle is not available on this operating system", -1));
    return TCL_ERROR;
#endif
}

/*-----------------------------------------------------------------------------
 * BSD_AbortCmd --
 *  
 * Implements the `abort' command:
 *    abort
 *  
 * Results:
 *      An abort signal is generated and probably aborts Tcl.
 *
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_AbortCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{

    abort();

    return TCL_OK;
}
