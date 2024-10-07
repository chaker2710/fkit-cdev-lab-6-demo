#include <fstream>
#include <vector>
#include <string>

/**
 * @brief Simple file database serialization and deserialization utility.
 *
 * Copyright © 2024 Danylo Kolodij - All Rights Reserved
 * You may use, distribute and modify this code under the terms of the MIT license.
 * You should have received a copy of the MIT license with this file. If not, please visit https://opensource.org/licenses/MIT
 */
namespace SimpleDB {
    const std::string filename = "data.bin";

    template <typename T>
    void serialize(const std::vector<T>& data) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Could not open file for writing.");
        }

        const size_t size = data.size();
        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));  // Write the size of the array first

        for (const T& item : data) {
            outFile.write(reinterpret_cast<const char*>(&item), sizeof(T));  // Write the raw data of each item
        }

        outFile.close();
    }

    template <typename T>
    std::vector<T> load() {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::vector<T> data(0);  // If the file does not exist,
            return data;             // return an empty vector
        }

        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));  // Read the size of the array

        std::vector<T> data(size);
        for (T& item : data) {
            inFile.read(reinterpret_cast<char*>(&item), sizeof(T));  // Read the raw data of each item
        }

        inFile.close();
        return data;
    }
}