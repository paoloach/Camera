//
// Created by paolo on 12/12/19.
//

#pragma once

#include <memory>

std::unique_ptr<char> searchForFolder(const char * folderName);
std::unique_ptr<char> createFolder(const char * folderName);
std::unique_ptr<char> createFileMetadata(const char * folderNameId, const char * fileName);
std::unique_ptr<char> uploadFileData(const char * fileId,  uint8_t * data, size_t dataLen);


