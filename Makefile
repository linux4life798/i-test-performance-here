# Run "make run"

CXXFLAGS += -std=gnu++20
CXXFLAGS += -O0 -Wall

TARGETS = context-switch-vs-spin-wait barrier-latency

all: $(TARGETS)

run: $(TARGETS)
	@echo
	@echo "Context Switch VS. Spin Wait"
	./context-switch-vs-spin-wait
	@echo
	@echo "Test Barrier Latency"
	./barrier-latency

clean:
	$(RM) $(TARGETS)

.PHONY: all run clean