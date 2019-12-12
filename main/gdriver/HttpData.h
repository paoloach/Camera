//
// Created by paolo on 03/12/19.
//

#pragma once

#include <cstring>
#include <bits/unique_ptr.h>
#include <cstdint>

struct HttpDataLink {
  HttpDataLink(char  * _data, uint16_t dataLen):
      data(std::unique_ptr<char  >(new char[dataLen+1])),
      len(dataLen), next(nullptr){
    memcpy(data.get(), _data, len);
    data.get()[dataLen]=0;
  }
  std::unique_ptr<char  > data;
  uint16_t len;
  struct HttpDataLink * next;
};



class HttpData {
public:
  HttpData():list(nullptr){}
  ~HttpData(){
    HttpDataLink * chunk=list;
    while (chunk != nullptr){
      HttpDataLink * nextChunk=chunk->next;
      delete chunk;
      chunk  = nextChunk;
    }
  }
  void clear();
  void addData(char * data, uint16_t len);
  std::unique_ptr<char > getContent();

private:
  HttpDataLink * getLastChunk(){
    HttpDataLink * chunk;
    for(chunk=list; chunk->next != nullptr; chunk=chunk->next);
    return chunk;
  }
  HttpDataLink * list;
};



