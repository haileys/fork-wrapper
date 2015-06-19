# fork-wrapper

`fork-wrapper` spawns a program and waits for all subprocesses to exit before exiting itself. `fork-wrapper` will terminate with the exit status of the last subprocess to exit.

## The Problem

Many daemons (eg. Unicorn) implement hot-reloading by forking a new master process. Once the new master process has booted up, the old master process exits.

This does not play well with Upstart, as it assumes that the old master process exiting means the service has stopped. The problem is even worse when the `respawn` stanza is being used - Upstart will notice the old master process dying and try to respawn it, even though the new master process is already running.

## The Solution

Linux 3.4 introduced a new option for the `prctl` syscall - `PR_SET_CHILD_SUBREAPER`. Setting this process attribute makes it a 'subreaper'. A subreaper behaves like init for its entire process subtree. When a child of a subreaper dies, the child's children are reparented to the subreaper rather than init.

`fork-wrapper` sets itself as a subreaper, forks and execs the given command, and then drops into a `wait(2)` loop. It will continue to reap children until there are none left. When all child processes have terminated, it then exits with with the status of the last child to exit.
