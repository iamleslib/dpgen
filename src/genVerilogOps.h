

#ifndef DPGEN_H
#define DPGEN_H

#include <string>
#include <vector>

std::string genVerilogOperations(std::vector<std::vector<std::string>> operationsLines,
	std::vector<std::vector<std::string>> inputLines,
	std::vector<std::vector<std::string>> outputLines,
	std::vector<std::vector<std::string>> wireLines,
	std::vector<std::vector<std::string>> registerLines);


#endif