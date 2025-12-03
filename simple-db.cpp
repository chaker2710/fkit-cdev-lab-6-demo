#pragma once
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
namespace SimpleDB
{

  // ------- Working with filename -------
  const std::string filename = "data.bin";
  // -------------------------------------
  ////
  ////
  ////
  ////
  ////
  // --- Reading and writing elements ---

  template <typename T>
  void writeElement(std::ofstream &outFile, const T &item)
  {
    static_assert(!std::is_same<T, std::string>::value, "Use specialized overload for std::string");
    outFile.write(reinterpret_cast<const char *>(&item), sizeof(T));
  }

  static void writeElement(std::ofstream &outFile, const std::string &item)
  {
    const size_t size = item.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));
    outFile.write(item.data(), size);
  }

  template <typename T>
  void readElement(std::ifstream &inFile, T &item)
  {
    static_assert(!std::is_same<T, std::string>::value, "Use specialized overload for std::string");
    inFile.read(reinterpret_cast<char *>(&item), sizeof(T));
  }

  static void readElement(std::ifstream &inFile, std::string &item)
  {
    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
    if (!inFile)
      return;
    item.resize(size);
    inFile.read(&item[0], size);
  }

  // --- Universal func for set fields in struct ---

  enum class Mode
  {
    Read,
    Write
  };

  class FieldStream
  {
    Mode mode;
    std::ifstream *inFile;
    std::ofstream *outFile;

  public:
    FieldStream(std::ifstream &in) : mode(Mode::Read), inFile(&in), outFile(nullptr) {}
    FieldStream(std::ofstream &out) : mode(Mode::Write), inFile(nullptr), outFile(&out) {}

    Mode getMode() const { return mode; }

    //!  Use this functions in the implementation of deserialize your structs.
    template <typename T>
    void process(T &value)
    {
      if (mode == Mode::Write)
        writeElement(*outFile, value);
      else
        readElement(*inFile, value);
    }
  };

  // --------------------------------------
  ////
  ////
  ////
  ////
  ////
  // ----- Working with the database -----

  // ! Entry into the database. Accepts vector as a parameter, not returns.
  template <typename T>
  void serialize(const std::vector<T> &data)
  {
    std::ofstream outFile(filename, std::ios::binary);

    if (!outFile)
    {
      throw std::runtime_error("Could not open file for writing.");
    }

    size_t size = data.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size)); // Write the size of the array first

    FieldStream fs(outFile);

    for (const T &item : data)
      const_cast<T &>(item).setFields(fs);
  }

  // ! Unloading from the database. Does not accept parameters, returns a vector.
  template <typename T>
  std::vector<T> load()
  {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
    {
      std::vector<T> data(0); // If the file does not exist,
      return data;            // return an empty vector
    }

    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size)); // Read the size of the array

    std::vector<T> data(size);
    FieldStream fs(inFile);

    for (T &item : data)
      item.setFields(fs); // Read the raw data of each item for structs

    return data;
  }

  // --------------------------------------

} // namespace SimpleDB