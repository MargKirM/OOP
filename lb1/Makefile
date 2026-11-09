TARGET = lab1
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SOURCES = main.cpp \
		  cell.cpp  \
		  game_field.cpp \
		  robot.cpp \
		  robot_factory.cpp \
		  game.cpp \
		  console_renderer.cpp

OBJDIR = objdir
OBJECTS := $(addprefix $(OBJDIR)/,$(patsubst %.cpp, %.o, $(SOURCES)))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(TARGET) $(OBJDIR)

.PHONY: all clean