Summary: An L2TP client/server, designed for VPN use.
Name: openl2tp
Version: %%VER%%
Release: %%REL%%%{?dist}
License: GPL
Group: System Environment/daemons
URL: ftp://downloads.sourceforge.net/projects/openl2tp/%{name}-%{version}.tar.gz
Source0: %{name}-%{version}.tar.gz
Vendor: Katalix Systems Ltd
Packager: James Chapman <jchapman--at--katalix.com>
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Prereq: ppp == %%PPPVER%%, readline >= 4.2, portmap, kernel >= 2.6.23
ExclusiveOS: Linux

BuildPrereq: ppp == %%PPPVER%%, readline >= 4.2, glibc >= 2.4, flex, bison, kernel-devel >= 2.6.23

%description
OpenL2TP is a complete implementation of RFC2661 - Layer Two Tunneling
Protocol Version 2, able to operate as both a server and a client. It
is ideal for use as an enterprise L2TP VPN server, supporting more
than 100 simultaneous connected users. It may also be used as a client
on a home PC or roadwarrior laptop.

OpenL2TP has been designed and implemented specifically for Linux. It
consists of

- a daemon, openl2tpd, handling the L2TP control protocol exchanges
  for all tunnels and sessions

- a plugin for pppd to allow its PPP connections to run over L2TP
  sessions

- a Linux kernel driver for efficient datapath (integrated into the
  standard kernel from 2.6.23).

- a command line application, l2tpconfig, for management.

%package devel
Summary: OpenL2TP support files for plugin development
Group: Development/Libraries

%description devel
This package contains support files for building plugins for OpenL2TP,
or applications that use the OpenL2TP APIs.

%prep
%setup

%build
make OPT_CFLAGS="$RPM_OPT_FLAGS" \
	PPPD_VERSION=%%PPPVER%%

%install
make install DESTDIR=$RPM_BUILD_ROOT \
	PPPD_VERSION=%%PPPVER%%

%{__mkdir} -p $RPM_BUILD_ROOT/etc/rc.d/init.d $RPM_BUILD_ROOT/%{_sysconfdir}/sysconfig
%{__cp} -f etc/rc.d/init.d/openl2tpd $RPM_BUILD_ROOT%{_sysconfdir}/rc.d/init.d/openl2tpd
%{__cp} -f etc/sysconfig/openl2tpd $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/openl2tpd

%clean
if [ "$RPM_BUILD_ROOT" != `echo $RPM_BUILD_ROOT | sed -e s/openl2tp-//` ]; then
	rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root,-)
%doc README LICENSE
%dir %{_libdir}/openl2tp
/usr/bin/l2tpconfig
/usr/sbin/openl2tpd
%{_libdir}/openl2tp/ppp_null.so
%{_libdir}/openl2tp/ppp_unix.so
%{_libdir}/openl2tp/ipsec.so
%{_libdir}/openl2tp/event_sock.so
%{_libdir}/pppd/%%PPPVER%%/openl2tp.so
%{_libdir}/pppd/%%PPPVER%%/pppol2tp.so
/usr/share/man/man1/l2tpconfig.1.gz
/usr/share/man/man4/openl2tp_rpc.4.gz
/usr/share/man/man5/openl2tpd.conf.5.gz
/usr/share/man/man7/openl2tp.7.gz
/usr/share/man/man8/openl2tpd.8.gz
/etc/rc.d/init.d/openl2tpd
/etc/sysconfig/openl2tpd
%config /etc/sysconfig/openl2tpd

%files devel
%defattr(-,root,root,-)
%doc plugins/README doc/README.event_sock
%{_libdir}/openl2tp/l2tp_rpc.x
%{_libdir}/openl2tp/l2tp_event.h
%{_libdir}/openl2tp/event_sock.h

%changelog

* %%DATE%% %%AUTHOR%% - %%VER%%

  - Import a patch from Ben McKeegan which ensures that subprocesses
    don't inherit opened file descriptors from openl2tpd. In some
    circumstances, sockets would remain open until all spawned
    processes exited, which lead to some unusual
    effects. Specifically, for any given tunnel, so long as any of the
    pppd processes that were forked while that tunnel was up
    (including those on different tunnels) are still running or any
    daemons started from their init scripts are running, the UDP
    socket is never closed, even when openl2tp itself has long since
    closed the socket and forgotten about it. This also fixes
    Bug #2056243 reported by Ulrich Holeschak.

  - Deprecate the l2tpconfig help command. The help command caused
    l2tpconfig to hang but since it doesn't add any value over the man
    page, let's deprecate it. This was first reported in Bug #1792299.

  - Fix compiler warnings in the pppd plugins.

  - Avoid linking openl2tpd/l2tpconfig against unnecessary libs
    (libnsl, libncurses, librt).

  - Split the RPC and header files out to a separate devel package;
    these files only need to be installed by developers implementing
    OpenL2TP plugins or applications that use the OpenL2TP APIs so
    they aren't needed in standard installs.

  - Fix a bug that was introduced in v1.5 which broke the
    inhibit_default_plugin flag. This flag is used by some test
    plugins to inhibit the use of ppp_unix.so for testing.

