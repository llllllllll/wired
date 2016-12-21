INCLUDE_DIRS := include/
INCLUDE := $(foreach d,$(INCLUDE_DIRS), -I$d)
DFILES := main.d

.PHONY: all clean

all: etc/test

etc/test: etc/test.cc
	$(CXX) $(INCLUDE) -MD $< -o etc/test

README.rst: etc/build-readme etc/test
	./$< etc/test.cc ./etc/test > $@

clean:
	rm main

-include $(DFILES)
