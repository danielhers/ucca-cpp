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

#include "passage.h"

namespace ucca {

  class Corpus {
  public:
    bool USE_SPELLING=false;

    std::map<int,std::vector<unsigned>> correct_act_passages;
    std::map<int,std::vector<unsigned>> passages;
    std::map<int,std::vector<unsigned>> passagesType;

    std::map<int,std::vector<unsigned>> correct_act_passagesDev;
    std::map<int,std::vector<unsigned>> passagesDev;
    std::map<int,std::vector<unsigned>> passagesTypeDev;
    std::map<int,std::vector<std::string>> passagesStrDev;
    unsigned npassagesDev;

    unsigned npassages;
    unsigned nwords;
    unsigned nactions;
    unsigned ntypes;

    unsigned npassagestest;
    unsigned npassagesdev;
    int max;
    int maxType;

    std::map<std::string, unsigned> wordsToInt;
    std::map<unsigned, std::string> intToWords;
    std::vector<std::string> actions;

    std::map<std::string, unsigned> typeToInt;
    std::map<unsigned, std::string> intToType;

    int maxChars;
    std::map<std::string, unsigned> charsToInt;
    std::map<unsigned, std::string> intToChars;

    // String literals
    static constexpr const char* UNK = "UNK";
    static constexpr const char* BAD0 = "<BAD0>";



  public:
    Corpus() {
      max = 0;
      maxType = 0;
      maxChars=0;
    }


    inline unsigned UTF8Len(unsigned char x) {
      if (x < 0x80) return 1;
      else if ((x >> 5) == 0x06) return 2;
      else if ((x >> 4) == 0x0e) return 3;
      else if ((x >> 3) == 0x1e) return 4;
      else if ((x >> 2) == 0x3e) return 5;
      else if ((x >> 1) == 0x7e) return 6;
      else return 0;
    }




    inline void load_correct_actions(const std::string& dir){
      int count=-1;
      int passage=-1;
      bool initial=false;
      bool first=true;
      wordsToInt[Corpus::BAD0] = 0;
      intToWords[0] = Corpus::BAD0;
      wordsToInt[Corpus::UNK] = 1; // unknown symbol
      intToWords[1] = Corpus::UNK;
      assert(max == 0);
      assert(maxType == 0);
      max=2;
      maxType=1;

      charsToInt[BAD0]=1;
      intToChars[1]="BAD0";
      maxChars=1;

      std::vector<unsigned> current_passage;
      std::vector<unsigned> current_passage_types;

      for (auto it = boost::filesystem::directory_iterator(boost::filesystem::path(dir));
           it != boost::filesystem::directory_iterator(); ++it) {
        Passage p(it->path().c_str());
        count = 0;
        if (!first) {
          passages[passage] = current_passage;
          passagesType[passage] = current_passage_types;
        }

        passage++;
        npassages = passage;

        initial = true;
        current_passage.clear();
        current_passage_types.clear();
        if (count == 0) {
          first = false;
          //stack and buffer, for now, leave it like this.
          count = 1;
          if (initial) {
            // the initial line in each passage may look like:
            // [][the-det, cat-noun, is-verb, on-adp, the-det, mat-noun, ,-punct, ROOT-ROOT]
            // first, get rid of the square brackets.
            lineS = lineS.substr(3, lineS.size() - 4);
            // read the initial line, token by token "the-det," "cat-noun," ...
            std::istringstream iss(lineS);
            do {
              std::string word;
              iss >> word;
              if (word.size() == 0) { continue; }
              // remove the trailing comma if need be.
              if (word[word.size() - 1] == ',') {
                word = word.substr(0, word.size() - 1);
              }
              // split the string (at '-') into word and POS tag.
              size_t typeIndex = word.rfind('-');
              if (typeIndex == std::string::npos) {
                std::cerr << "cant find the dash in '" << word << "'" << std::endl;
              }
              assert(typeIndex != std::string::npos);
              std::string type = word.substr(typeIndex + 1);
              word = word.substr(0, typeIndex);
              // new POS tag
              if (typeToInt[type] == 0) {
                typeToInt[type] = maxType;
                intToType[maxType] = type;
                ntypes = maxType;
                maxType++;
              }

              // new word
              if (wordsToInt[word] == 0) {
                wordsToInt[word] = max;
                intToWords[max] = word;
                nwords = max;
                max++;

                unsigned j = 0;
                while(j < word.length()) {
                  std::string wj = "";
                  for (unsigned h = j; h < j + UTF8Len(word[j]); h++) {
                    wj += word[h];
                  }
                  if (charsToInt[wj] == 0) {
                    charsToInt[wj] = maxChars;
                    intToChars[maxChars] = wj;
                    maxChars++;
                  }
                  j += UTF8Len(word[j]);
                }
              }

              current_passage.push_back(wordsToInt[word]);
              current_passage_types.push_back(typeToInt[type]);
            } while(iss);
          }
          initial=false;
        }
        else if (count==1){
          int i=0;
          bool found=false;
          for (auto a: actions) {
            if (a==lineS) {
              std::vector<unsigned> a=correct_act_passage[passage];
              a.push_back(i);
              correct_act_passage[passage]=a;
              found=true;
            }
            i++;
          }
          if (!found) {
            actions.push_back(lineS);
            std::vector<unsigned> a=correct_act_passage[passage];
            a.push_back(actions.size()-1);
            correct_act_passage[passage]=a;
          }
          count=0;
        }
      }


      // Add the last passage.
      if (current_passage.size() > 0) {
        passages[passage] = current_passage;
        passagesType[passage] = current_passage_types;
        passage++;
        npassages = passage;
      }

      std::cerr<<"done"<<"\n";
      for (auto a: actions) {
        std::cerr<<a<<"\n";
      }
      nactions=actions.size();
      std::cerr<<"nactions:"<<nactions<<"\n";
      std::cerr<<"nwords:"<<nwords<<"\n";
      for (unsigned i=0;i<ntypes;i++){
        std::cerr<<i<<":"<<intToType[i]<<"\n";
      }
      nactions=actions.size();

    }

