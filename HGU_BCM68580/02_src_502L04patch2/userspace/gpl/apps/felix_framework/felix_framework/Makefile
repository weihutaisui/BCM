all: conditional_build

prefix = $(INSTALL_DIR)/usr/local/felix

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

conditional_build: check_tools
	cd Trunk; mvn -Dpackaging=plugins install -DskipTests
	cd Trunk; mvn -Dpackaging=bundle install -DskipTests
	mkdir -p $(prefix)
	cp -rf Trunk/main/bin $(prefix)
	cp -rf Trunk/main/bundle $(prefix)
	cp -rf Trunk/main/conf $(prefix)

check_tools:
	@echo "Checking for mvn (Apache-Maven):"
	@$(HOSTTOOLS_DIR)/scripts/checkver.pl -e mvn || $(COND_FAIL);
	@echo "Checking for Java and Maven enviroment paths:"
	@if [ "$(JAVA_HOME)" = "" ]; then \
          echo "You need to set Java environment JAVA_HOME (See osgiJvmDevelopmentGuide.doc)"; exit 1; \
	fi
	@if [ "$(M2_HOME)" = "" ] || [ "$(M2)" = "" ]; then \
          echo "You need to set Maven environment variables M2_HOME/M2 (See osgiJvmDevelopmentGuide.doc)"; exit 1; \
	fi

ifneq ($(strip $(BUILD_OSGI_FELIX)),)
clean: check_tools 
	rm -rf $(prefix)/bin
	rm -rf $(prefix)/bundle
	rm -rf $(prefix)/conf
	-cd Trunk; mvn -Dpackaging=plugins uninstall
	-cd Trunk; mvn -Dpackaging=bundle uninstall

else

clean: 
	@echo "skipping $@ (not configured)"

endif

