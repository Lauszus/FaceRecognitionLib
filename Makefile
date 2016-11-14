CC = gcc
CXX = g++

CFLAGS = -Wall -O3 -std=gnu11 -Wfatal-errors -Wunused-parameter -Wextra -openmp
CXXFLAGS = -Wall -O3 -std=gnu++11 -Wfatal-errors -Wunused-parameter -Wextra -openmp

# Include Eigen library
CXXFLAGS += `pkg-config --cflags eigen3`

# Include RedSVD library
CXXFLAGS += -I./RedSVD/include

# No debugging
# CFLAGS += -DNDEBUG
# CXXFLAGS += -DNDEBUG

# Change this to the name of your main file
PROG = main

# Command used to remove the object files and executable
RM = rm -f
RM_DIR = rm -rf
OBJECT_DIR = unix

ifeq ($(OS),Windows_NT)
	# Add .exe suffix on Windows
	PROG:=$(addsuffix .exe, $(PROG))
	# Use del command for deleting files and rmdir command for removing directories on Windows
	RM := del
	RM_DIR := rmdir /s /q
	# Use directory called win for Windows
	OBJECT_DIR := win
endif

# Create a list of all object files
OBJS  = $(addsuffix .o,$(addprefix $(OBJECT_DIR)/,$(basename $(notdir $(wildcard *.c)))))
OBJS += $(addsuffix .o,$(addprefix $(OBJECT_DIR)/,$(basename $(notdir $(wildcard *.cpp)))))

.PHONY: all clean run

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS)

# Rebuild everything when makefile changes and create build directory.
$(OBJS): Makefile | $(OBJECT_DIR)

$(OBJECT_DIR):
	mkdir $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJECT_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(PROG)

# Remove all objects and executable
clean:
	$(RM_DIR) $(OBJECT_DIR)
	$(RM) $(PROG)

# Run executable
run: all
	./$(PROG)
