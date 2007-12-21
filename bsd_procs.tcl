#
# tclbsd -- tcl procs that are part of the package
#
# $Id: bsd_procs.tcl,v 1.2 2007-12-21 08:23:19 karl Exp $
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

proc rusage_snapshot {arrayVar} {
    upvar $arrayVar data

    array set data [rusage]
}

proc rusage_report {firstArrayVar secondArrayVar} {
    upvar $firstArrayVar first
    upvar $secondArrayVar second

    set result ""
    foreach element [array names first] {
        lappend result $element [expr $second($element) - $first($element)]
    }
    return $result
}

#
# rusage_exec - take a snapshot of rusage statistics from before and after
#  running the passed-in command and return a report of what resources
#  were used while it was running.
#
proc rusage_exec {command} {
    rusage_snapshot before
    eval $command
    rusage_snapshot after
    return [rusage_report before after]
}

#
# cputime - return the CPU time of the code that's passed as an argument
#
proc cputime {code {iterations 1}} {
    set startRusage [::bsd::rusage]

    for {set i 0} {$i < $iterations} {incr i} {
        uplevel $code
    }

    set endRusage [::bsd::rusage]

    array set start $startRusage
    array set end $endRusage

    set text ""
    foreach var "userTimeUsed systemTimeUsed" {
        set val [expr {($end($var) - $start($var)) / $iterations}]

        append text " / $val $var"
    }
    return [string range $text 3 end]
}

}
