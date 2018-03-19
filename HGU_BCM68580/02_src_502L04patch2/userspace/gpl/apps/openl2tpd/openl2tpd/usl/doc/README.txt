USL (User Space Library) implements features found in many userspace
applications with a goal of simplifying a typical userspace
application that has state machines, uses multiple timers, forks child
processes and reads multiple file descriptors.

USL provides an infrastructure to allow the application to be single
threaded, despite handling events from multiple sources. Event
callbacks are always called by USL's own main loop, avoiding the need
for data structures to be locked when accessed by timers, signal
handlers and main code. The application should be designed to never
stall in its registered callbacks since doing so will stall the main
loop.

The following subsystems are provided. Each may be used individually.

state machines		 fsm.txt
timers			 timers.txt
file desciptors		 fd.txt
signals			 signals.txt
processes		 pid.txt
lists			 list.txt
debug			 debug.txt
