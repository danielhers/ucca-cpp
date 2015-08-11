#ifndef UCCA_CORPUS_H_
#define UCCA_CORPUS_H_

#include <string>
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <functional>
#include <vector>
#include <map>
#include <string>

#include <boost/filesystem.hpp>

#include "passage.h"

namespace ucca {

  static const char *const SHIFT = "SHIFT";
  static const char *const SWAP = "SWAP";
  static const char *const LEFT_ARC = "LEFT-ARC";
  static const char *const RIGHT_ARC = "RIGHT-ARC";
  static const char *const PROMOTE = "PROMOTE";
  static const char *const REMOTE = "REMOTE";
  static const char *const NO_OP = "NO-OP";

  class Corpus {
  public:
    std::map<unsigned, std::vector<unsigned>> correct_action_passages;
    std::map<unsigned, std::vector<unsigned>> passages;
    unsigned npassages;

    std::map<unsigned, std::vector<unsigned>> correct_action_passages_dev;
    std::map<unsigned, std::vector<unsigned>> passages_dev;
    std::map<unsigned, std::vector<std::string>> passages_str_dev;
    unsigned npassages_dev;

    unsigned nwords;
    unsigned nactions;

    int max;

    std::map<std::string, unsigned> wordsToInt;
    std::map<unsigned, std::string> intToWords;
    std::vector<std::string> actions;

    // String literals
    static constexpr const char *UNK = "UNK";
    static constexpr const char *BAD0 = "<BAD0>";


  public:
    Corpus() {
      max = 0;
    }

    inline void load_correct_actions(const std::string dir, bool dev=false) {
      unsigned passage = 0;
      wordsToInt[BAD0] = 0;
      intToWords[0] = BAD0;
      wordsToInt[UNK] = 1; // unknown symbol
      intToWords[1] = UNK;
      assert(max == 0);
      max = 2;

      std::vector<unsigned> terminals;
      std::vector<std::string> buffer;
      std::vector<std::string> stack;

      for (auto it = boost::filesystem::directory_iterator(boost::filesystem::path(dir));
           it != boost::filesystem::directory_iterator(); ++it) {
        Passage* p = Passage::load(it->path().c_str());
        std::vector<std::string> current_passage_str;

        terminals.clear();
        // add terminals to buffer
        for (auto it = p->layers[0]->nodes.begin(); it != p->layers[0]->nodes.end(); ++it) {
          const std::string& word = it->second->text;

          // new word or OOV
          if (wordsToInt[word] == 0) {
            if (dev) {
              current_passage_str.push_back(word);
              word = UNK;
            } else {
              wordsToInt[word] = max;
              intToWords[max] = word;
              nwords = max;
              max++;
            }
          } else if (dev) { // word found
            // add an empty string for any token except OOVs (it is easy to
            // recover the surface form of non-OOV using intToWords(id)).
            current_passage_str.push_back("");
          }

          terminals.push_back(wordsToInt[word]);
          buffer.push_back(it->first);
        } // terminals

        // simulate parsing on the given structure
        while (stack.size() > 2 || buffer.size() > 1) {
          Node* current = p->nodes[buffer.back()];
          for (auto parent_it = current->incoming.begin(); parent_it != current->incoming.end(); ++parent_it) {
            if (std::find(buffer.begin(), buffer.end(), parent_it->second->from->id) != buffer.end()) {
              // TODO link to parent
            } else {
              add_correct_action(passage, dev, PROMOTE);
            }
          }
        }

        if (dev) {
          passages_dev[passage] = terminals;
          passages_str_dev[passage] = current_passage_str;
        } else {
          passages[passage] = terminals;
        }

        passage++;
      }
      if (dev) {
        npassages_dev = passage;
      } else {
        npassages = passage;
      }
      nactions = actions.size();

      std::cerr << "done" << std::endl;
      for (auto a: actions) {
        std::cerr << a << std::endl;
      }
      std::cerr << "nactions:" << nactions << std::endl;
      std::cerr << "nwords:" << nwords << std::endl;
    }

    inline unsigned get_or_add_word(const std::string &word) {
      unsigned &id = wordsToInt[word];
      if (id == 0) {
        id = max;
        ++max;
        intToWords[id] = word;
        nwords = max;
      }
      return id;
    }

    inline void add_correct_action(unsigned int passage, bool dev, const std::string actionS) {
      auto actionIter = std::find(actions.begin(), actions.end(), actionS);
      if (actionIter != actions.end()) {
        unsigned actionIndex = std::distance(actions.begin(), actionIter);
        if (dev) {
          correct_action_passages_dev[passage].push_back(actionIndex);
        } else {
          correct_action_passages[passage].push_back(actionIndex);
        }
      } else if (!dev) {
        correct_action_passages[passage].push_back(actions.size());
        actions.push_back(actionS);
      }
    }

  };

} // namespace

#endif
