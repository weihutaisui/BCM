# -*- tcl -*-

if {[lsearch [namespace children] ::tcltest] == -1} {
    package require tcltest
    namespace import -force ::tcltest::*
}

set message ""

proc clearResult { } {
    global message
    set message ""
}

proc l2tpConfig { args } {
    global message
    catch { exec ../l2tpconfig -t $args } msg
    set message $message\n$msg
    return $message\n
}

proc sleep { args } {
    global message
    exec sleep $args
    return $message
}

proc addCrLf { args } {
    global message
    set message $message\n
    return $message
}

proc getSysIpAddr { args } {
    catch { exec /sbin/ifconfig eth2 } msg
    foreach word $msg {
	set subwords [split $word :]
	if { [string compare [lindex $subwords 0] addr] == 0 } {
	    return [lindex $subwords 1]
	}
    }
    return ""
}

# Use cat -E to put a '$' on the end of each line. This means lines
# ending with a '\' won't be joined in the output.

proc catFile { args } {
    global message
    catch { exec cat -E $args } msg
    set message $message\n$msg
    return $message\n
}

# Create an IP address from a specified base, avoiding reserved addresses

proc makeIpAddr { base num } {
    set quad [ split $base . ]
    set octet1 [ lindex $quad 0 ]
    set octet2 [ lindex $quad 1 ]
    set octet3 [ lindex $quad 2 ]
    set octet4 [ lindex $quad 3 ]

    for { set i 0 } { $i < $num } { incr i } {
	incr octet4
	if { $octet4 > 254 } {
	    set octet4 1
	    incr octet3
	    if { $octet3 > 254 } {
		set octet3 0
		incr octet2
	    }
	}
    }
    set ip [ join [ list $octet1 $octet2 $octet3 $octet4 ] . ]
    return $ip
}

# check IP reachabiliy - use ping and suppress output. Use return value 
# to derive reachable status

proc pingReachableTest { args } {
    global message
    set result [ catch { exec ping -c 1 -w 5 -n $args } msg ]
    if { $result == 0 } {
	set message "$message\npingReachableTest: OK"
    } else {
	set message "$message\npingReachableTest: FAILED"
    }
    # set message $message\n$msg
    return $message\n
}

proc pingDataTest { addr size count } {
    global message
    set result [ catch { exec ping -l $count -s $size -q -w 5 -n $addr } msg ]
    if { $result == 0 } {
	set message "$message\npingDataTest: OK"
    } else {
	set message "$message\npingDataTest: FAILED"
    }
    # set message $message\n$msg
    return $message\n
}

