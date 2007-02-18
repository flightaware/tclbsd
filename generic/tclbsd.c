/*
 * tclbsd.c --
 *
 * $Id: tclbsd.c,v 1.6 2007-02-18 06:29:48 karl Exp $
 *
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
    /* not until Tcl 8.5 */
    /* Tcl_Namespace *namespace; */

    /*
     * This may work with 8.0, but we are using strictly stubs here,
     * which requires 8.1.
     */
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgProvide(interp, "BSD", PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    /* Not until Tcl 8.5 */
    /* namespace = Tcl_CreateNamespace (interp, "bsd", (ClientData)NULL, (Tcl_NamespaceDeleteProc *)NULL); */
    if (Tcl_Eval (interp, "namespace eval bsd {}") == TCL_ERROR) {
	return TCL_ERROR;
    }

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

    /* Not until Tcl 8.5 */
    /* if (Tcl_Export (interp, namespace, "*", 0) == TCL_ERROR) {
	return TCL_ERROR;
    } */
    if (Tcl_Eval (interp, "namespace eval bsd {namespace export *}") == TCL_ERROR) {
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

