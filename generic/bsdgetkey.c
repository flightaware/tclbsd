/*
 * getkey.c --
 *
 * C code for Unix System V to read single keys from the keyboard
 *
 *---------------------------------------------------------------------------
 * 
 *
 */

#include <sys/types.h>
#include <unistd.h>

#ifndef USE_TERMIO
#  ifndef USE_TERMIOS
#    define USE_TERMIOS
#  endif
#endif

#ifdef USE_TERMIO
#include <termio.h>
#endif

#ifdef USE_TERMIOS
#include <termios.h>
#include <sys/ioctl.h>
#define termio termios

#ifndef TCGETA
#  define TCGETA TIOCGETA
#endif

#ifndef TCSETA
#  define TCSETA TIOCSETAW
#endif

#ifndef TCFLSH
#  define TCFLSH TIOCFLUSH
#endif

#ifndef TCSBRK
#  define TCSBRK TIOCSBRK
#endif

#endif /* USE_TERMIOS */

#include <tcl.h>


/*
 *----------------------------------------------------------------------
 *
 * BSD_GetKeyCmd --
 *      Implements the getkey command:
 *        getkey [timeout]
 *
 * If no timeout value is specified, waits indefinitely.
 *
 * Results:
 *      Standard TCL results.
 *
 *----------------------------------------------------------------------
 */
int
BSD_GetKeyObjCmd (clientData, interp, objc, objv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           objc;
    Tcl_Obj      *const objv[];
{
    double ftimeout;
    struct termio orig_term_settings;
    struct termio single_char_term_settings;
    unsigned char timeout;
    unsigned char vmin = 0;
    char c;
    int returnval;

    if (objc > 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "?timeout?");
        return TCL_ERROR;
    }

    if (objc == 1) {
	timeout = 0;
	vmin = 1;
    } else {	
        if (Tcl_GetDoubleFromObj (interp, objv[1], &ftimeout) != TCL_OK)
            return TCL_ERROR;

	if ((ftimeout < 0.0) || (ftimeout > 25.5)) {
            Tcl_AppendResult (interp, 
                              " timeout must be between 0.0 and 25.5",
			      (char *) NULL);
	    return TCL_ERROR;
	}
	timeout = 10.0 * ftimeout;
    }

    /* get their terminal settings, copy them and do a version for input
     * without canonical input processing, so we can timeout input, get
     * stuff without a newline, etc, etc. */
    if (ioctl(0,TCGETA,&orig_term_settings) < 0) goto unixError;

    single_char_term_settings = orig_term_settings; /* structure copy */

#if 0
    single_char_term_settings.c_iflag &= ~(INLCR|IGNCR|ICRNL);
    single_char_term_settings.c_lflag &= ~(ICANON|ECHO|ISIG);
    single_char_term_settings.c_cc[VINTR] = 0377;
#else
    cfmakeraw (&single_char_term_settings);
#endif
    single_char_term_settings.c_cc[VMIN] = vmin;
    single_char_term_settings.c_cc[VTIME] = timeout;

    if (ioctl(0,TCSETA,&single_char_term_settings) < 0) goto unixError;

    returnval = read (0, &c, 1);
    if (returnval > 0) {
#if 0
	Tcl_SetByteArrayObj (Tcl_GetObjResult (interp), &c, 1);
#else
        Tcl_SetIntObj (Tcl_GetObjResult (interp), c);
#endif
    }

    if (ioctl(0,TCSETA,&orig_term_settings)) goto unixError;
    return TCL_OK;

unixError:
    Tcl_PosixError (interp);
    return TCL_ERROR;
}

// vim: set ts=8 sw=4 sts=4 noet :
