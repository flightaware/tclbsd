package require BSD
package require Tclx

::bsd::setproctitle "test proctitle"
sleep 30
::bsd::setproctitle "30 seconds"
sleep 10
::bsd::setproctitle "40 seconds"
