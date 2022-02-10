//TODO: Account for unsigned/signed operations.

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

///////////////////// Helper Function Prototypes /////////////////////////////////////////////////
unordered_map<string, string> genVarToSizeAndTypeMap(vector<vector<string>> inputLines,
	vector<vector<string>> outputLines,
	vector<vector<string>> wireLines,
	vector<vector<string>> registerLines);

string getBitWidth(string sizeAndTypeToken);
string getInputParamStr(int compBitWidth, int inputBitWidth, string inputVar, string inputSizeAndTypeToken);
void printVVS(vector<vector<string>> vect);
///////////////////////////////////////////////////////////////////////////////////////////


string genVerilogOperations(vector<vector<string>> operationsLines, vector<vector<string>> inputLines,
							vector<vector<string>> outputLines, vector<vector<string>> wireLines, 
							vector<vector<string>> registerLines)
{


	unordered_map<string, string> varToSizeAndTypeMap = genVarToSizeAndTypeMap(inputLines, outputLines, wireLines, registerLines);

	unordered_map<string, int> numberOfComponentsUsed = {
															{"REG", 0},
															{"ADD", 0},
															{"SADD", 0},
															{"SUB", 0},
															{"SSUB", 0},
															{"MUL", 0},
															{"SMUL", 0},
															{"COMP", 0},
															{"SCOMP", 0},
															{"MUX2x1", 0},
															{"SHR", 0}, //May need to add logical...
															{"SHL", 0}
														};

	// opSymbol to compName map.

	string verilogString = "\n";

	//For every operation...
	for (int i = 0; i < operationsLines.size(); i++)
	{
		vector<string> opTokens = operationsLines[i];

		////////////////////// REG Case //////////////////////////////////////////////////////////////////////////////
		//Check to see if the operation has just one operand, implies a register assignment. EX: <a = b>
		if (opTokens.size() == 3)
		{
			//Generate verilog code for a register component...
			
			string outputVar = opTokens[0];
			string inputVar = opTokens[2];

			//Bit width of the reg component is the width of the ouput variable.
			string outputSizeAndTypeToken = varToSizeAndTypeMap[outputVar];
			string bitWidth = getBitWidth(outputSizeAndTypeToken);

			string inputSizeAndTypeToken = varToSizeAndTypeMap[inputVar];
			string inputBitWidth = getBitWidth(inputSizeAndTypeToken);

			//Determine if input param width needs to be zero padded, sign extended, or truncated.
			string inputParamStr = getInputParamStr(stoi(bitWidth), stoi(inputBitWidth), inputVar, inputSizeAndTypeToken);

			//Increment the number of REGs used, so I can assign the REG module a unique name.
			numberOfComponentsUsed["REG"]++; 

			//Generate the verilog module instantiation.
			verilogString = verilogString + "REG #(.DATAWIDTH(" + bitWidth + ")) "
				+ "REG_" + to_string(numberOfComponentsUsed["REG"]) + "("
				+ inputParamStr + ", Clk, Rst, " + outputVar + ");\n";
		} ////////////////// END REG Case //////////////////////////////////////////////////////////////////////////////

		////////////////////// MUX2x1 Case //////////////////////////////////////////////////////////////////////////////
		//Check to see if the operation is a ternary operation, implying a mux. EX: <g = dLTe ? d : e>
		else if (opTokens.size() == 7)
		{
			//Generate verilog code for a MUX2x1 component...

			string outputVar = opTokens[0];
			string inputVarA = opTokens[6]; // Chosen when select bit = 0.
			string inputVarB = opTokens[4];
			string selectVar = opTokens[2];
			string inputASizeAndTypeToken = varToSizeAndTypeMap[inputVarA];
			string inputBSizeAndTypeToken = varToSizeAndTypeMap[inputVarB];

			//Bit width of the MUX2x1 component is the width of the ouput variable.
			string bitWidth = getBitWidth(varToSizeAndTypeMap[outputVar]);
			string A_bitWidth = getBitWidth(varToSizeAndTypeMap[inputVarA]);
			string B_bitWidth = getBitWidth(varToSizeAndTypeMap[inputVarB]);

			//Determine if input param widths needs to be zero padded, sign extended, or truncated.
			string inputAParamStr = getInputParamStr(stoi(bitWidth), stoi(A_bitWidth), inputVarA, inputASizeAndTypeToken);
			string inputBParamStr = getInputParamStr(stoi(bitWidth), stoi(B_bitWidth), inputVarB, inputBSizeAndTypeToken);
			
			//Increment the number of MUX2x1s used, so I can assign the MUX2x1 module a unique name.
			numberOfComponentsUsed["MUX2x1"]++;

			//Generate the verilog module instantiation.
			verilogString = verilogString + "MUX2x1 #(.DATAWIDTH(" + bitWidth + ")) "
				+ "MUX2x1_" + to_string(numberOfComponentsUsed["MUX2x1"]) + "("
				+ inputAParamStr + ", " + inputBParamStr + ", " + selectVar + ", " + outputVar + ");\n";
		} ///////////////// END MUX2x1 Case //////////////////////////////////////////////////////////////////////////////

		/////////////////////////////// +,-,*,>,<,==,>>,<< Case ///////////////////////////////////////////////////////////////
		//Check to see if the operation has 5 tokens, implies one of the following: +,-,*,>,<,==,>>,<<.
		else if (opTokens.size() == 5)
		{
			string outputVar = opTokens[0];
			string inputVarA = opTokens[2];
			string inputVarB = opTokens[4];
			string opSymbol = opTokens[3];
			string inputASizeAndTypeToken = varToSizeAndTypeMap[inputVarA];
			string inputBSizeAndTypeToken = varToSizeAndTypeMap[inputVarB];
			string outputSizeAndTypeToken = varToSizeAndTypeMap[outputVar];

			string A_bitWidth = getBitWidth(inputASizeAndTypeToken);
			string B_bitWidth = getBitWidth(inputBSizeAndTypeToken);
			string output_bitWidth = getBitWidth(outputSizeAndTypeToken);
			string bitWidth;

			//****** Comparator Case **************
			if (opSymbol == ">" || opSymbol == "<" || opSymbol == "==")
			{
				//Bit width of a comparator is the width of the largest input.
				bitWidth = (stoi(A_bitWidth) > stoi(B_bitWidth)) ? A_bitWidth : B_bitWidth;

				//Determine if input param widths needs to be zero padded, sign extended, or truncated.
				string inputAParamStr = getInputParamStr(stoi(bitWidth), stoi(A_bitWidth), inputVarA, inputASizeAndTypeToken);
				string inputBParamStr = getInputParamStr(stoi(bitWidth), stoi(B_bitWidth), inputVarB, inputBSizeAndTypeToken);

				//Determine which output port should be used. .gt(), .lt(), or .eq()
				string outputParamStr;
				if (opSymbol == ">")
				{
					outputParamStr = ".gt(" + outputVar + ")";
				}
				else if (opSymbol == "<")
				{
					outputParamStr = ".lt(" + outputVar + ")";
				}
				else if (opSymbol == "==")
				{
					outputParamStr = ".eq(" + outputVar + ")";
				}

				//Use a signed comparator (SCOMP) if one of the operands is signed.
				string compType; // SCOMP or COMP
				if (inputASizeAndTypeToken[0] == 'I' || inputBSizeAndTypeToken[0] == 'I')
				{
					compType = "SCOMP";
					numberOfComponentsUsed["SCOMP"]++;
				}
				else
				{
					compType = "COMP";
					numberOfComponentsUsed["COMP"]++;
				}
				
				//Generate the verilog module instantiation.
				verilogString = verilogString + compType + " #(.DATAWIDTH(" + bitWidth + ")) "
					+ compType + "_" + to_string(numberOfComponentsUsed[compType]) + "("
					+ ".a(" + inputAParamStr + "), .b(" + inputBParamStr + "), " + outputParamStr + ");\n";
			}
			//**** End Comparator Case ************

			//**** +, -, *, <<, >> Cases ******************
			else
			{
				unordered_map<string, string> opSymbolToCompMap = {
					{ "+", "ADD" },
					{ "-", "SUB" },
					{ "*", "MULT" },
					{ ">>", "SHR" },
					{ "<<", "SHL" } };

				//Bit width of the component is the width of the output variable.
				bitWidth = output_bitWidth;

				//Determine if input param widths needs to be zero padded, sign extended, or truncated.
				string inputAParamStr = getInputParamStr(stoi(bitWidth), stoi(A_bitWidth), inputVarA, inputASizeAndTypeToken);
				string inputBParamStr = getInputParamStr(stoi(bitWidth), stoi(B_bitWidth), inputVarB, inputBSizeAndTypeToken);

				//Check if I need to use a signed or unsigned component.
				string compType = opSymbolToCompMap[opSymbol]; //component type.
				if (inputASizeAndTypeToken[0] == 'I' || inputBSizeAndTypeToken[0] == 'I')
				{
					if (opSymbol != "<<" && opSymbol != ">>") // Don't have signed shift components.
					{
						compType = "S" + compType;
					}
				}
				numberOfComponentsUsed[compType]++;

				//Generate the verilog module instantiation.
				verilogString = verilogString + compType + " #(.DATAWIDTH(" + bitWidth + ")) "
					+ compType + "_" + to_string(numberOfComponentsUsed[compType]) + "("
					+ inputAParamStr + ", " + inputBParamStr + ", " + outputVar + ");\n";
			}
			//**** END +, -, *, <<, >> Cases **************
		} //////////////////////// END +,-,*,>,<,==,>>,<< Case ////////////////////////////////////////////////////////////////

		//Else weird number of opTokens.
		else
		{
			cout << "genVerilogOperations() received a bad operation tokens vector.";
			return NULL;
		}
	} //End for every operation...

	return verilogString;
}


