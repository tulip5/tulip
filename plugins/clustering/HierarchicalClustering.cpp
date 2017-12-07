/**
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

#include "HierarchicalClustering.h"

using namespace std;
using namespace tlp;

PLUGIN(HierarchicalClustering)

//================================================================================
HierarchicalClustering::HierarchicalClustering(PluginContext *context) : Algorithm(context) {}
//================================================================================
HierarchicalClustering::~HierarchicalClustering() {}
//================================================================================

class LessThan {
public:
  DoubleProperty *metric;
  bool operator()(node n1, node n2) {
    return (metric->getNodeValue(n1) < metric->getNodeValue(n2));
  }
};

bool HierarchicalClustering::split(DoubleProperty *metric, list<node> &orderedNode) {

  for (const node &n : graph->nodes())
    orderedNode.push_back(n);

  LessThan comp;
  comp.metric = metric;
  orderedNode.sort(comp);

  list<node>::iterator itListNode;
  double tmpDbl;

  // We split the sorted list in two part
  int nbElement = orderedNode.size();
  nbElement /= 2;

  if (nbElement < 10)
    return (true);

  itListNode = orderedNode.begin();
  tmpDbl = metric->getNodeValue(*itListNode);
  int n = 0;
  ++itListNode;
  nbElement--;

  while ((itListNode != orderedNode.end()) &&
         ((nbElement > 0) || (tmpDbl == metric->getNodeValue(*itListNode)))) {
    tmpDbl = metric->getNodeValue(*itListNode);
    ++itListNode;
    n++;
    nbElement--;
  }

  orderedNode.erase(itListNode, orderedNode.end());
  return false;
}
//================================================================================
bool HierarchicalClustering::run() {

  string tmp1, tmp2;
  DoubleProperty *metric = graph->getProperty<DoubleProperty>("viewMetric");
  tmp1 = "Hierar Sup";
  tmp2 = "Hierar Inf";
  bool result = false;

  while (!result) {
    list<node> badNodeList;
    result = split(metric, badNodeList);

    if (!result) {
      BooleanProperty sel1(graph);
      BooleanProperty sel2(graph);
      BooleanProperty splitRes(graph);

      sel1.setAllNodeValue(true);
      sel1.setAllEdgeValue(true);
      sel2.setAllNodeValue(true);
      sel2.setAllEdgeValue(true);
      splitRes.setAllNodeValue(true);
      splitRes.setAllEdgeValue(true);

      list<node>::iterator itl;

      for (itl = badNodeList.begin(); itl != badNodeList.end(); ++itl)
        splitRes.setNodeValue(*itl, false);

      for (const node &nit : graph->nodes()) {

        if (splitRes.getNodeValue(nit)) {
          sel2.setNodeValue(nit, false);

          for (const edge &ite : graph->getInOutEdges(nit)) {
            sel2.setEdgeValue(ite, false);
          }

        } else {
          sel1.setNodeValue(nit, false);

          for (const edge &ite : graph->getInOutEdges(nit)) {
            sel1.setEdgeValue(ite, false);
          }
        }
      }

      Graph *tmpSubGraph;
      tmpSubGraph = graph->addSubGraph(&sel1);
      tmpSubGraph->setAttribute<string>("name", tmp1);
      (graph->addSubGraph(&sel2))->setAttribute<string>("name", tmp2);
      graph = tmpSubGraph;
    }
  }

  return true;
}
