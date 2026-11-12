TARGET = lab2
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SOURCES = main.cpp \
          cell.cpp \
          game_field.cpp \
          robot.cpp \
          robot_factory.cpp \
          game.cpp \
          console_renderer.cpp \
          ability.cpp \
          status.cpp \
          item.cpp

OBJDIR = objdir
OBJECTS := $(addprefix $(OBJDIR)/,$(patsubst %.cpp, %.o, $(SOURCES)))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(TARGET) $(OBJDIR)

.PHONY: all clean