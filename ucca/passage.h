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

#include "cmp_id.h"
#include "exceptions.h"

namespace ucca {

  class Passage;
  class Layer;
  class Node;
  class Edge;

  // layer 0 node types
  static const char *const PUNCTUATION = "Punctuation";
  static const char *const WORD = "Word";

  // layer 1 node types
  static const char *const FN = "FN";
  static const char *const PNCT = "PNCT";
  static const char *const LKG = "LKG";

  // edge types: foundational layer
  static const char *const A = "A";
  static const char *const P = "P";
  static const char *const S = "S";
  static const char *const D = "D";
  static const char *const C = "C";
  static const char *const E = "E";
  static const char *const N = "N";
  static const char *const R = "R";
  static const char *const H = "H";
  static const char *const L = "L";
  static const char *const F = "F";
  static const char *const G = "G";

  // edge types: linkage
  static const char *const LR = "LR";
  static const char *const LA = "LA";

  // edge types: preterminals
  static const char *const T = "T";
  static const char *const U = "U";

  class Passage {
  public:
    Passage(unsigned id);
    ~Passage();

    static Passage *load(const std::string);
    static Passage *load(std::istream &);

    void save(const std::string);
    void save(std::ostream &) const;

    Node* add_node(unsigned layer_id, Node* node);
    Node* add_node(unsigned layer_id, std::string node_id, const std::string& type);
    Node* add_node(unsigned layer_id, unsigned position, const std::string& type);

    Node* add_terminal(unsigned position, unsigned paragraph, unsigned paragraph_position,
                       const std::string& text);

    Edge* add_edge(unsigned layer_id, Node* node1, Node* node2, std::string type);
    Edge* add_edge(unsigned layer_id, std::string id1, std::string id2, std::string type);
    Edge* add_edge(unsigned int layer_id1, unsigned int position1,
                   unsigned int layer_id2, unsigned int position2, std::string type);

    unsigned id;
    unsigned annotation_id;
    std::map<unsigned, Layer *> layers;
    std::map<std::string, Node *, cmp_id> nodes;
    std::map<std::pair<std::string, std::string>, Edge *, cmp_id_pair> edges;

  private:
    Passage() : id(0), annotation_id(0) {};
    Edge* add_edge(unsigned layer_id, Node *node1, std::string id2, std::string type);
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

    Node(Layer *layer, std::string id, std::string type);
    ~Node();

    std::string id;
    std::string type;
    unsigned paragraph;
    unsigned paragraph_position;
    std::string text;
    bool implicit;
    bool uncertain;
    std::string remarks;
    Layer *layer;
    std::map<std::string, Edge *, cmp_id> outgoing;
    std::map<std::string, Edge *, cmp_id> incoming;
  };

  class Edge {
  public:
    friend class Passage;

    Edge(Node *from, Node *to, std::string type, std::string to_id=to->id);
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
