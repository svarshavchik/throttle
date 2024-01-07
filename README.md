Throttle
========

throttle is a small tool that I originally wrote to throttle parallel
linking of many binaries in a large C++ project.

I use distcc to parallelize compilation across a server pool. The
parallelization works well, but after building my shared libraries there's
a mad scramble to link a bunch of binaries with those shared libraries, and
I end up with make spawning a crapton of parallel links. Each individual
link takes a fair bit of RAM. distcc only distributes the compiles (of
course), so all links run on the same server, which starts swapping
like crazy.

Unfortunately, make -j is an all or nothing proposition. make either
parallelizes independent targets, or parallelized nothing.

Command reference
-----------------

throttle [lockfile] [n] [program] [argument]...

"lockfile" - throttle creates this scratch file that it uses for locking.
Place it in /var/tmp, or maybe even in the build directory itself.
throttle chain-executes [program] with its [argument]s; but will block
if [n] other throttle processes are executing the program, and waits until
one of them exits.

Building throttle
-----------------

Stock autotools build. After cloning the git repo:

aclocal
autoheader
automake --add-missing
./configure
make

On Fedora, to build an installable rpm:

make dist
rpmbuild -ta throttle-{version}.tar.gz

On Debian/Ubuntu, to build an installable deb:

make deb
sudo dpkg -i deb/throttle_<version>-*.deb

Integrating throttle with autotools
-----------------------------------

The simplest way to use throttle with GNU make-generated Makefile is
to override CCLD and CXXLD when invoking (g)make from your build environment.
For example, I set Emacs' compile-command to:

make -k -j 14 \
    CCLD='throttle /var/tmp/link.lock 3 gcc' \
    CXXLD='throttle /var/tmp/link.lock 3 g++'

automake-generated Makefile sets the link commands "CCLD" to "gcc" and
"CXXLD" to "g++". This simply overrides these commands when invoking make.

The end result is that I get 14 parallel compiles going at the same time,
but everything get throttled down to three parallel links.

Of course, make doesn't know anything about throttle. make may (and will)
kick off 14 parallel links, which will get done three at a time, with
the remaining processes waiting on file locks instead of, perhaps,
compiling something else. But, in practice, I've observed that this
rarely happens.

Bugs
----

throttle tries to acquire an available lock. If all locks are acquired,
throttle sleeps for one second and tries again. So, when an existing
process terminates, it can take up to a single chronological second to
start another instance of [program].
