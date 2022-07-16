MODULE1=mod_example_1
MODULEA=mod_example_A
MODULE2=mod_example_2
MODULE3=mod_example_3
MODULE_NAME1=example_1
MODULE_NAMEA=example_A
MODULE_NAME2=example_2
MODULE_NAME3=example_3
APXS=/usr/bin/apxs

all: $(MODULE1) $(MODULEA) $(MODULE2) $(MODULE3)

mod_example_1:
	$(APXS) -Wc,-g -c $@.c 

mod_example_A:
	$(APXS) -Wc,-g -c $@.c 

mod_example_2:
	$(APXS) -Wc,-g -c $@.c 

mod_example_3:
	$(APXS) -Wc,-g -c $@.c 


install1: $(MODULE1) 
	$(APXS) -i -a -n $(MODULE_NAME1) $(MODULE1).la

installA: $(MODULEA) 
	$(APXS) -i -a -n $(MODULE_NAMEA) $(MODULEA).la

install2: $(MODULE2) 
	$(APXS) -i -a -n $(MODULE_NAME2) $(MODULE2).la

install3: $(MODULE3) 
	$(APXS) -i -a -n $(MODULE_NAME3) $(MODULE3).la

install: install1 installA install2 install3

clean:
	rm -rf *.so *.o *.slo *.la *.lo .libs
