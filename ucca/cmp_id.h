//
// Created by danielh on 8/9/15.
//

#ifndef UCCA_CMP_ID_H
#define UCCA_CMP_ID_H

#include <vector>
#include <algorithm>

struct cmp_id {
  static char dot_to_space(const char c) {
    return c == '.' ? ' ' : c;
  }

  static std::vector<unsigned> split_id(const std::string &s) {
    std::vector<unsigned> n;
    std::string s_spaces;
    std::transform(s.begin(), s.end(), std::back_inserter(s_spaces), dot_to_space);
    std::stringstream ss(s_spaces);
    std::copy(std::istream_iterator<unsigned>(ss), std::istream_iterator<unsigned>(), std::back_inserter(n));
    return n;
  }

  bool operator()(const std::string &a, const std::string &b) const {
    std::vector<unsigned> a_n = split_id(a);
    std::vector<unsigned> b_n = split_id(b);
    return a_n < b_n;
  }
};

struct cmp_id_pair {
  bool operator()(const std::pair<std::string, std::string> &a,
                  const std::pair<std::string, std::string> &b) const {
    cmp_id cmp;
    if (cmp(a.first, b.first)) return true;
    else if (a.first == b.first && cmp(a.second, b.second)) return true;
    else return false;
  }
};

#endif //UCCA_CMP_ID_H
