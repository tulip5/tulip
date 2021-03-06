#!/bin/bash

# this script intents to generate c++ code needed to init
# structures dealing with Font Awesome icons
# the first parameter is the _variable.scss file
# found in the scss sub directory
# after extracting files from a fontawesome downladable zip archive
# the second is the cpp file which must be copied as
# library/tulip-ogl/src/TulipInitFontAwesome.cpp
# The files fa-[brands-400|regular-400|solid-900].[ttf|woff|woff2] from
# the webfonts sub directory must be copied in library/tulip-ogl/bitmaps

FA_VARIABLES_FILE=$1
CPP_FILE=$2

FA_VERSION=$(grep fa-version ${FA_VARIABLES_FILE} | awk -F '"' '{print $2}')
(echo "// Warning: do not update this file !!!";
 echo "// It was automatically generated by utils/scripts/generate_fa_cpp_init_code.sh";
 echo "// from Font Awesome icons version ${FA_VERSION}";
 echo;
 echo "std::string TulipFontAwesome::getVersion() {"
 echo "  return \"${FA_VERSION}\";";
 echo "}";
 echo;
 echo "static void initIconCodePoints() {";
 # grep 'fa-var-' ${FA_VARIABLES_FILE} | awk -F ';' '{print $1}' | awk -F '$' '{print $2}' | awk -F 'var-' '{print $1 $2}' | awk -F ': \' '{printf "  addIconCodePoint(\"%s\", 0x%s);\n", $1, $2}';
 grep 'fa-var-' ${FA_VARIABLES_FILE} | awk -F ';' '{print $1}' | awk -F '$' '{print $2}' | awk -F 'var-' '{print $1 $2}' | awk -F ': ' '{print $1 $2}' | awk -F '\' '{printf "  addIconCodePoint(\"%s\", 0x%s);\n", $1, $2}';
 echo;
 echo "#error fa-regular-400.ttf needs to be patched because the defined font has the same name that the one in fa-solid-900.ttf (see library/tulip-ogl/src/TulipFontAwesome.cpp)"
 echo;
 echo "  clearFtFaces();";
 echo;
 echo "  iconsNames.reserve(iconCodePoint.size());";
 echo;
 echo "  for (const auto &it : iconCodePoint) {";
 echo "    iconsNames.emplace_back(it.first);";
 echo "  }";
 echo "}") > ${CPP_FILE}

