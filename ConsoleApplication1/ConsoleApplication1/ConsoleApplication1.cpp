#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;



#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <regex>

string generateOutputFileName(const std::string& inputFilePath);

// Function to parse the XPDL file and count BPMN elements
void parseXPDL(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file." << std::endl;
        return;
    }

    // Define a map to store element counts
    std::unordered_map<std::string, int> elementCounts;

    std::string line;
    while (std::getline(file, line)) {
        // Use regex to identify BPMN elements by their XML tags and attributes
        std::regex tagRegex("<(bpmn:[^\\s>]+)\\b(?:[^>]*\\b(?:name|type)=\"([^\"]*)\")?");
        std::smatch matches;
        if (std::regex_search(line, matches, tagRegex)) {
            std::string tagName = matches[1];
            std::string elementType = matches[2];

            // Increment the count for the BPMN element
            elementCounts[tagName]++;

            // Increment subtype count if available
            if (!elementType.empty()) {
                elementCounts[elementType]++;
            }
        }
    }

    file.close();

    // Output the counts of each BPMN element
    std::cout << "BPMN_Model_Elements:" << std::endl;
    std::cout << "Total_Events: " << elementCounts["bpmn:event"] << std::endl;
    std::cout << "Start_Events: " << elementCounts["bpmn:startEvent"] << std::endl;
    std::cout << "Intermediate_Events: " << elementCounts["bpmn:intermediateEvent"] << std::endl;
    std::cout << "End_Events: " << elementCounts["bpmn:endEvent"] << std::endl;
    std::cout << "Total_Activities: " << elementCounts["bpmn:task"] << std::endl;
    std::cout << "Tasks: " << elementCounts["bpmn:task"] << std::endl;
    std::cout << "User_Tasks: " << elementCounts["UserTask"] << std::endl;
    std::cout << "Service_Tasks: " << elementCounts["ServiceTask"] << std::endl;
    std::cout << "Script_Tasks: " << elementCounts["ScriptTask"] << std::endl;
    std::cout << "Manual_Tasks: " << elementCounts["ManualTask"] << std::endl;
    std::cout << "Sub_Processes: " << elementCounts["bpmn:subProcess"] << std::endl;
    std::cout << "Total_Gateways: " << elementCounts["bpmn:exclusiveGateway"]
        + elementCounts["bpmn:parallelGateway"] + elementCounts["bpmn:inclusiveGateway"] << std::endl;
    std::cout << "Exclusive_Gateways_XOR: " << elementCounts["bpmn:exclusiveGateway"] << std::endl;
    std::cout << "Parallel_Gateways_AND: " << elementCounts["bpmn:parallelGateway"] << std::endl;
    std::cout << "Inclusive_Gateways_OR: " << elementCounts["bpmn:inclusiveGateway"] << std::endl;
    std::cout << "Total_Artifacts: " << elementCounts["bpmn:dataObject"] + elementCounts["bpmn:group"]
        + elementCounts["bpmn:textAnnotation"] << std::endl;
    std::cout << "Data_Objects: " << elementCounts["bpmn:dataObject"] << std::endl;
    std::cout << "Groups: " << elementCounts["bpmn:group"] << std::endl;
    std::cout << "Annotations: " << elementCounts["bpmn:textAnnotation"] << std::endl;
    std::cout << "Total_Swimlanes: " << elementCounts["bpmn:lane"] << std::endl;
    std::cout << "Pools: " << elementCounts["bpmn:pool"] << std::endl;
    std::cout << "Lanes: " << elementCounts["bpmn:lane"] << std::endl;
}

int calculateCT(const std::string& path) {
    std::ifstream file(path); // Open the modified XPDL file for reading
    if (!file.is_open()) {
        std::cerr << "Cannot open file. Please check the file path and try again." << std::endl;
        return -1; // Return -1 to indicate error
    }

    int totalCycleTime = 0;

    // Regular expression to match activity times in the format "ExecutionTime="X minutes""
    std::regex timeRegex(R"(\bExecutionTime="(\d+)\s+minutes"\b)");

    std::string line;
    while (std::getline(file, line)) {
        // Search for matches using regex
        std::smatch matches;
        if (std::regex_search(line, matches, timeRegex)) {
            // Add the time from each match to the total cycle time
            for (size_t i = 1; i < matches.size(); ++i) {
                totalCycleTime += std::stoi(matches[i].str());
            }
        }
    }

    file.close();

    return totalCycleTime;
}

