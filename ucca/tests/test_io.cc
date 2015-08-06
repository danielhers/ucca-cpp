//
// Created by danielh on 8/6/15.
//

#include <boost/filesystem.hpp>

#include "ucca/passage.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) exit(0);
  boost::filesystem::path in_dir(argv[1]);
  boost::filesystem::path out_dir(argv[2]);
  for (auto it = boost::filesystem::directory_iterator(in_dir); it != boost::filesystem::directory_iterator(); ++it) {
    Passage* passage = Passage::load(it->path().c_str());
    string outfname = out_dir.string() + boost::filesystem::path::preferred_separator +
        boost::filesystem::basename(it->path()) + boost::filesystem::extension(it->path());
    passage->save(outfname.c_str());
  }
}