* Sun Aug 17 2008 James Chapman - 1.5

  - Fix ppp profile ppp authentication options which seem to have been
    broken since 0.17. The auth_pap, auth_chap etc options should
    translate into refuse-xxx options to pppd, but these pppd
    arguments were not generated properly.

  - While testing an install for an ISP, it was found that pppd would
    always offer EAP authentication when negotiating with its peer,
    despite auth_eap=off being set in the ppp profile. It turns out
    that some ppp peers (including Cisco!) drop the connection unless
    the peer asks for the exactly the same authentication
    method. Specifically, in a Cisco, if CHAP is configured, it will
    drop the connection if the peer suggests that it can do EAP. To
    configure pppd to force a specific authentication method, one of
    pppd's require-xxx options must be used. Until now, OpenL2TP has
    no interface to control pppd's require-xxx auth options. With this
    release, if all but one of the ppp profile's auth_xxx options is
    disabled, OpenL2TP will now automatically add the require-xxx pppd
    auth option for the one auth option that is enabled. For example,
    to configure CHAP only, set auth_chap=yes auth_pap=no
    auth_mschapv1=no auth_mschapv2=no auth_eap=no in the ppp
    profile. This will yield the following pppd arguments: refuse-pap
    refuse-mschap refuse-mschap-v2 refuse-eap require-chap.

  - Fixed a bug in the per-tunnel session limit handling. If
    configured with a session limit (non-zero max_sessions value), the
    tunnel's session count would be decremented twice if a session was
    cleaned up as a result of reaching the session limit. As a result,
    the next session would be successfully created, resulting in more
    sessions on the tunnel than the user requested.

  - Improve session event generation such that session_down events are
    generated only if the session_up event was previously generated
    and session_deleted events are generated only if the
    session_created event was previously generated. Previously, if a
    session never came up successfully, a session_down and a
    session_deleted event was generated. Anyone using custom openl2tp
    plugins which use the session_down and session_deleted hooks
    should test their plugins after taking this release.

  - Add a new event_sock plugin which exports a named pipe
    /tmp/openl2tpd.evt for use by other processes. A process may
    listen on the named pipe for event messages from openl2tpd which
    tell of session up/down/created/deleted events. See
    doc/README.event_sock for details.

  - Change the loading of plugins to be done after the core
    application init has completed. In previous versions, plugins were
    loaded during command line argument parsing. This change allows
    plugins to use openl2tpd internal APIs that would otherwise be
    uninintalialised when the plugin is loaded. The new event_sock
    plugin makes use of this to register its file descriptor(s) with
    the application main loop.

* Thu May 8 2008 James Chapman - 1.4

  - Fix a bug in the parsing of some parameters of config
    files. Previous versions limited the characters that could be used
    in string values such that parameters that were filenames
    containing the / character lead to parse errors. The parser is now
    more flexible about the characters that it allows in string
    values.

  - Revert a change made in 1.0 to do with L2TP Hello message
    generation. Prior to 1.0, we only sent Hello packets if no L2TP
    control _or_ data packets passed over the tunnel during the Hello
    timeout period, but this was changed to unconditionally send
    Hellos. It turns out that the L2TPv3 spec (Openl2tp is currently
    L2TPv2) specifies the behaviour that we had prior to 1.0. The new
    spec says that the L2TPv2 spec was misinterpreted by several
    vendors and explicitely calls out that all L2TP implementations
    should check for both control and data activity before sending a
    Hello. This avoids possible Hello timeouts when under extreme load
    conditions, where Hello messages might be swamped by data traffic.

  - Don't create the tunnel PPPoX socket until the tunnel is
    up. (Session PPPoX sockets are unchanged.) We previously created
    the tunnel socket when the tunnel instance was created, before the
    peer tunnel id was known. This change means that the actual peer
    tunnel id so can be passed down to the kernel when the connect()
    call is made.

  - Update the ipsec plugin to use absolute paths to the ipsec
    utilities. This avoids problems when the server runs somewhere
    other than / (typically when run in the foreground for debugging).

  - Change GNU-specific __FUNCTION__ strings in the source code to
    __func__ since the former is now deprecated. This change does not
    alter the generated code.

  - Change the structure of a struct l2tp_packet to store a struct
    iovec in the structure rather than using our own struct
    l2tp_packet_buffer. This avoids allocating a struct iovec when
    building the struct msghdr for sendmsg() calls. It also avoids
    converting from one struct format to another and is therefore more
    efficient. There are no externally visible differences as a result
    of this change.

  - Fix the no_ppp=yes parameter of session and session profiles. A
    change in version 1.2 broke this feature such that sessions failed
    whenever this parameter was set.

