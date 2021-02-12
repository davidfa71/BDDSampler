DIRS=sampler

all:
	$(MAKE) -C cudd-3.0.0 
	$(MAKE) -C cudd-3.0.0 install
	@for i in $(DIRS); do  $(MAKE) -C $$i; done
clean:
	make -C cudd-3.0.0 clean
	@for i in $(DIRS); do $(MAKE) -C $$i clean; done
	@rm -f bin/* 2>/dev/null
	@rm -f lib/* 2>/dev/null

