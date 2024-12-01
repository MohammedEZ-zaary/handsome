#include "../../../include/httpServer/fileManager/fileHandlerUtils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace FileManager {

void saveFileBuffer(const std::string &imageBuffer,
                    const std::string &fileName) {
  // Open the file in binary mode
  std::ofstream outFile(fileName, std::ios::binary);
  if (!outFile) {
    std::cerr << "Error: Unable to open file " << fileName << " for writing."
              << std::endl;
    return;
  }

  // Write the buffer to the file
  outFile.write(imageBuffer.data(), imageBuffer.size());
  // Close the file
  outFile.close();
  std::cout << "Image saved successfully to " << fileName << "." << std::endl;
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
