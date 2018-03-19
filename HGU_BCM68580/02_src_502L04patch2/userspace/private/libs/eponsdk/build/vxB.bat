cls
@echo #####################################################
@echo start to make SDK
@echo #####################################################
set SDK_BASE=%CD%\..
make %1% -f Makefile OS_TYPE=VXWORKS CPU_TYPE=pentium CPU=PENTIUM


