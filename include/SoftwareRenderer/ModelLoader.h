#pragma once

#include <vector>

#include <tuple>
#include <string>
#include <optional>

#include <fstream>
#include <iostream>

#include <LeptonMath/Vector.h>

namespace sr {

	using Model3D = std::tuple<std::vector<float>, std::vector<int>>;

	void normalize(std::vector<float>& data) {
		float biggest = 0;
		for (auto x : data)
			if (std::abs(x) > biggest) biggest = std::abs(x);
		for (auto& x : data) x /= biggest;
	}

	std::vector<std::string> splitLineObj(const std::string& line) {
		int index = 0;
		int next = 0;

		std::vector<std::string> subStrings;

		while (index < line.size()) {
			if (line[index] == ' ') {
				if (index != next) subStrings.push_back(line.substr(next, size_t(index) - next));
				while (index != line.size() && line[index] == ' ') index++;
				next = index;
			}
			else {
				index++;
			}
		}

		if (index != next) subStrings.push_back(line.substr(next, size_t(index) - next));
		return subStrings;
	}

	std::optional<Model3D> loadObj(const std::string& path) {
		Model3D modelData = std::make_tuple<std::vector<float>, std::vector<int>>({}, {});
		auto& [positions, indices] = modelData;

		std::ifstream input;
		input.open(path);
		if (!input.is_open()) return {};
		
		std::string line;
		while (std::getline(input, line)) {
			auto split = splitLineObj(line);
			//std::cout << line << std::endl;
			if (split.size() > 1) {
				if (split[0].compare("v") == 0) {
					if (split.size() != 4)
						return {}; // File corrupt
					positions.push_back(std::stof(split[1]));
					positions.push_back(std::stof(split[2]));
					positions.push_back(std::stof(split[3]));
				}
				else if (split[0].compare("f") == 0) {
					if (split.size() != 4) 
						return {}; // File corrupt
					indices.push_back(std::stoi(split[1]) - 1);
					indices.push_back(std::stoi(split[2]) - 1);
					indices.push_back(std::stoi(split[3]) - 1);
				}
			}
		}


		input.close();
		normalize(positions);

		//for (auto x : indices) std::cout << x << std::endl;

		return modelData;
	}

}
