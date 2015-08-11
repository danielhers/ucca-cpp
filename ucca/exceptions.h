//
// Created by danielh on 8/9/15.
//

#ifndef UCCA_EXCEPTIONS_H
#define UCCA_EXCEPTIONS_H

class xml_exception : public std::runtime_error {
public:
  xml_exception(const std::string s) : runtime_error(s) { }
};

class file_not_found_exception : public std::runtime_error {
public:
  file_not_found_exception(const std::string fname) : runtime_error(std::string("File not found: ") + fname) { }
};

#endif //UCCA_EXCEPTIONS_H
