#include "../../../include/httpServer/fileManager/fileHandlerUtils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
std::mutex fileMutex;
namespace fs = std::filesystem;

namespace FileManager {

void saveFileBuffer(const std::string &imageBuffer,
                    const std::string &fileName) {
  std::lock_guard<std::mutex> lock(fileMutex);
  // Open the file in binary mode
  std::ofstream outFile(fileName, std::ios::binary | std::ios::app);
  if (!outFile) {
    std::cerr << "Error: Unable to open file " << fileName << " for writing."
              << std::endl;
    return;
  }
  // Write the buffer to the file
  outFile.write(imageBuffer.data(), imageBuffer.size());
  // Close the file
  outFile.close();
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

bool removeFile(std::string filePath) {
  try {
    // Check if the file exists
    if (fs::exists(filePath)) {
      // Remove the file
      if (fs::remove(filePath)) {
        return true;
      } else {
        std::cerr << "Failed to remove the file: " << filePath << std::endl;
        return false;
      }
    } else {
      return false;
      std::cout << "File not found: " << filePath << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;

    return false;
  }
}
// function check if the path folder is existe
bool isPathFolderExists(std::string folderPath) {
  fs::path folder(folderPath);
  if (fs::exists(folder) && fs::is_directory(folder)) {
    return true;
  } else {
    return false;
  }
}

// Function to find the full path of a file by name
std::optional<std::string> findFileFullPath(const std::string &fileName,
                                            const std::string &directory) {
  try {
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
      if (entry.is_regular_file() && entry.path().filename() == fileName) {
        return entry.path().string();
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return std::nullopt; // File not found
}
} // namespace FileManager
