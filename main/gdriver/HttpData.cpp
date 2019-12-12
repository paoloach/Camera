//
// Created by paolo on 03/12/19.
//

#include "HttpData.h"
#include <cstring>
#include <esp_log.h>

void HttpData::addData(char *data, uint16_t len) {
  auto * chunk = new HttpDataLink(data, len);
  if (list== nullptr){
    list=chunk;
  } else {
    auto lastChunk = getLastChunk();
    lastChunk->next =chunk;
  }
}

void HttpData::clear(){
  HttpDataLink * chunk=list;
  while (chunk != nullptr){
    HttpDataLink * nextChunk=chunk->next;
    delete chunk;
    chunk  = nextChunk;
  }
  list= nullptr;
}

std::unique_ptr<char> HttpData::getContent() {
  uint16_t size=0;
  HttpDataLink * chunk;
  for(chunk=list; chunk != nullptr; chunk=chunk->next){
    size+=chunk->len;
  }

  if (size == 0)
    return nullptr;
  auto flatData = std::unique_ptr<char >(new char[size+1]);
  char * iter=flatData.get();
  for(chunk=list; chunk != nullptr; chunk=chunk->next){
    memcpy(iter, chunk->data.get(), chunk->len);
    iter+=chunk->len;
  }
  *iter=0;
  return flatData;
}