////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////


//Helper function for genVerilogOperations().
unordered_map<string, string> genVarToSizeAndTypeMap(vector<vector<string>> inputLines,
													vector<vector<string>> outputLines, 
													vector<vector<string>> wireLines,
													vector<vector<string>> registerLines)
{


	//Want a mapping like: {{"a", "UInt8"}, ...}
	unordered_map<string, string> varToSizeAndTypeMap;
	
	//For every line starting with "input"....
	for (int i = 0; i < inputLines.size(); i++)
	{
		string typeAndSize = inputLines[i][1]; // Second element of the line looks like: "Int16"
		//For every variable in the line, add it to the map.
		for (int j = 2; j < inputLines[i].size(); j++)
		{
			varToSizeAndTypeMap[inputLines[i][j]] = typeAndSize;
		}
	}
	
	//For every line starting with "output"...
	for (int i = 0; i < outputLines.size(); i++)
	{
		string typeAndSize = outputLines[i][1]; // Second element of the line looks like: "Int16"
		//For every variable in the line, add it to the map.
		for (int j = 2; j < outputLines[i].size(); j++)
		{
			varToSizeAndTypeMap[outputLines[i][j]] = typeAndSize;
		}
	}

	//For every line starting with "wire"...
	for (int i = 0; i < wireLines.size(); i++)
	{
		string typeAndSize = wireLines[i][1]; // Second element of the line looks like: "Int16"
		//For every variable in the line, add it to the map.
		for (int j = 2; j < wireLines[i].size(); j++)
		{
			varToSizeAndTypeMap[wireLines[i][j]] = typeAndSize;
		}
	}

	//For every line starting with "register"...
	for (int i = 0; i < registerLines.size(); i++)
	{
		string typeAndSize = registerLines[i][1]; // Second element of the line looks like: "Int16"
		//For every variable in the line, add it to the map.
		for (int j = 2; j < registerLines[i].size(); j++)
		{
			varToSizeAndTypeMap[registerLines[i][j]] = typeAndSize;
		}
	}

	/////////////////////
	/*cout << "key   " << "   value" << endl;
	for (auto i : varToSizeAndTypeMap)
	{
		cout << i.first << "   " << i.second << endl;
	}
	cout << endl;*/
	//////////////////////
	return varToSizeAndTypeMap;
}


