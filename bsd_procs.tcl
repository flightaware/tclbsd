#
# tclbsd -- tcl procs that are part of the package
#
# $Id: bsd_procs.tcl,v 1.1 2006-03-01 20:58:59 karl Exp $
#

namespace eval ::bsd {

#
# calc_resources -- evaluate the command
#
proc calc_resources {command} {
    set result ""
    set pre [::bsd::rusage]
    uplevel $command
    set post [::bsd::rusage]
    foreach "prevar preval" $pre "postvar postval" $post {
	lappend result $prevar [expr $postval - $preval]
    }
    return $result
}

}
