/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
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

#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <tulip/ViewWidget.h>
#include <tulip/BooleanProperty.h>
#include <QtCore/QModelIndex>

namespace Ui {
class TableViewWidget;
}

namespace tlp {
class GraphModel;
}

class TableViewConfiguration;
class PropertiesEditor;

class TableView : public tlp::ViewWidget {
  Q_OBJECT

  Ui::TableViewWidget* _ui;
  PropertiesEditor* propertiesEditor;
  tlp::GraphModel* _model;

public:
  PLUGININFORMATIONS("Spreadsheet","Tulip Team","04/17/2012","Spreadsheet view for raw data","4.0","")

  TableView(tlp::PluginContext*);
  virtual ~TableView();

  tlp::DataSet state() const;
  void setState(const tlp::DataSet &);
  void setupWidget();
  QList<QWidget*> configurationWidgets() const;

public slots:
  void readSettings();
  void setPropertyVisible(tlp::PropertyInterface*,bool);
  void filterChanged();
  tlp::BooleanProperty* getFilteringProperty() const;
  bool hasEffectiveFiltering();


protected:
  void graphChanged(tlp::Graph *);
  void graphDeleted();

protected slots:
  void setAllFilteredValue(tlp::PropertyInterface*);
  void delHighlightedRows();
  void toggleHighlightedRows();
  void selectHighlightedRows();
  void setAllHighlightedRows(tlp::PropertyInterface*);
  void mapToGraphSelection();

  void columnsInserted(const QModelIndex&,int,int);
  void showCustomContextMenu(const QPoint & pos);
};

#endif // TABLEVIEW_H
