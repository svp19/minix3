part1: part1.cpp input.ppm
	@g++ part1.cpp && ./a.out input.ppm output_part1.ppm

part2_1a: part2_1a.cpp input.ppm
	@g++ -std=c++11 -pthread part2_1a.cpp && ./a.out input.ppm output_part2_1a.ppm

part2_1b: part2_1b.cpp input.ppm
	@g++ -std=c++11 -pthread part2_1b.cpp && ./a.out input.ppm output_part2_1b.ppm

part2_2: part2_2gray.cpp part2_2edge.cpp input.ppm
	@g++ -std=c++11 -pthread part2_2gray.cpp -o gray && ./gray input.ppm output_part2_2.ppm & \
	g++ -std=c++11 -pthread part2_2edge.cpp -o edge && ./edge input.ppm output_part2_2.ppm

part2_3: part2_3.cpp input.ppm
	@g++ -std=c++11 part2_3.cpp && ./a.out input.ppm output_part2_3.ppm

rmall: *.ppm a.out gray edge
	@rm output_part1.ppm output_part2_1a.ppm output_part2_1b.ppm \
	output_part2_2.ppm output_part2_3.ppm a.out gray edge