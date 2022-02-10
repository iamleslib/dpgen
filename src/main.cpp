
//  main.cpp
//  Assignment 2
//  Sean Farris, Dario Salazar, Leslie Ibarra.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "genVerilogOps.h"

using namespace std;


/////////// Function Prototypes //////////////////////////////
string GenerateHeadersAndPorts(vector<vector<string>> inputLines, vector<vector<string>> outputLines, vector<vector<string>> wireLines, vector<vector<string>> regLines,
                               vector<vector<string>> operationsLines, string verilogFileName);

bool existence(vector <string> outputVars, string tempVars);
///////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {
    
    string inFileName;
    string outFileName;
    
    //Checking for correct command line arguments //////////////////////////////////////////////
    if (argc == 3)    //Correct # of arguments
    {
        //Setting file names for input command arguments
        inFileName = argv[1];
        outFileName = argv[2];
        
        if (!(inFileName.substr(inFileName.length() - 4) == ".txt") || !(outFileName.substr(outFileName.length() - 2) == ".v"))    //netlist file specified is type .txt
        {
            cout << "Usage: " << argv[0] << " <netlistFile> <verilogFile>" << endl;
            return -1;
        }
    }
    else
    {
        //Error message for incorrect # of command arguments
        cout << "Incorrect number of command arguments entered" << endl;
        cout << "Usage: " << argv[0] << " <netlistFile> <verilogFile>" << endl;
        return -1;
    }
    
    ///////////////End Checking command line arguments.///////////////////////////////////////////////////////////////////////
    
    
    vector<vector<string>> inputLines;
    vector<vector<string>> outputLines;
    vector<vector<string>> wireLines;
    vector<vector<string>> registerLines;
    vector<vector<string>> operationLines;
    vector<string> inputVars;
    vector<string> outputVars;
    vector<string> wireVars;
    vector<string> registerVars;
    vector<string> tempVector;
    vector <string> validOperators = { "=", "+", "-", "*", "==", "<<", ">>", "<", ">", "?", ":" };
    
    ifstream file(inFileName);
    if (!file)
    {
        cout << "Unable to open the file.\n";
        return -1;
    }
    if (file.peek() == std::ifstream::traits_type::eof()) // Check if file is empty.
    {
        cout << "The input file was empty. Uh-oh!!!\n";
        return -1;
    }
    
    string str;
    
    while (std::getline(file, str)) {
        //std::cout << str << "\n";
        istringstream iss(str);
        int stop = 0;
        ///seperate individual words
        do
        {
            string subs;
            iss >> subs;
            if ((subs.size() >= 2) && ((subs[0] == '/') && (subs[1] == '/'))){
                
                stop = 1;
                subs = "";
            }
            else{
                
                if (!subs.empty() && (stop == 0)){
                    for (int i = 0; i<subs.length(); i++)
                        if (subs[i] == ','){
                            subs.resize(i);
                        }
                    tempVector.push_back(subs);
                    //cout << "Substring: " << subs << endl;
                }
            }
        } while (iss);
        ///// END seperate individual words
        
        
        ////START seperate into input, output, wire, register, vector of string vectors
        if (tempVector.empty()){ // Check if tempVector is null
            //Do Nothing, go to next line.
        }
        else if (tempVector[0].compare("input") == 0){
            //cout<< tempVector[0];
            inputLines.push_back(tempVector);
            for (int i = 2; i < tempVector.size(); i++){
                inputVars.push_back(tempVector[i]);
            }
            
            
        }
        else if (tempVector[0].compare("output") == 0){
            outputLines.push_back(tempVector);
            for (int i = 2; i < tempVector.size(); i++){
                outputVars.push_back(tempVector[i]);
            }
            
        }
        else if (tempVector[0].compare("wire") == 0){
            wireLines.push_back(tempVector);
            for (int i = 2; i < tempVector.size(); i++){
                wireVars.push_back(tempVector[i]);
            }
            
        }
        else if (tempVector[0].compare("register") == 0){
            registerLines.push_back(tempVector);
            for (int i = 1; i < tempVector.size(); i++){
                registerVars.push_back(tempVector[i]);
            }
            
        }
        else {
            operationLines.push_back(tempVector);
        }
        
        tempVector.clear();
        
    }
    ////END seperate into input, output, wire, register, vector of string vectors
    
    /////// START Check if variables exist
    ///// check left side of equal sign if it is an output, wire, register variable
    for (int i = 0; i < operationLines.size(); i++){
        if (operationLines[i][1].compare("=") == 0){ // checks assignment statement variables
            if ((existence(outputVars, operationLines[i][0]) || existence(wireVars, operationLines[i][0]) || existence(registerVars, operationLines[i][0])) == false){
                cout << "Error in Input File1" << endl;
                return 0;
            }
        }
    }
    
    for (int i = 0; i < operationLines.size(); i++){
        
        if (operationLines[i].size() == (3)){
            if (!((existence(inputVars, operationLines[i][2]) || existence(wireVars, operationLines[i][2])) || existence(registerVars, operationLines[i][2]))){
                cout << "Error in Input File2" << endl;
                return 0;
            }
            
        }
        if (operationLines[i].size() == (5)){
            
            if (!((existence(inputVars, operationLines[i][2]) || existence(wireVars, operationLines[i][2]) || existence(registerVars, operationLines[i][2])))){
                
                cout << "Error in Input File3" << endl;
                return 0;
            }
            if (!existence(validOperators, operationLines[i][3])){
                cout << "Error in Input File4" << endl;
                return 0;
            }
            
            if (!((existence(inputVars, operationLines[i][4]) || existence(wireVars, operationLines[i][4]) || existence(registerVars, operationLines[i][4])))){
                
                cout << "Error in Input File5" << endl;
                return 0;
            }
            
        }
        if (operationLines[i].size() == (7)){
            if (!((existence(inputVars, operationLines[i][2]) || existence(wireVars, operationLines[i][2]) || existence(registerVars, operationLines[i][2])))){
                
                cout << "Error in Input File6" << endl;
                return 0;
            }
            
            if (existence(validOperators, operationLines[i][3]) == false){
                cout << "Error in Input File7" << endl;
                return 0;
            }
            if (!((existence(inputVars, operationLines[i][4]) || existence(wireVars, operationLines[i][4]) || existence(registerVars, operationLines[i][4])))){
                
                cout << "Error in Input File8" << endl;
                return 0;
            }
            
            if (!existence(validOperators, operationLines[i][5])){
                cout << "Error in Input File9" << endl;
                return 0;
            }
            if ((existence(inputVars, operationLines[i][6]) || existence(wireVars, operationLines[i][6]) || existence(registerVars, operationLines[i][6])) == false){
                
                cout << "Error in Input File10" << endl;
                return 0;
            }
            
        }
    }
    file.close();
    
    //----------- Generate Verilog Code ---------------------------------------------------
    
    string verilogString = GenerateHeadersAndPorts(inputLines, outputLines, wireLines, registerLines, operationLines, outFileName);
    
    verilogString += genVerilogOperations(operationLines, inputLines, outputLines, wireLines, registerLines);
    
    verilogString += "\nendmodule";
    
    ofstream outFileStream;
    outFileStream.open(outFileName);
    outFileStream << verilogString;
    outFileStream.close();
    
    
    

    
    return 0;
}


