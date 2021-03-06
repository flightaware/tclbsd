#
# tclbsd -- tcl procs that are part of the package
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

        append text " / [format %.4f $val] $var"
    }
    return [string range $text 3 end]
}

#
# linux_cptime
#
proc linux_cptime {_array} {
    upvar $_array array

    if {[catch {set fp [open /proc/stat]} catchResult] == 1} {
	error "cptime option is not available on this operating system"
    }
    gets $fp line
    close $fp
    lassign $line dummy user nice sys idle iowait irq softirq steal guest guest_nice
    set intr [expr {$irq + $softirq}]
    set newTotal [expr {$user + $nice + $sys + $idle + $irq + $softirq + $steal + $guest + $guest_nice}]

    set result [list]
    if {[info exists array(total)]} {
	set total [expr {$newTotal - $array(total)}]
	foreach var "user nice sys idle intr steal guest guest_nice" {
	    set diff [expr {[set $var] - $array($var)}]
	    lappend result $var [expr {round(($diff * 100.0) / $total)}]
	}
    }

    array set array [list user $user nice $nice sys $sys idle $idle iowait $iowait intr $intr steal $steal guest $guest guest_nice $guest_nice total $newTotal]
    return $result
}

} ;# namespace ::bsd

# vim: set ts=8 sw=4 sts=4 noet :
