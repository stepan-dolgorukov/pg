#pragma once

#include <cstddef> // std::size_t

char** generate_passwords(
	std::size_t amount,
	std::size_t length,
	bool * const options);