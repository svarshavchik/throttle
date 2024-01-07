bin_PROGRAMS=throttle

EXTRA_DIST=throttle.spec README.md \
	packaging/debian/rules.in \
	packaging/debian/changelog.in \
	packaging/debian/copyright \
	packaging/debian/control.in \
	packaging/debian/source/format \
	packaging/debian/throttle.lintian-overrides


throttle_SOURCES=throttle.C

.PHONY: deb

deb: dist
	rm -rf deb
	mkdir deb
	cp @PACKAGE@-@VERSION@.tar.gz deb/@PACKAGE@_@VERSION@.orig.tar.gz
	cd deb; tar xf @PACKAGE@_@VERSION@.orig.tar.gz
	set -e; cd deb/@PACKAGE@-@VERSION@; \
		. /etc/lsb-release; \
		cp -r packaging/debian .; \
		for f in changelog rules control; do \
			sed 's/@''VERSION''@/@VERSION@/g;s/@''DATE''@/'"`date -R`"'/g;s/@''RELEASE''@/'"`echo $$DISTRIB_RELEASE | tr '.' '+'`"'/' < debian/$$f.in >debian/$$f; done; \
		chmod +x debian/rules; \
		debuild -us -uc
