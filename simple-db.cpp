#pragma once
#include <boost/pfr.hpp>
#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

/**
 * @brief Simple file database serialization and deserialization utility.
 *
 * Copyright © 2024 Danylo Kolodij - All Rights Reserved
 * You may use, distribute and modify this code under the terms of the MIT
 * license. You should have received a copy of the MIT license with this file.
 * If not, please visit https://opensource.org/licenses/MIT
 */
namespace SimpleDB {
inline std::string filename = "data.bin";

template <typename T> void writeElement(std::ofstream &outFile, const T &item) {
  if constexpr (std::is_same_v<T, std::string>) {
    const size_t size = item.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));
    outFile.write(item.data(), size);
  } else {
    outFile.write(reinterpret_cast<const char *>(&item), sizeof(T));
  }
}

template <typename T> void readElement(std::ifstream &inFile, T &item) {
  if constexpr (std::is_same_v<T, std::string>) {
    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
    item.resize(size);
    inFile.read(item.data(), size);
  } else {
    inFile.read(reinterpret_cast<char *>(&item), sizeof(T));
  }
}

template <typename T> void serialize(const std::vector<T> &data) {
  std::ofstream outFile(filename, std::ios::binary);
  if (!outFile) {
    throw std::runtime_error("Could not open file for writing.");
  }

  const size_t size = data.size();
  outFile.write(reinterpret_cast<const char *>(&size),
                sizeof(size)); // Write the size of the array first

  for (const T &item : data) {
    if constexpr (std::is_class_v<T>) {
      boost::pfr::for_each_field(item, [&outFile](const auto &field) {
        writeElement(outFile, field);
      });
    } else {
      writeElement(outFile, item); // Write the raw data of each item
    }
  }

  outFile.close();
}

template <typename T> std::vector<T> load() {
  std::ifstream inFile(filename, std::ios::binary);
  if (!inFile) {
    std::vector<T> data(0); // If the file does not exist,
    return data;            // return an empty vector
  }

  size_t size;
  inFile.read(reinterpret_cast<char *>(&size),
              sizeof(size)); // Read the size of the array

  std::vector<T> data(size);
  for (T &item : data) {
    if constexpr (std::is_class_v<T>) {
      boost::pfr::for_each_field(item, [&inFile](auto &field) {
        readElement(inFile, field);
      }); // Read the raw data of each item for structs
    } else {
      readElement(inFile, item);
    }
  }

  inFile.close();
  return data;
}
} // namespace SimpleDB