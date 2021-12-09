all: histogramEqualization

histogramEqualization: 
	g++ main.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
	

clean:
	rm start