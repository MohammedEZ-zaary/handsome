#ifndef FILE_MANAGER
#define FILE_MANAGER
#include <optional>
#include <string>

namespace FileManager {
void saveFileBuffer(const std::string &imageBuffer,
                    const std::string &fileName);

std::string readFileContent(const std::string &filePath);
bool removeFile(std::string path);

std::optional<std::string> findFileFullPath(const std::string &fileName,
                                            const std::string &directory = ".");

} // namespace FileManager

#endif
