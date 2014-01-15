/*
 * tclbsd.c --
 *
 */

#include <tcl.h>
#include "bsd.h"

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT


/*
 *----------------------------------------------------------------------
 *
 * Bsd_Init --
 *
 *	Initialize the BSD extension.  The string "tclbsd" 
 *      in the function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *
 *----------------------------------------------------------------------
 */

EXTERN int
Bsd_Init(Tcl_Interp *interp)
{
    Tcl_Namespace *namespace;

    if (Tcl_InitStubs(interp, "8.5", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgRequire(interp, "Tcl", "8.5", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgProvide(interp, "BSD", PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

     namespace = Tcl_CreateNamespace (interp, "bsd", (ClientData)NULL, (Tcl_NamespaceDeleteProc *)NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::rusage",
                          BSD_RusageObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::rlimit",
                          BSD_RlimitObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::statfs",
                          BSD_StatfsObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::getfsstat",
                          BSD_GetfsstatObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::getloadavg",
                          BSD_GetLoadAvgObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::setproctitle",
                          BSD_SetProcTitleObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::getkey",
                          BSD_GetKeyObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::syslog",
                          BSD_SyslogObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::abort",
                          BSD_AbortCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::cptime",
                          BSD_getcptimeObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    Tcl_CreateObjCommand (interp,
			  "bsd::uptime",
                          BSD_UptimeObjCmd,
                          (ClientData) NULL,
                          (Tcl_CmdDeleteProc*) NULL);

    if (Tcl_Export (interp, namespace, "*", 0) == TCL_ERROR) {
        return TCL_ERROR;
    }

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Bsd_SafeInit --
 *
 *	Initialize the BSD extension in a safe interpreter.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Very little
 *
 *----------------------------------------------------------------------
 */

EXTERN int
Bsd_SafeInit(Tcl_Interp *interp)
{
    return TCL_OK;
}

