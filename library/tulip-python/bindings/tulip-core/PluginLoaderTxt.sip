/*
 *
 * This file is part of Tulip (http://tulip.labri.fr)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

// +-------------------------------------------------------------------------+
// | Tulip Python Bindings                                                   |
// | inspired from bindings by the Booggie project development team          |
// | (https://sourceforge.net/projects/booggie/)                             |
// +-------------------------------------------------------------------------+

namespace tlp {

class PluginLoaderTxt : tlp::PluginLoader {
%TypeHeaderCode
#include <tulip/PluginLoaderTxt.h>
%End

public:
  
  virtual void start(const std::string &path);
  
  virtual void loading(const std::string &filename);
  
  virtual void loaded(const tlp::Plugin* info, const std::list <tlp::Dependency>&);
  
  virtual void aborted(const std::string &filename,const  std::string &erreurmsg);
  
  virtual void finished(bool state,const std::string &msg);
  
};

};
