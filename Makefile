CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = fintrack
SRCS     = main.cpp ExpenseManager.cpp
OBJS     = $(SRCS:.cpp=.o)

.PHONY: all clean run report

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

report:
	python3 report.py

clean:
	rm -f $(OBJS) $(TARGET)
