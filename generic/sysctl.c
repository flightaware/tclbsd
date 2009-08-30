
/* 
 * cptime.c --
 *
 *      Contains the BSD extensions for Tcl.
 *-----------------------------------------------------------------------------
 * Copyright 2009 Karl Lehenbauer
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  NeoSoft makes
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 *-----------------------------------------------------------------------------
 */

#include <sys/param.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <assert.h>

#include <tcl.h>
#include "bsd.h"

/*-----------------------------------------------------------------------------
 * BSD_getcptimeObjCmd --
 *  
 * Implements the `cputime' command:
 *    cputime
 *  
 * Results:
 *      A standard Tcl result.
 *      
 * Side effects:
 *      See the user documentation.
 *-----------------------------------------------------------------------------
 */     
int
BSD_getcptimeObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *CONST objv[];
{
#ifdef CPUSTATES
    int             i;
    int             err;
    long           *times;
    long            total;
    long            oldTotal = 0;
    long            newTotal = 0;
    long            diff;
    int             percent;
    long            prior;
    size_t          size;
    size_t          retSize;

    Tcl_Obj        *listObj = Tcl_NewObj();
    Tcl_Obj        *objName = NULL;
    Tcl_Obj        *totalNameObj;
    Tcl_Obj        *priorObj;

    if (objc != 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "arrayName");
	return TCL_ERROR;
    }

    retSize = size = sizeof (long) * CPUSTATES;
    times = (long *)ckalloc (size);
    err = sysctlbyname("kern.cp_time", times, &retSize, (void *)NULL, (size_t)0);
    // printf("%d %d %ld %ld\n", err, errno, size, retSize);
    if (err < 0) {
        Tcl_PosixError (interp);
	return TCL_ERROR;
    }
    assert (size == retSize);

    totalNameObj = Tcl_NewStringObj ("total", -1);
    priorObj = Tcl_ObjGetVar2 (interp, objv[1], totalNameObj, 0);
    if (priorObj != NULL) {
	if (Tcl_GetLongFromObj (interp, priorObj, &oldTotal) == TCL_ERROR) {
	    return TCL_ERROR;
	}
    }

    for (i = 0; i < CPUSTATES; i++) {
        newTotal += times[i];
    }

    if (Tcl_ObjSetVar2 (interp, objv[1], totalNameObj, Tcl_NewLongObj (newTotal), 0) == NULL) {
        return TCL_ERROR;
    }
    total = newTotal - oldTotal;

    for (i = 0; i < CPUSTATES; i++) {
        
        switch (i) {
	  case CP_USER:
	    objName = Tcl_NewStringObj ("user", -1);
	    break;

	  case CP_NICE:
	    objName = Tcl_NewStringObj ("nice", -1);
	    break;

	  case CP_SYS:
	    objName = Tcl_NewStringObj ("sys", -1);
	    break;

	  case CP_INTR:
	    objName = Tcl_NewStringObj ("intr", -1);
	    break;

	  case CP_IDLE:
	    objName = Tcl_NewStringObj ("idle", -1);
	    break;
	}

	Tcl_IncrRefCount (objName);
	priorObj = Tcl_ObjGetVar2 (interp, objv[1], objName, 0);
	if (priorObj != NULL) {
	    if (Tcl_GetLongFromObj (interp, priorObj, &prior) == TCL_ERROR) {
	        return TCL_ERROR;
	    }
	    diff = times[i] - prior;
	    percent = (int)((diff * 100.0 + 0.5) / total);

	    Tcl_ListObjAppendElement (interp, listObj, objName);
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewIntObj (percent));
	}

	if (Tcl_ObjSetVar2 (interp, objv[1], objName, Tcl_NewLongObj (times[i]), TCL_LEAVE_ERR_MSG) == NULL) {
	    return TCL_ERROR;
	}

	Tcl_DecrRefCount (objName);
    }

    Tcl_SetObjResult (interp, listObj);
    ckfree ((char *)times);
    return TCL_OK;
#else
    Tcl_SetStringObj (Tcl_GetObjResult (interp), "cputime option is not available on this operating system", -1);
    return TCL_ERROR;
#endif
}

