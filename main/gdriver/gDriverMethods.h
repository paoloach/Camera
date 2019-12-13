//
// Created by paolo on 12/12/19.
//

#pragma once

#include <memory>

std::unique_ptr<char> searchForFolder(char * folderName);
std::unique_ptr<char> createFolder(char * folderName);
