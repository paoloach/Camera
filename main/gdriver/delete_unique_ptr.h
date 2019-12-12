//
// Created by paolo on 08/12/19.
//
#pragma once

#include <memory>
#include <functional>

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;
