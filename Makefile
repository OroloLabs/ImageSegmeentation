main: LoadDataset.cpp
	@g++ LoadDataset.cpp KMSegment.cpp -o main -ljpeg --std=c++11

test:
	@rm main;clear;g++ LoadDataset.cpp KMSegment.cpp -o main --std=c++11 -ljpeg --std=c++11 ;./main
 
writer:
	 @clear;g++ -o write_jpeg_example write_jpeg_example.cpp -ljpeg; ./write_jpeg_example


