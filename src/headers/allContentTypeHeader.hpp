#ifndef ALL_CONTENT_TYPE_HEADER_HPP
#define ALL_CONTENT_TYPE_HEADER_HPP
#include <map>
#include <string>
class ContentTypeMapper {
public:
    ContentTypeMapper(){
        // Initialize the content type map
        contentTypeMap = {
            {".txt", "text/plain"},
            {".html", "text/html"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".xml", "application/xml"},
            {".csv", "text/csv"},
            {".json", "application/json"},
            {".pdf", "application/pdf"},
            {".zip", "application/zip"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".svg", "image/svg+xml"},
            {".webp", "image/webp"},
            {".mp3", "audio/mpeg"},
            {".wav", "audio/wav"},
            {".ogg", "audio/ogg"},
            {".mp4", "video/mp4"},
            {".webm", "video/webm"},
            {".ogv", "video/ogg"},
            {".bin", "application/octet-stream"},
            {".xls", "application/vnd.ms-excel"},
            {".ppt", "application/vnd.ms-powerpoint"},
            {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            {".flv", "application/x-shockwave-flash"},
            {".form", "application/x-www-form-urlencoded"},
            {".graphql", "application/graphql"},
            {".jsonld", "application/ld+json"},
        };
 
    };
    // Function to get content type header based on the filename
    std::string getContentTypeHeader(const std::string& filename);
private:
    // Method to extract file extension from the filename
    std::string extractFileExtension(const std::string& filename);
    std::map<std::string, std::string> contentTypeMap; // Content type mapping
};
#endif