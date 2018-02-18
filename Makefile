INCLUDE_DIRS := include/
INCLUDE := $(foreach d,$(INCLUDE_DIRS), -I$d)
DFILES := etc/test.d layer9-example/predict.d
CXXFLAGS := -std=gnu++17

.PHONY: all
all: test

.PHONY: test
test:
	WIRED_INCLUDE_PATH=include/ py.test

.PHONY: style-check
style-check:
	@flake8

layer9-example/%.p7: layer9-example/%.csv
	@bin/csv2p7 $(shell basename $< .csv) $< -o $@

layer9-example/predict: layer9-example/main.cc layer9-example/samples.p7 layer9-example/observations.p7
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -MD $< -o $@

.PHONY: clean
clean:
	@rm etc/test etc/test.d layer9-example/*.p7 layer9-example/predict

-include $(DFILES)
