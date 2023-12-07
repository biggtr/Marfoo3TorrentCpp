#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string& encoded_value, size_t& pos) {
    if (pos >= encoded_value.size()) {
        throw std::runtime_error("Invalid position");
    }

    if (encoded_value[pos] == 'l') {
        // Handling List decoding
        ++pos; // Move past the 'l'
        std::vector<json> listElements;

        while (pos < encoded_value.size() && encoded_value[pos] != 'e') {
            // Recursively decode each element within the list
            listElements.push_back(decode_bencoded_value(encoded_value, pos));
        }
        ++pos; // Move past the 'e' character
        return listElements;
    } else if (std::isdigit(encoded_value[pos])) {
        // Handling String decoding
        size_t colon_index = encoded_value.find(':', pos);
        if (colon_index != std::string::npos) {
            std::string number_string = encoded_value.substr(pos, colon_index - pos);
            int64_t number = std::atoll(number_string.c_str());
            std::string str = encoded_value.substr(colon_index + 1, number);
            pos = colon_index + 1 + number; // Move the position
            return json(str);
        } else {
            throw std::runtime_error("Invalid encoded value: " + encoded_value);
        }
    } else if (encoded_value[pos] == 'i') {
        // Handling Integer decoding
        size_t e_index = encoded_value.find('e', pos);
        if (e_index != std::string::npos) {
            std::string number_string = encoded_value.substr(pos + 1, e_index - pos - 1);
            int64_t number = std::atoll(number_string.c_str());
            pos = e_index + 1; // Move the position
            return json(number);
        } else {
            throw std::runtime_error("Invalid encoded value: " + encoded_value);
        }
    } else {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value[pos]);
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: argc < 3 " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
       
        size_t pos = 0; // Initialize pos
        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value,pos);
        std::cout << decoded_value.dump() << std::endl;
    }

    else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
