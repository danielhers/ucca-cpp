//
// Created by danielh on 7/30/15.
//

#ifndef UCCA_PASSAGE_H
#define UCCA_PASSAGE_H

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace ucca {

  class Passage;

  class Layer;

  class Node;

  class Edge;

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

  class xml_exception : public std::runtime_error {
  public:
    xml_exception(const std::string &s) : runtime_error(s) { }
  };

  class file_not_found_exception : public std::runtime_error {
  public:
    file_not_found_exception(const char *fname) : runtime_error(std::string("File not found: ") + fname) { }
  };

  class Passage {
  public:
    Passage(unsigned id);

    ~Passage();

    static Passage *load(const char *fname) {
      std::ifstream in;
      in.open(fname);
      if (!in.is_open()) throw file_not_found_exception(fname);
      auto p = load(in);
      in.close();
      return p;
    }

    static Passage *load(const std::string &s) { return load(s.c_str()); }

    static Passage *load(std::istream &);

    void save(const char *fname) const {
      std::ofstream out;
      out.open(fname);
      if (!out.is_open()) throw file_not_found_exception(fname);
      save(out);
      out.close();
    }

    void save(const std::string &s) const { save(s.c_str()); }

    void save(std::ostream &) const;

    unsigned id;
    unsigned annotation_id;
    std::map<unsigned, Layer *> layers;
    std::map<std::string, Node *, cmp_id> nodes;
    std::map<std::pair<std::string, std::string>, Edge *, cmp_id_pair> edges;

  private:
    Passage() : id(0), annotation_id(0) { };
  };

  class Layer {
  public:
    friend class Passage;

    Layer(unsigned id);

    ~Layer();

    unsigned id;
    std::map<std::string, Node *, cmp_id> nodes;
    std::map<std::pair<std::string, std::string>, Edge *, cmp_id_pair> edges;
  };

  class Node {
  public:
    friend class Passage;

    Node(std::string id, std::string type);

    ~Node();

    std::string id;
    std::string type;
    unsigned paragraph;
    unsigned paragraph_position;
    std::string text;
    bool implicit;
    bool uncertain;
    std::string remarks;
    std::map<std::string, Edge *, cmp_id> edges;
  };

  class Edge {
  public:
    friend class Passage;

    Edge(Node *from, std::string to_id, std::string type);

    ~Edge();

    std::string type;
    bool remote;
    Node *from;
    Node *to;

  private:
    std::string to_id;
  };

}

#endif //UCCA_PASSAGE_H
