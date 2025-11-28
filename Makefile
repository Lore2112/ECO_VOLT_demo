CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -Iinclude
LDFLAGS = -pthread

TARGET = ecoVoltDemo

#source files
SOURCES = main.cpp \
		  src/sensori/InterfacciaSensore.cpp \
		  src/sensori/SensoreI2C.cpp \
		  src/core/ElaboraLetture.cpp
#object files
OBJECTS = $(SOURCES:.cpp=.o)

#link object files to create executable
$(TARGET): $(OBJECTS)
		$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build completa! Avvia con: sudo ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

rebuild: clean all

run: $(TARGET)
	sudo ./$(TARGET)

.PHONY: all clean rebuild run