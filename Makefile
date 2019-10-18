all:
	g++ -o kmeans kmeans.cpp --std=c++11
	
run:
	./kmeans

clean:
	rm -rf *o kmeans
