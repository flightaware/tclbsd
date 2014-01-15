
/* 
 * bsd.h --
 *
 *      Contains the NeoSoft/Superconnect TCL BSD extensions.
 *-----------------------------------------------------------------------------
 * Copyright 1998 NeoSoft.
 * Copyright 2005 Superconnect, Ltd.
 * Copyright 2013 FlightAware LLC
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  NeoSoft,
 * Superconnect, FlightAware and Karl Lehenbauer make no representations 
 * about the suitability of this software for any purpose.
 *
 * It is provided "as is" without express or implied warranty.
 *
 */

extern int
BSD_RusageObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_RlimitObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_StatfsObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_GetfsstatObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_GetLoadAvgObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_SetProcTitleObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_GetKeyObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_SyslogObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_AbortCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_getcptimeObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
BSD_UptimeObjCmd (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

