#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <string>
#include <fstream>
#include <sstream>


namespace parserlib {


    /**
     * Utility function for loading an ASCII file.
     * @param filename name of the file.
     * @param appendZero appends the '\0' character, optionally.
     * @return the file as a string.
     */
    inline std::string loadASCIIFile(const char* filename, const bool appendZero = false) {
        std::ifstream strm(filename);
        std::stringstream buffer;
        buffer << strm.rdbuf();
        if (appendZero) {
            buffer << '\0';
        }
        return buffer.str();
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