//////////////////////////////////////////////////////////////////
//------------- Helper Functions ------------------------------------------------
///////////////////////////////////////////////////////////////////


string GenerateHeadersAndPorts(vector<vector<string>> inputLines, vector<vector<string>> outputLines, vector<vector<string>> wireLines, vector<vector<string>> regLines,
                               vector<vector<string>> operationsLines, string verilogFileName)
/* This function generates the header, inputs, outputs, wires, and regs for verilogFile */
{
    
    //Header for verilog file
    string verilogStr(" `timescale 1ns / 1ps \n\n\n");
    
    //Finding module name
    size_t found = verilogFileName.find_last_of("/");
    string circuitFileName = verilogFileName.substr(found + 1);        //getting rid of leading path name
    
    found = circuitFileName.find_last_of("\\");
    circuitFileName = circuitFileName.substr(found + 1);
    
    //Setting module name
    string moduleName("module " + circuitFileName.erase(circuitFileName.size() - 2) + "(");
    verilogStr += moduleName;
    
    //Loop through input ports
    for (int i = 0; i < inputLines.size(); i++)
    {
        for (int j = 2; j < inputLines[i].size(); j++)
        {
            verilogStr += inputLines[i][j] + ", ";
            
        }
    }
    
    //Loop through output ports
    for (int i = 0; i < outputLines.size(); i++)
    {
        for (int j = 2; j < outputLines[i].size(); j++)
        {
            verilogStr += outputLines[i][j];
            if (!(i == outputLines.size() - 1 && j == outputLines[i].size() - 1))
            {
                verilogStr += ", ";
            }
            
        }
    }
    
    //Checking if Clk and Rst ports are needed
    bool clkAndRstNeeded = false;
    for (int i = 0; i < operationsLines.size(); ++i)
    {
        if (operationsLines[i].size() == 3)
        {
            //Checking the number of elements in inner vector of operationsLines if equal to 3 (i.e. we have a register operation and need Clk and Rst)
            verilogStr += ", Clk, Rst";
            clkAndRstNeeded = true;
            break;
        }
        
    }
    
    
    verilogStr += ");\n\n";
    
    
    //-------------->Declaring input variables
    if (clkAndRstNeeded)
    {
        verilogStr += "input Clk, Rst;\n";
    }
    
    
    for (int i = 0; i < inputLines.size(); i++)
    {
        verilogStr += inputLines[i][0];
        if (inputLines[i][1][0] == 'I')
        {
            verilogStr += " signed ";
        }
        
        //Finding bitwith of inputs
        size_t found = inputLines[i][1].find_last_of("t");
        string bitWidth = inputLines[i][1].substr(found + 1);        //getting bitwidth size
        
        verilogStr += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
        
        for ( int j = 2; j < inputLines[i].size(); j++)
        {
            verilogStr += inputLines[i][j];
            
            if (!(j == inputLines[i].size() - 1))
            {
                verilogStr += ",  ";
            }
        }
        verilogStr += ";\n";
    }
    verilogStr += "\n";
    
    
    
    //------------->Declaring output variables
    for (int i = 0; i < outputLines.size(); i++)
    {
        verilogStr += outputLines[i][0];
        if (outputLines[i][1][0] == 'I')
        {
            verilogStr += " signed ";
        }
        
        //Finding bitwith of outputs
        size_t found = outputLines[i][1].find_last_of("t");
        string bitWidth = outputLines[i][1].substr(found + 1);        //getting bitwidth size
        
        verilogStr += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
        
        for (long int j = 2; j < outputLines[i].size(); j++)
        {
            verilogStr += outputLines[i][j];
            if (!(j == outputLines[i].size() - 1))
            {
                verilogStr += ",  ";
            }
            
        }
        
        verilogStr += ";\n";
    }
    
    verilogStr += "\n";
    
    
    //--------------->Declaring wires
    for (int i = 0; i < wireLines.size(); i++)
    {
        verilogStr += wireLines[i][0];
        if (wireLines[i][1][0] == 'I')
        {
            verilogStr += " signed ";
        }
        
        //Finding bitwith of wires
        size_t found = wireLines[i][1].find_last_of("t");
        string bitWidth = wireLines[i][1].substr(found + 1);        //getting bitwidth size
        
        verilogStr += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
        
        for (long int j = 2; j < wireLines[i].size(); j++)
        {
            verilogStr += wireLines[i][j];
            if (!(j == wireLines[i].size() - 1))
            {
                verilogStr += ",  ";
            }
            
        }
        
        verilogStr += ";\n";
    }
    
    verilogStr += "\n";
    
    
    //------------->Declaring registers
    for (int i = 0; i < regLines.size(); i++)
    {
        verilogStr += "wire";
        if (regLines[i][1][0] == 'I')
        {
            verilogStr += " signed ";
        }
        
        //Finding bitwith of registers
        size_t found = regLines[i][1].find_last_of("t");
        string bitWidth = regLines[i][1].substr(found + 1);        //getting bitwidth size
        
        verilogStr += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
        
        for (long int j = 2; j < regLines[i].size(); j++)
        {
            verilogStr += regLines[i][j];
            if (!(j == regLines[i].size() - 1))
            {
                verilogStr += ",  ";
            }
            
        }
        
        verilogStr += ";\n";
    }
    
    verilogStr += "\n";
    
    return verilogStr;
}


bool existence(vector <string> outputVars, string tempVars){
    if (std::find(outputVars.begin(), outputVars.end(), tempVars) != outputVars.end())
        return true;
    else
        return false;
}