void assignRandomTime(std::ifstream& inputFile, std::ofstream& outputFile) {
    std::string line;
    while (std::getline(inputFile, line)) {
        // Find task elements and add random execution times
        size_t pos = line.find("<bpmn:task");
        if (pos != std::string::npos) {
            // Generate random execution time between 5 to 15 minutes
            int duration = rand() % 11 + 5;

            // Append execution time attribute to the task element
            line.insert(pos + 11, " ExecutionTime=\"" + std::to_string(duration) + " minutes\"");
        }

        outputFile << line << std::endl;
    }

    std::cout << "Execution times added successfully." << std::endl;
}

void parsetool(const std::string& path, bool flag = false) {
    std::ifstream file(path); // Open the file for reading
    if (!file.is_open()) {
        std::cerr << "Cannot open file. Please check the file path and try again." << std::endl;
        return;
    }

    std::string outputFilePath = generateOutputFileName(path);

    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to create output file." << std::endl;
        file.close();
        return;
    }

    assignRandomTime(file, outputFile);

    file.close();
    outputFile.close();

    std::cout << "Modified XPDL file saved as: " << outputFilePath << std::endl;
}

//std::string generateOutputFileName(const std::string& inputFilePath) {
//    // Find the position of the last dot in the input file name
//    size_t dotPos = inputFilePath.find_last_of('.');
//    if (dotPos == std::string::npos) {
//        // If no dot found, append "_output" to the input file name
//        return inputFilePath + "_output.xpdl";
//    }
//    else {
//        // Insert "_output" before the last dot in the input file name
//        return inputFilePath.substr(0, dotPos) + "_output" + inputFilePath.substr(dotPos);
//    }
//}


int main() {
    string filename,filename2,filename3;
    int cycleTime = -1; // Initialize cycleTime to -1

    srand(static_cast<unsigned int>(std::time(nullptr)));
    
    int choice;
    std::cout << "Menu:" << std::endl;
    std::cout << "1. Parse XPDL file" << std::endl;
    std::cout << "2. Assign Random Times" << std::endl;
    std::cout << "3. Calculate Cycle Time" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
        switch (choice) {
        case 1:
            cout << "Enter the path to the XPDL file: ";
            cin >> filename;
            parseXPDL(filename);
            break;
            break;
        case 2:
            // Seed for random number generation
            cout << "Enter the path to the XPDL file: ";
            cin >> filename;
            parsetool(filename);
            break;
        case 3:
            // Calculate total cycle time
            cout << "Enter the path to the XPDL file: ";
            cin >> filename;
            cycleTime = calculateCT(filename);

            if (cycleTime >= 0) {
                std::cout << "Total Cycle Time: " << cycleTime << " minutes" << std::endl;
            }
            else {
                std::cerr << "Error occurred while calculating cycle time." << std::endl;
            }
            break;
        case 4:
            exit(0);
            break;
        default:
            std::cout << "Invalid choice. Please enter a valid option." << std::endl;
            break;
        }
    

    return 0;
}
string generateOutputFileName(const std::string& inputFilePath) {
    // Find the position of the last dot in the input file name
    size_t dotPos = inputFilePath.find_last_of('.');
    if (dotPos == std::string::npos) {
        // If no dot found, append "_output" to the input file name
        return inputFilePath + "_output.xpdl";
    }
    else {
        // Insert "_output" before the last dot in the input file name
        return inputFilePath.substr(0, dotPos) + "_output" + inputFilePath.substr(dotPos);
    }
}

