
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

static int
mysysctl(const char *name, void *oldp, size_t *oldlenp,
    void *newp, size_t newlen)
{
        int error;

        error = sysctlbyname(name, oldp, oldlenp, newp, newlen);
        // if (error != 0 && errno != ENOMEM)
                // err(1, "sysctl(%s)", name);
        return (error);
}

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
    int             i;
    int             err;
    long           *times;
    size_t          size;
    size_t          retSize;

    Tcl_Obj       **objList;
    Tcl_Obj        *listObj;

    if (objc != 1) {
	Tcl_WrongNumArgs (interp, 1, objv, "");
	return TCL_ERROR;
    }

    retSize = size = sizeof (long) * CPUSTATES;
    times = (long *)ckalloc (size);
    objList = (Tcl_Obj **)ckalloc (CPUSTATES * 2 * sizeof (Tcl_Obj *));
    err = sysctlbyname("kern.cp_time", times, &retSize, (void *)NULL, (size_t)0);
    // printf("%d %d %ld %ld\n", err, errno, size, retSize);
    if (err < 0) {
        Tcl_PosixError (interp);
	return TCL_ERROR;
    }
    assert (size == retSize);

    for (i = 0; i < CPUSTATES; i++) {
        objList[i * 2 + 1] = Tcl_NewLongObj (times[i]);
        
        switch (i) {
	  case CP_USER:
	    objList[i * 2] = Tcl_NewStringObj ("user", -1);
	    break;

	  case CP_NICE:
	    objList[i * 2] = Tcl_NewStringObj ("nice", -1);
	    break;

	  case CP_SYS:
	    objList[i * 2] = Tcl_NewStringObj ("sys", -1);
	    break;

	  case CP_INTR:
	    objList[i * 2] = Tcl_NewStringObj ("intr", -1);
	    break;

	  case CP_IDLE:
	    objList[i * 2] = Tcl_NewStringObj ("idle", -1);
	    break;
	}
    }

    listObj = Tcl_NewListObj (CPUSTATES * 2, objList);
    Tcl_SetObjResult (interp, listObj);
    ckfree ((char *)times);
    ckfree ((char *)objList);
    return TCL_OK;
}