    inline unsigned get_or_add_word(const std::string& word) {
      unsigned& id = wordsToInt[word];
      if (id == 0) {
        id = max;
        ++max;
        intToWords[id] = word;
        nwords = max;
      }
      return id;
    }

    inline void load_correct_actionsDev(std::string file) {
      std::ifstream actionsFile(file);
      std::string lineS;

      assert(maxType > 1);
      assert(max > 3);
      int count = -1;
      int passage = -1;
      bool initial = false;
      bool first = true;
      std::vector<unsigned> current_passage;
      std::vector<unsigned> current_passage_types;
      std::vector<std::string> current_passage_str;
      while (getline(actionsFile, lineS)) {
        ReplaceStringInPlace(lineS, "-RRB-", "_RRB_");
        ReplaceStringInPlace(lineS, "-LRB-", "_LRB_");
        if (lineS.empty()) {
          // an empty line marks the end of a passage.
          count = 0;
          if (!first) {
            passagesDev[passage] = current_passage;
            passagesTypeDev[passage] = current_passage_types;
            passagesStrDev[passage] = current_passage_str;
          }

          passage++;
          npassagesDev = passage;

          initial = true;
          current_passage.clear();
          current_passage_types.clear();
          current_passage_str.clear();
        } else if (count == 0) {
          first = false;
          //stack and buffer, for now, leave it like this.
          count = 1;
          if (initial) {
            // the initial line in each passage may look like:
            // [][the-det, cat-noun, is-verb, on-adp, the-det, mat-noun, ,-punct, ROOT-ROOT]
            // first, get rid of the square brackets.
            lineS = lineS.substr(3, lineS.size() - 4);
            // read the initial line, token by token "the-det," "cat-noun," ...
            std::istringstream iss(lineS);
            do {
              std::string word;
              iss >> word;
              if (word.size() == 0) { continue; }
              // remove the trailing comma if need be.
              if (word[word.size() - 1] == ',') {
                word = word.substr(0, word.size() - 1);
              }
              // split the string (at '-') into word and POS tag.
              size_t typeIndex = word.rfind('-');
              assert(typeIndex != std::string::npos);
              std::string type = word.substr(typeIndex + 1);
              word = word.substr(0, typeIndex);
              // new POS tag
              if (typeToInt[type] == 0) {
                typeToInt[type] = maxType;
                intToType[maxType] = type;
                ntypes = maxType;
                maxType++;
              }
              // add an empty string for any token except OOVs (it is easy to
              // recover the surface form of non-OOV using intToWords(id)).
              current_passage_str.push_back("");
              // OOV word
              if (wordsToInt[word] == 0) {
                if (USE_SPELLING) {
                  max = nwords + 1;
                  //std::cerr<< "max:" << max << "\n";
                  wordsToInt[word] = max;
                  intToWords[max] = word;
                  nwords = max;
                } else {
                  // save the surface form of this OOV before overwriting it.
                  current_passage_str[current_passage_str.size()-1] = word;
                  word = Corpus::UNK;
                }
              }
              current_passage.push_back(wordsToInt[word]);
              current_passage_types.push_back(typeToInt[type]);
            } while(iss);
          }
          initial = false;
        } else if (count == 1) {
          auto actionIter = std::find(actions.begin(), actions.end(), lineS);
          if (actionIter != actions.end()) {
            unsigned actionIndex = std::distance(actions.begin(), actionIter);
            correct_act_passageDev[passage].push_back(actionIndex);
          } else {
            // TODO: right now, new actions which haven't been observed in training
            // are not added to correct_act_passageDev. This may be a problem if the
            // training data is little.
          }
          count=0;
        }
      }

      // Add the last passage.
      if (current_passage.size() > 0) {
        passagesDev[passage] = current_passage;
        passagesTypeDev[passage] = current_passage_types;
        passagesStrDev[passage] = current_passage_str;
        passage++;
        npassagesDev = passage;
      }

      actionsFile.close();
    }

    void ReplaceStringInPlace(std::string& subject, const std::string& search,
                              const std::string& replace) {
      size_t type = 0;
      while ((type = subject.find(search, type)) != std::string::npos) {
        subject.replace(type, search.length(), replace);
        type += replace.length();
      }
    }

  } // namespace

#endif
