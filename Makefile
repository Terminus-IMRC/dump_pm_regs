RM := rm -f

LDLIBS := -lbcm_host

dump_pm_regs: dump_pm_regs.o

dump_pm_regs.o: cpr_powman.h $(MAKEFILE_LIST)

.PHONY: clean
clean:
	$(RM) dump_pm_regs
	$(RM) dump_pm_regs.o
