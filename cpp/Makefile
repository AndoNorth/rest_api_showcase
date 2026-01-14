CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wno-pessimizing-move -O2

BIN := bin
EXECUTABLE := rest_api

SRC := src/main.cpp src/app/app.cpp src/http/http_server.cpp src/db/db.cpp $(shell find lib -name "*.cpp")
INCLUDE := -Iinclude -Ilib

LIBRARIES := -lboost_system -lpthread -lmariadbclient

all: $(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	rm -rf $(BIN)