//Helper function.
string getBitWidth(string sizeAndTypeToken)
{
	string bitWidth;
	if (sizeAndTypeToken[0] == 'U')
	{
		// Get the number after "UInt".  Eg "UInt32" -> "32", "UInt1" -> "1".
		bitWidth = sizeAndTypeToken.substr(4);
	}
	else if (sizeAndTypeToken[0] == 'I')
	{
		// Get the number after "Int".  Eg "Int32" -> "32", "Int1" -> "1".
		bitWidth = sizeAndTypeToken.substr(3);
	}
	else
	{
		//Error
		bitWidth = "ERROR in getBitWidth()";
	}
	return bitWidth;
}


// Helper function.  Pads, sign extends, or truncates an input signal's bitwidth. Returns the corresponding verilog string.
string getInputParamStr(int compBitWidth, int inputBitWidth, string inputVar, string inputSizeAndTypeToken)
{
	string inputParamStr;
	if (inputBitWidth > compBitWidth)
	{
		inputParamStr = inputVar + "[" + to_string(compBitWidth - 1) + ":0]"; // Eg b[15:0]
	}
	else if (inputBitWidth < compBitWidth)
	{
		//If input is unsigned, zero pad to bitWidth. Else, sign extend to bitWidth.
		if (inputSizeAndTypeToken[0] == 'U')
		{
			//Add zero padding.
			inputParamStr = "{0," + inputVar + "}";
		}
		else // Signed
		{
			//sign extend. E.g. extend 16bit a to 32bits: { {16{a[15]}}, a }
			int bitDiff = compBitWidth - inputBitWidth;
			inputParamStr = "{ {" + to_string(bitDiff) + "{" + inputVar + "[" + to_string(inputBitWidth - 1)
				+ "]}}, " + inputVar + " }";
		}
	}
	else // inputBitWidth == bitWidth
	{
		inputParamStr = inputVar;
	}

	return inputParamStr;
}

void printVVS(vector<vector<string>> vect)
{
	cout << endl << "The vector: " << endl;
	for (int i = 0; i < vect.size(); i++)
	{
		cout << "{";
		for (int j = 0; j < vect[i].size(); j++)
		{
			cout << "{\"" + vect[i][j] + "\", ";
		}
		cout << "}\n";
	}

}