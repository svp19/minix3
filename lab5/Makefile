part1: transform.cpp mussoorie.ppm
	@g++ transform.cpp && ./a.out mussoorie.ppm output_part1.ppm

part2_1a: transform_2a.cpp mussoorie.ppm
	@g++ -std=c++11 -pthread transform_2a.cpp && ./a.out mussoorie.ppm output_part2_1a.ppm

part2_1b: transform_2b.cpp mussoorie.ppm
	@g++ -std=c++11 -pthread transform_2b.cpp && ./a.out mussoorie.ppm output_part2_1b.ppm

part2_2: transform_3gray.cpp transform_3edge.cpp mussoorie.ppm
	@g++ -std=c++11 -pthread transform_3gray.cpp -o gray && ./gray mussoorie.ppm output_part2_2.ppm & g++ -std=c++11 -pthread transform_3edge.cpp -o edge && ./edge mussoorie.ppm output_part2_2.ppm

part2_3: part2_3.cpp mussoorie.ppm
	@g++ -std=c++11 part2_3.cpp && ./a.out mussoorie.ppm output_part2_3.ppm