#pragma once

#include <vector>

#include <tuple>
#include <string>
#include <optional>

namespace sr {

	using Model3D = std::tuple<std::vector<float>, std::vector<int>>;

	std::optional<Model3D> loadObj(const std::string& path);

}
