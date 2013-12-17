/**
 * @file    String.h
 * @ingroup Utils
 * @brief   String related functions.
 *
 * Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <algorithm>
#include <string>

namespace Utils {

/**
 * @brief Remove "white" characters inplace on the left of the string (spaces, tabs and endlines)
 *
 * @param[in,out] aString   String modified by the left-trim operation
 *
 * @return Reference to the trimed string
*/
inline std::string& ltrim(std::string& aString) {
    return aString.erase(0, aString.find_first_not_of(" \t\n\r"));
}

/**
 * @brief Remove "white" characters inplace on the right of the string (spaces, tabs and endlines)
 *
 * @param[in,out] aString   String modified by the right-trim operation
 *
 * @return Reference to the trimed string
*/
inline std::string& rtrim(std::string& aString) {
    return aString.erase(aString.find_last_not_of(" \t\n\r")+1);
}

/**
 * @brief Remove "white" characters inplace on both side of the string (spaces, tabs and endlines)
 *
 * @param[in,out] aString   String modified by the trim operation
 *
 * @return Reference to the trimed string
*/
inline std::string& trim(std::string& aString) {
    return ltrim(rtrim(aString));
}


} // namespace Utils