//void assignRandomTime(std::ifstream& inputFile, std::ofstream& outputFile) {
//    std::string line;
//    while (std::getline(inputFile, line)) {
//        // Find task elements and add random execution times
//        size_t pos = line.find("<bpmn:task");
//        if (pos != std::string::npos) {
//            // Generate random execution time between 5 to 15 minutes
//            int duration = rand() % 11 + 5;
//
//            // Append execution time attribute to the task element
//            line.insert(pos + 11, " ExecutionTime=\"" + std::to_string(duration) + " minutes\"");
//        }
//
//        outputFile << line << std::endl;
//    }
//
//    std::cout << "Execution times added successfully." << std::endl;
//}
//
//std::string generateOutputFileName(const std::string& inputFilePath) {
//    // Find the position of the last dot in the input file name
//    size_t dotPos = inputFilePath.find_last_of('.');
//    if (dotPos == std::string::npos) {
//        // If no dot found, append "_output" to the input file name
//        return inputFilePath + "_output.xpdl";
//    }
//    else {
//        // Insert "_output" before the last dot in the input file name
//        return inputFilePath.substr(0, dotPos) + "_output" + inputFilePath.substr(dotPos);
//    }
//}
//
//int calculateCT(const std::string& path) {
//    std::ifstream file(path); // Open the modified XPDL file for reading
//    if (!file.is_open()) {
//        std::cerr << "Cannot open file. Please check the file path and try again." << std::endl;
//        return -1; // Return -1 to indicate error
//    }
//
//    int totalCycleTime = 0;
//
//    // Regular expression to match activity times in the format "ExecutionTime="X minutes""
//    std::regex timeRegex(R"(\bExecutionTime="(\d+)\s+minutes"\b)");
//
//    std::string line;
//    while (std::getline(file, line)) {
//        // Search for matches using regex
//        std::smatch matches;
//        if (std::regex_search(line, matches, timeRegex)) {
//            // Add the time from each match to the total cycle time
//            for (size_t i = 1; i < matches.size(); ++i) {
//                totalCycleTime += std::stoi(matches[i].str());
//            }
//        }
//    }
//
//    file.close();
//
//    return totalCycleTime;
//}
//
//void parsetool(const std::string& path, bool flag = false) {
//    std::ifstream file(path); // Open the file for reading
//    if (!file.is_open()) {
//        std::cerr << "Cannot open file. Please check the file path and try again." << std::endl;
//        return;
//    }
//
//    std::string outputFilePath = generateOutputFileName(path);
//
//    std::ofstream outputFile(outputFilePath);
//    if (!outputFile.is_open()) {
//        std::cerr << "Error: Unable to create output file." << std::endl;
//        file.close();
//        return;
//    }
//
//    assignRandomTime(file, outputFile);
//
//    file.close();
//    outputFile.close();
//
//    std::cout << "Modified XPDL file saved as: " << outputFilePath << std::endl;
//
//    // Calculate and display the total cycle time
//    int cycleTime = calculateCT(outputFilePath);
//    if (cycleTime >= 0) {
//        std::cout << "Total Cycle Time: " << cycleTime << " minutes" << std::endl;
//    }
//    else {
//        std::cerr << "Error occurred while calculating cycle time." << std::endl;
//    }
//}
//
//void displayRandomTimes(const std::string& path) {
//    std::ifstream file(path); // Open the modified XPDL file for reading
//    if (!file.is_open()) {
//        std::cerr << "Cannot open file. Please check the file path and try again." << std::endl;
//        return;
//    }
//
//    // Regular expression to match task names and their execution times
//    std::regex taskTimeRegex("<bpmn:task\\s+name=\"([^\"]+)\"\\s+ExecutionTime=\"(\\d+)\\s+minutes\"\\s*/>");
//
//        std::cout << "Random Times:" << std::endl;
//    std::string line;
//    while (std::getline(file, line)) {
//        std::smatch matches;
//        if (std::regex_search(line, matches, taskTimeRegex)) {
//            // Display task name and execution time
//            std::cout << "Task: " << matches[1] << ", Execution Time: " << matches[2] << " minutes" << std::endl;
//        }
//    }
//
//    file.close();
//}







