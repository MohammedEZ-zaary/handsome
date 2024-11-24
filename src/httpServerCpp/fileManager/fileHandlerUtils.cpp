#include "../../../include/httpServer/fileManager/fileHandlerUtils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
namespace FileManager {
void saveImageToFile(const std::vector<char> &body,
                     const std::string &fileName) {
  // Open the file in binary mode
  std::ofstream outFile(fileName, std::ios::binary);
  if (!outFile) {
    std::cerr << "Error: Unable to open file " << fileName << " for writing."
              << std::endl;
    return;
  }

  // Write the buffer to the file
  outFile.write(body.data(), body.size());
  if (!outFile.good()) {
    std::cerr << "Error: Failed to write data to file " << fileName << "."
              << std::endl;
  }

  // Close the file
  outFile.close();
  if (outFile.good()) {
    std::cout << "Image saved successfully to " << fileName << "." << std::endl;
  }
}
std::string readFileContent(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file) {
    std::cerr << "Unable to open the file!" << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf(); // Read file contents into the stringstream
  return buffer.str();    // Return the file content
}
} // namespace FileManager
