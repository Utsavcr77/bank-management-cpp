CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET   = bank_app
SRCDIR   = src
SRCS     = $(SRCDIR)/main.cpp $(SRCDIR)/Account.cpp $(SRCDIR)/Bank.cpp $(SRCDIR)/Utils.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
	@echo "Build successful! Run with: ./$(TARGET)"

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
	@echo "Cleaned."
