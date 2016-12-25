INCLUDE_DIRS := include/
INCLUDE := $(foreach d,$(INCLUDE_DIRS), -I$d)
DFILES := etc/test.d

.PHONY: all test clean

all: etc/test

test: etc/test
	./etc/test

etc/test: etc/test.cc
	@$(CXX) $(INCLUDE) -MD $< -o etc/test

README.rst: etc/build-readme etc/test
	@./$< etc/test.cc ./etc/test > $@

clean:
	rm etc/test etc/test.d

-include $(DFILES)