* Wed Apr 9 2008 James Chapman - 1.3

  - Fix a bug in the handling of the host_name AVP which would cause
    junk characters on the end of the name, depending on the order of
    the AVPs in the message. If this AVP is followed by an AVP with
    its Mandatory bit set, the host_name string isn't null-terminated,
    so OpenL2TP includes extra characters in the string. Since the
    host_name AVP can be used by OpenL2TP to find a matching peer
    profile or to do simple tunnel authentication, this was causing
    tunnel setup problems with some vendor equipment (Huawei).

  - Remove the validation checks on the M-bit of AVPs. The L2TP spec
    specifies whether the M-bit should be set or clear for each AVP,
    so OpenL2TP used to validate that and reject messages from peers
    that did not conform to the spec. It has recently been found that
    Huawei L2TP implementations set the M-bit incorrectly in some AVPs
    which caused tunnel setup problems. But further research into
    L2TPv3 (which is not yet supported by OpenL2TP) found that L2TPv3
    _requires_ that L2TPv2 implementations do not validate AVP M-bit
    values against the L2TPv2 spec. So for L2TPv3 interopability, this
    check has been removed.

  - Rewrite the pppd-to-openl2tpd interface to use regular Unix domain
    sockets rather than RPC. This interface is used by pppd to tell
    openl2tpd when the ppp interface comes up or goes down. While RPC
    is good for the management interface where the command client and
    L2TP server may run on machines with different architectures, the
    pppd processes always run on the same CPU as openl2tpd, so the
    complexity of RPC is overkill. Users won't see any behaviour
    change - things should work exactly as before. Heavy users (ISPs)
    might notice some performance improvement when setting up or
    tearing down thousands of PPP sessions.

* Mon Mar 24 2008 James Chapman - 1.2

  - Fix multilink PPP. This has been broken since 1.0, when the PPP
    setup timer was introduced. The fix is to have the openl2tp pppd
    plugin send an event to openl2tpd for every PPP interface in the
    multilink PPP bundle, not just the first.

  - Fix a bug in the config file parser which failed to process the
    default_route argument of ppp profiles. Any config file with this
    option would fail to parse.

* Sun Feb 24 2008 James Chapman - 1.1

  - Fix compiler optimisation issues when compiled with recent gcc-4.x
    compilers. Modern gcc can aggressively reorder instructions and
    even functions. In the case of USL list operations, this can cause
    code to be executed the wrong side of list add/remove which
    results in corrupted lists or timing issues. The problem can show
    itself by openl2tpd hanging while walking lists, but other random
    crashes could also occur. The fix is to use memory barriers in the
    low level USL list operations.

  - Initialize the random seed using a value from /dev/urandom at
    startup so that the assigned tunnel_id / session_id values have
    better randomness. (We used to use /dev/urandom directly for all
    random ids rather than libc's random() call but we switched to
    random() when it was found that stress testing could cause reads
    to /dev/urandom to block due to the system's entropy pool being
    exhausted in extreme load conditions.) Using /dev/urandom to init
    the random seed is a good compromise.

  - Fix pppd plugin to always call the ip up/down hooks to send status
    back to openl2tpd, regardless of whether pppol2tp_ifname was
    specified. Previously, the hooks were only needed when the
    interface name wasn't specified because we used the event to
    discover the ppp interface name in openl2tpd. But these hooks are
    now used to stop the session's ppp setup timer so we always need
    them. This bug was introduced in 1.0 and was reported by Iouri
    Kharon.

  - Apply several fixes from Ben McKeegan:

    - Fix ppp_unix plugin to prevent "active pppd count gone negative"
      error messages. This bug could cause the daemon to segfault when
      lots of PPP sessions were present.

    - Fix a bug in the transmit queue code which could crash the
      daemon if a ZLB ever found its way onto the packet queue.

    - Fix uninitialised variable bug in USL's child pid handling
      code. This could cause an openl2tpd crash on a heavily loaded
      system.

    - Fix a buffer overrun bug in the CLI code if l2tpconfig is
      connected to a remote openl2tpd.

  - Fix errors found with valgrind:

    - Incorrect checks for valid fd before close() calls in several
      places.

    - Possible buffer overrun when storing result_code AVP data from
      peer.

    - Fix uninitialised data errors in struct sockaddr
      structures. While these specific valgrind errors are bogus, it
      is useful to fix the code to get rid of the valgrind complaints.


* Sun Feb 11 2008 James Chapman - 1.0

- Initial build
