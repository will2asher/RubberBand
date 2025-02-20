MKPATH=mk/
include $(MKPATH)buildsys.mk

SUBDIRS = src lib
CLEAN = *.dll *.exe
DISTCLEAN = config.status config.log docs/.deps \
	mk/buildsys.mk mk/extra.mk
REPOCLEAN = aclocal.m4 autom4te.cache configure src/autoconf.h.in version

.PHONY: tests dist
tests:
	$(MAKE) -C src tests

TAG = angband-`scripts/version.sh`
OUT = $(TAG).tar.gz

manual:
	echo "To make the manual, please enter docs/ and run `make html` after installing Sphinx."

dist:
	git checkout-index --prefix=$(TAG)/ -a
	scripts/version.sh > $(TAG)/version
	$(TAG)/autogen.sh
	rm -rf $(TAG)/autogen.sh $(TAG)/autom4te.cache
	tar --exclude .gitignore --exclude *.dll --exclude .github \
		--exclude .travis.yml -czvf $(OUT) $(TAG)
	rm -rf $(TAG)

# Hack to clean up test results in tests.
pre-distclean:
	@find tests -name run.out -exec rm {} \;

# Remove the files generated by autogen.sh and the version stamp file.
# In general, this should not be used when working with a distributed
# source archive (.tar.gz) since those archives do not include autogen.sh for
# rebuilding the configure script.
repoclean: distclean
	for i in "" $(REPOCLEAN) ; do \
		test x"$$i" = x"" && continue; \
		if test -d "$$i" -o -f "$$i" ; then rm -rf "$$i" ; fi \
	done
