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

// ------- Working with filename -------
inline std::string filename = "data.bin";

inline void setFilename(const std::string &file) {
  filename = file;
} // ! Function for changing the file name
// -------------------------------------
////
////
////
////
////
// --- Reading and writing elements ---

//!  Use this functions in the implementation of serialize your structs.
template <typename T> void writeElement(std::ofstream &outFile, const T &item) {
  if constexpr (std::is_same_v<T, std::string>) {
    const size_t size = item.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));
    outFile.write(item.data(), size);
  } else {
    outFile.write(reinterpret_cast<const char *>(&item), sizeof(T));
  }
}

//!  Use this functions in the implementation of deserialize your structs.
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
// --------------------------------------
////
////
////
////
////
// -------------- Concepts --------------
template <typename T>
concept Serializable = requires(const T t, std::ofstream &out) {
  { t.serialize(out) } -> std::same_as<void>;
};

template <typename T>
concept Deserializable = requires(T t, std::ifstream &in) {
  { t.deserialize(in) } -> std::same_as<void>;
};

template <typename T>
concept Archivable = Serializable<T> && Deserializable<T>; // Universal
// --------------------------------------
////
////
////
////
////
// ----- Working with the database -----

// ! Entry into the database. Accepts vector as a parameter, not returns.
template <Archivable T> void serialize(const std::vector<T> &data) {
  std::ofstream outFile(filename, std::ios::binary);
  if (!outFile) {
    throw std::runtime_error("Could not open file for writing.");
  }

  const size_t size = data.size();
  outFile.write(reinterpret_cast<const char *>(&size),
                sizeof(size)); // Write the size of the array first

  for (const T &item : data) {
    item.serialize(outFile);
  }

  outFile.close();
}

// ! Unloading from the database. Does not accept parameters, returns a vector.
template <Archivable T> std::vector<T> load() {
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
    item.deserialize(inFile); // Read the raw data of each item for structs
  }

  inFile.close();
  return data;
}
// --------------------------------------

} // namespace SimpleDB