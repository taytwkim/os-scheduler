CXX = clang++
CXXFLAGS = -g -O0 -Wall

scheduler: src/*.cpp
	$(CXX) $(CXXFLAGS) src/*.cpp -o scheduler

clean:
	rm -f scheduler
