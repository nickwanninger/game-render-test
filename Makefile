CC = clang
WARNINGS = -Wall -Wformat
CFLAGS = -std=c99 -I./include -O3
LDLIBS = -lSDL2 -lm -lpthread
version = 0.0.1

objs = $(srcs:.c=.o)
exe = game

SRCDIR = src
OBJDIR = build

STRUCTURE := $(shell find $(SRCDIR) -type d)
CODEFILES := $(addsuffix /*,$(STRUCTURE))
CODEFILES := $(wildcard $(CODEFILES))

SRCFILES := $(filter %.c,$(CODEFILES))
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.c=%.o))

.PHONY: all clean

all: $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.c)
	@echo "Compiling    \x1B[92m$@\x1B[0m \x1B[2m<- $<\x1B[0m"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe): $(OBJFILES)
	@echo "Linking      \x1B[93m$@\x1B[0m \x1B[2m<- $(OBJDIR)/*\x1B[0m "
	@$(CC) $(WARNINGS) $(LDLIBS) -o $@ $(foreach i,$^,$(i) )
	@echo "\x1B[93mCompiled     v$(version)\x1B[0m"

clean:
	$(Q)rm $(exe)
	$(Q)rm -rf $(OBJDIR)
