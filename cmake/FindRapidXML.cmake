find_path(RAPIDXML_DIR rapidxml.hpp
          PATH_SUFFIXES include
          PATHS
          ${RapidXML_ROOT}
          $ENV{RapidXML_ROOT}
          ~/Library/Frameworks
          /Library/Frameworks
          /usr/local/
          /usr/
          /sw # Fink
          /opt/local/ # DarwinPorts
          /opt/csw/ # Blastwave
          /opt/)