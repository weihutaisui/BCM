# all.tcl --
#
# This file contains a top-level script to run all of the L2TP
# tests.  Execute it by invoking "source all.test" when running tcltest
# in this directory.
#
# Copyright (c) 1998-1999 by Scriptics Corporation.
# Copyright (c) 2000 by Ajuba Solutions
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
# 

set tcl_traceExec 0
set testOrder [list kernel peer_profile tunnel_profile session_profile ppp_profile system tunnel]

set tcltestVersion [package require tcltest]
namespace import -force ::tcltest::*

if {$tcl_platform(platform) == "macintosh"} {
	tcltest::singleProcess 1
}

proc ::tcltest::initConstraintsHook {} {
    set ::tcltest::testConstraints(kernelModule) 1
    set ::tcltest::testConstraints(kernelModuleLoaded) [ file exists /proc/net/pppol2tp ]
    set ::tcltest::testConstraints(l2tpdRunning) 1
}

proc ::tcltest::cleanupTestsHook {} {
    # Add cleanup code here
}

set ::tcltest::debug 0

set ::tcltest::testSingleFile false
set ::tcltest::testsDirectory [file dir [info script]]

set testFiles [::tcltest::getMatchingFiles]
array set testStatus {}

# Do tests in the order specified by $testOrder
foreach testType $testOrder {
    foreach file $testFiles {
	if { [string compare $file $::tcltest::testsDirectory/$testType.test] != 0 } {
	    continue
	}
	if {[catch {source $file} msg]} {
	    puts $::tcltest::outputChannel $msg
	}
	set testStatus($::tcltest::testsDirectory/$testType.test) 1
    }
}

# Do remaining tests that aren't explicitely listed
foreach file $testFiles {
    if { [info exists testStatus($file)] } {
	continue
    }
    if {[catch {source $file} msg]} {
	puts $::tcltest::outputChannel $msg
    }
    set testStatus($file) 1
}

::tcltest::cleanupTests 1

return
