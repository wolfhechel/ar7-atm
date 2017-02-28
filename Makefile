# File: drivers/net/avalanche_cpmac/Makefile
#
# Makefile for the Linux network (CPMAC) device drivers.
#


O_TARGET := tiatm.o

obj-m	+= tiatm.o
tiatm-objs := tn7atm.o tn7dsl.o tn7sar.o dsl_hal_api.o dsl_hal_support.o cpsar.o aal5sar.o

tiatm.o: $(tiatm-objs)
	$(LD) $(LDOPTS) -o wlan.$(KMODSUF) -r $(tiatm-objs)
	
EXTRA_CFLAGS += -DEL -I. -DPOST_SILICON -DCOMMON_NSP -DCONFIG_LED_MODULE -DDEREGISTER_LED -DNO_ACT

-include $(TOPDIR)/Rules.make
