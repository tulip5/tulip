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

#include "tulip/QuickAccessBar.h"

#include <QFontDatabase>
#include <QComboBox>
#include <QDebug>
#include <QListView>
#include <QMainWindow>
#include <QMouseEvent>
#include <QApplication>
#include <QGraphicsView>

#include <tulip/GraphPropertiesModel.h>
#include <tulip/GraphModel.h>
#include <tulip/TulipFontDialog.h>
#include <tulip/ColorProperty.h>
#include <tulip/GlGraphComposite.h>
#include <tulip/GlGraphRenderingParameters.h>
#include <tulip/TlpQtTools.h>
#include <tulip/GlMainWidget.h>
#include <tulip/GlMainView.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/Perspective.h>
#include <tulip/SnapshotDialog.h>
#include <tulip/TulipItemDelegate.h>
#include <tulip/CaptionGraphicsSubItems.h>
#include <tulip/Perspective.h>

using namespace tlp;

class TopPopupComboBox : public QComboBox {
  QListView *_view;

public:
  TopPopupComboBox(QWidget *parent = nullptr) : QComboBox(parent), _view(nullptr) {}

  bool eventFilter(QObject *, QEvent *ev) override {
    if (ev->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEv = static_cast<QMouseEvent *>(ev);

      if (!_view->geometry().contains(mouseEv->globalPos()))
        _view->close();
      else {
        setCurrentIndex(_view->indexAt(mouseEv->pos()).row());
        _view->close();
      }
    }

    return false;
  }

  void showPopup() override {
    QPoint mainWindowPos = tlp::Perspective::instance()->mainWindow()->pos();

    if (_view == nullptr) {
      _view = findChild<QListView *>();
      _view->installEventFilter(this);
      _view->viewport()->installEventFilter(this);
      _view->setParent(nullptr);
      _view->setMouseTracking(true);
    }

    _view->setAutoScroll(false);
    _view->setWindowFlags(Qt::Popup);
    _view->resize(width(), 200);
    _view->move(mapToGlobal(pos()).x() + mainWindowPos.x(),
                QCursor::pos().y() - 200 - height() / 2);
    _view->show();
  }

  void hidePopup() override {
    if (_view != nullptr)
      _view->close();
  }
};

QuickAccessBar::QuickAccessBar(QWidget *parent) : QWidget(parent), _mainView(nullptr) {}

#include "ui_QuickAccessBar.h"

QPushButton *QuickAccessBarImpl::showNodesButton() {
  return _ui->showNodesToggle;
}

QPushButton *QuickAccessBarImpl::showEdgesButton() {
  return _ui->showEdgesToggle;
}

QPushButton *QuickAccessBarImpl::showLabelsButton() {
  return _ui->showLabelsToggle;
}

QPushButton *QuickAccessBarImpl::showLabelScaled() {
  return _ui->labelsScaledToggle;
}

ColorButton *QuickAccessBarImpl::backgroundColorButton() {
  return _ui->backgroundColorButton;
}

QPushButton *QuickAccessBarImpl::showColorInterpolation() {
  return _ui->colorInterpolationToggle;
}

QuickAccessBarImpl::QuickAccessBarImpl(QGraphicsItem *quickAccessBarItem,
                                       QuickAccessButtons buttons, QWidget *parent)
    : QuickAccessBar(parent), _ui(new Ui::QuickAccessBar), _quickAccessBarItem(quickAccessBarItem),
      delegate(new TulipItemDelegate(this)), _oldFontScale(1), _oldNodeScale(1),
      _captionsInitialized(false) {
  QString ss = Perspective::styleSheet();
  ss.append("#QuickAccessBar { background-color: rgba(255,255,255,100); }");
  setStyleSheet(ss);
  _ui->setupUi(this);
  _ui->backgroundColorButton->setDialogTitle("Choose the background color");
  _ui->nodeColorButton->setDialogTitle("Choose the node's default color");
  _ui->edgeColorButton->setDialogTitle("Choose the edge's default color");
  _ui->nodeBorderColorButton->setDialogTitle("Choose the default color for the border of nodes");
  _ui->edgeBorderColorButton->setDialogTitle("Choose the default color for the border of edges");
  _ui->labelColorButton->setDialogTitle("Choose the default color for the label of nodes or edges");

  if (buttons.testFlag(ALLBUTTONS))
    return;

  if (!buttons.testFlag(NODESCOLORCAPTION))
    _ui->nodesColorCaptionButton->hide();

  if (!buttons.testFlag(NODESSIZECAPTION))
    _ui->nodesSizeCaptionButton->hide();

  if (!buttons.testFlag(EDGESCOLORCAPTION))
    _ui->edgesColorCaptionButton->hide();

  if (!buttons.testFlag(EDGESIZECAPTION))
    _ui->edgesSizeCaptionButton->hide();

  if (!buttons.testFlag(SCREENSHOT))
    _ui->screenshotButton->hide();

  if (!buttons.testFlag(BACKGROUNDCOLOR))
    _ui->backgroundColorButton->hide();

  if (!buttons.testFlag(NODECOLOR))
    _ui->nodeColorButton->hide();

  if (!buttons.testFlag(EDGECOLOR))
    _ui->edgeColorButton->hide();

  if (!buttons.testFlag(NODEBORDERCOLOR))
    _ui->nodeBorderColorButton->hide();

  if (!buttons.testFlag(EDGEBORDERCOLOR))
    _ui->edgeBorderColorButton->hide();

  if (!buttons.testFlag(LABELCOLOR))
    _ui->labelColorButton->hide();

  if (!buttons.testFlag(COLORINTERPOLATION))
    _ui->colorInterpolationToggle->hide();

  if (!buttons.testFlag(SIZEINTERPOLATION))
    _ui->sizeInterpolationToggle->hide();

  if (!buttons.testFlag(SHOWEDGES))
    _ui->showEdgesToggle->hide();

  if (!buttons.testFlag(SHOWLABELS))
    _ui->showLabelsToggle->hide();

  if (!buttons.testFlag(LABELSSCALED))
    _ui->labelsScaledToggle->hide();

  if (!buttons.testFlag(NODESHAPE))
    _ui->nodeShapeButton->hide();

  if (!buttons.testFlag(EDGESHAPE))
    _ui->edgeShapeButton->hide();

  if (!buttons.testFlag(NODESIZE))
    _ui->nodeSizeButton->hide();

  if (!buttons.testFlag(EDGESIZE))
    _ui->edgeSizeButton->hide();

  if (!buttons.testFlag(NODELABELPOSITION))
    _ui->labelPositionButton->hide();

  if (!buttons.testFlag(SELECTFONT))
    _ui->fontButton->hide();

  if (!buttons.testFlag(SHOWNODES))
    _ui->showNodesToggle->hide();
}

void QuickAccessBarImpl::addButtonAtEnd(QAbstractButton *button) {
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);
  _ui->horizontalLayout->addWidget(button);
  _ui->horizontalLayout->addItem(spacer);
}

void QuickAccessBarImpl::addButtonsAtEnd(const QVector<QAbstractButton *> &buttonvect) {
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);

  for (auto b : buttonvect) {
    _ui->horizontalLayout->addWidget(b);
  }

  _ui->horizontalLayout->addItem(spacer);
}

void QuickAccessBarImpl::addSeparator() {
  QFrame *sep = new QFrame(this);
  sep->setFrameShape(QFrame::VLine);
  sep->setFrameShadow(QFrame::Sunken);
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);
  _ui->horizontalLayout->addWidget(sep);
  _ui->horizontalLayout->addItem(spacer);
}

QuickAccessBarImpl::~QuickAccessBarImpl() {
  if (_captionsInitialized) {
    delete _captions[0];
    delete _captions[1];
    delete _captions[2];
    delete _captions[3];
  }

  delete _ui;
}

void QuickAccessBar::setGlMainView(GlMainView *v) {
  _mainView = v;
  reset();
}

void QuickAccessBarImpl::reset() {
  _resetting = true;

  _ui->backgroundColorButton->setTulipColor(scene()->getBackgroundColor());
  _ui->colorInterpolationToggle->setChecked(renderingParameters()->isEdgeColorInterpolate());
  _ui->sizeInterpolationToggle->setChecked(renderingParameters()->isEdgeSizeInterpolate());
  _ui->showEdgesToggle->setChecked(renderingParameters()->isDisplayEdges());
  _ui->showNodesToggle->setChecked(renderingParameters()->isDisplayNodes());
  _ui->showLabelsToggle->setChecked(renderingParameters()->isViewNodeLabel());
  _ui->labelsScaledToggle->setChecked(renderingParameters()->isLabelScaled());
  updateFontButtonStyle();
  _resetting = false;
}

void QuickAccessBarImpl::showHideNodesColorCaption() {
  showHideCaption(CaptionItem::NodesColorCaption);
}

void QuickAccessBarImpl::showHideNodesSizeCaption() {
  showHideCaption(CaptionItem::NodesSizeCaption);
}

void QuickAccessBarImpl::showHideEdgesColorCaption() {
  showHideCaption(CaptionItem::EdgesColorCaption);
}

void QuickAccessBarImpl::showHideEdgesSizeCaption() {
  showHideCaption(CaptionItem::EdgesSizeCaption);
}

void QuickAccessBarImpl::showHideCaption(CaptionItem::CaptionType captionType) {
  if (!_captionsInitialized) {
    _captionsInitialized = true;

    _captions[0] = new CaptionItem(_mainView);
    _captions[0]->create(CaptionItem::NodesColorCaption);
    _captions[0]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[0]->captionGraphicsItem()->setVisible(false);

    _captions[1] = new CaptionItem(_mainView);
    _captions[1]->create(CaptionItem::NodesSizeCaption);
    _captions[1]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[1]->captionGraphicsItem()->setVisible(false);

    _captions[2] = new CaptionItem(_mainView);
    _captions[2]->create(CaptionItem::EdgesColorCaption);
    _captions[2]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[2]->captionGraphicsItem()->setVisible(false);

    _captions[3] = new CaptionItem(_mainView);
    _captions[3]->create(CaptionItem::EdgesSizeCaption);
    _captions[3]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[3]->captionGraphicsItem()->setVisible(false);

    for (size_t i = 0; i < 4; i++) {
      connect(_captions[i]->captionGraphicsItem(), SIGNAL(interactionsActivated()),
              _captions[(i + 1) % 4]->captionGraphicsItem(), SLOT(removeInteractions()));
      connect(_captions[i]->captionGraphicsItem(), SIGNAL(interactionsActivated()),
              _captions[(i + 2) % 4]->captionGraphicsItem(), SLOT(removeInteractions()));
      connect(_captions[i]->captionGraphicsItem(), SIGNAL(interactionsActivated()),
              _captions[(i + 3) % 4]->captionGraphicsItem(), SLOT(removeInteractions()));
      connect(_captions[i], SIGNAL(filtering(bool)), _captions[(i + 1) % 4],
              SLOT(removeObservation(bool)));
      connect(_captions[i], SIGNAL(filtering(bool)), _captions[(i + 2) % 4],
              SLOT(removeObservation(bool)));
      connect(_captions[i], SIGNAL(filtering(bool)), _captions[(i + 3) % 4],
              SLOT(removeObservation(bool)));
    }
  }

  size_t captionIndice = 0;

  if (captionType == CaptionItem::NodesSizeCaption)
    captionIndice = 1;
  else if (captionType == CaptionItem::EdgesColorCaption)
    captionIndice = 2;
  else if (captionType == CaptionItem::EdgesSizeCaption)
    captionIndice = 3;

  _captions[captionIndice]->captionGraphicsItem()->setVisible(
      !_captions[captionIndice]->captionGraphicsItem()->isVisible());

  unsigned int numberVisible = 0;

  for (size_t i = 0; i < 4; i++) {
    if (_captions[i]->captionGraphicsItem()->isVisible()) {
      _captions[i]->captionGraphicsItem()->setPos(QPoint(numberVisible * 130, -260));
      numberVisible++;
    }
  }
}

void QuickAccessBarImpl::takeSnapshot() {
  SnapshotDialog dlg(_mainView, _mainView->graphicsView()->window());
  dlg.exec();
}

void QuickAccessBarImpl::setBackgroundColor(const QColor &c) {
  if (scene()->getBackgroundColor() != QColorToColor(c)) {
    scene()->setBackgroundColor(QColorToColor(c));
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setColorInterpolation(bool f) {
  if (renderingParameters()->isEdgeColorInterpolate() != f) {
    renderingParameters()->setEdgeColorInterpolate(f);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setSizeInterpolation(bool f) {
  if (renderingParameters()->isEdgeSizeInterpolate() != f) {
    renderingParameters()->setEdgeSizeInterpolate(f);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelColor(const QColor &c) {

  BooleanProperty *selected = inputData()->getElementSelected();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();
  ColorProperty *labelColors = inputData()->getElementLabelColor();
  ColorProperty *labelBorderColors = inputData()->getElementLabelBorderColor();

  Color color = QColorToColor(c);

  for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
    labelColors->setNodeValue(n, color);
    labelBorderColors->setNodeValue(n, color);
    hasSelected = true;
  }

  if (hasSelected == false) {
    labelColors->setAllNodeValue(color);
    labelBorderColors->setAllNodeValue(color);
  }

  for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
    labelColors->setEdgeValue(e, color);
    labelBorderColors->setEdgeValue(e, color);
    hasSelected = true;
  }

  if (hasSelected == false) {
    labelColors->setAllEdgeValue(color);
    labelBorderColors->setAllEdgeValue(color);
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setAllColorValues(unsigned int eltType, ColorProperty *prop,
                                           const Color &color) {
  BooleanProperty *selected = inputData()->getElementSelected();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();

  if (eltType == NODE) {
    for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
      prop->setNodeValue(n, color);
      hasSelected = true;
    }

    if (hasSelected == false)
      prop->setAllNodeValue(color);
  } else {
    for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
      prop->setEdgeValue(e, color);
      hasSelected = true;
    }

    if (hasSelected == false)
      prop->setAllEdgeValue(color);
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setNodeColor(const QColor &c) {
  setAllColorValues(NODE, inputData()->getElementColor(), QColorToColor(c));
}

void QuickAccessBarImpl::setEdgeColor(const QColor &c) {
  setAllColorValues(EDGE, inputData()->getElementColor(), QColorToColor(c));
}

void QuickAccessBarImpl::setNodeBorderColor(const QColor &c) {
  setAllColorValues(NODE, inputData()->getElementBorderColor(), QColorToColor(c));
}

void QuickAccessBarImpl::setEdgeBorderColor(const QColor &c) {
  setAllColorValues(EDGE, inputData()->getElementBorderColor(), QColorToColor(c));
}

void QuickAccessBarImpl::setAllValues(unsigned int eltType, PropertyInterface *prop) {
  QVariant val = TulipItemDelegate::showEditorDialog(static_cast<tlp::ElementType>(eltType), prop,
                                                     _mainView->graph(), delegate,
                                                     _mainView->graphicsView()->window());

  // Check if edition has been cancelled
  if (!val.isValid())
    return;

  BooleanProperty *selected = inputData()->getElementSelected();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();

  if (eltType == NODE) {
    for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
      GraphModel::setNodeValue(n.id, prop, val);
      hasSelected = true;
    }

    if (hasSelected == false)
      GraphModel::setAllNodeValue(prop, val);
  } else {
    for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
      GraphModel::setEdgeValue(e.id, prop, val);
      hasSelected = true;
    }

    if (hasSelected == false)
      GraphModel::setAllEdgeValue(prop, val);
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setNodeShape() {
  setAllValues(NODE, inputData()->getElementShape());
}

void QuickAccessBarImpl::setEdgeShape() {
  setAllValues(EDGE, inputData()->getElementShape());
}

void QuickAccessBarImpl::setNodeSize() {
  setAllValues(NODE, inputData()->getElementSize());
}

void QuickAccessBarImpl::setEdgeSize() {
  setAllValues(EDGE, inputData()->getElementSize());
}

void QuickAccessBarImpl::setNodeLabelPosition() {
  setAllValues(NODE, inputData()->getElementLabelPosition());
}

void QuickAccessBarImpl::setEdgesVisible(bool v) {
  if (renderingParameters()->isDisplayEdges() != v) {
    renderingParameters()->setDisplayEdges(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setNodesVisible(bool v) {
  if (renderingParameters()->isDisplayNodes() != v) {
    renderingParameters()->setDisplayNodes(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelsVisible(bool v) {
  if (renderingParameters()->isViewNodeLabel() != v) {
    renderingParameters()->setViewNodeLabel(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelsScaled(bool v) {
  if (renderingParameters()->isLabelScaled() != v) {
    renderingParameters()->setLabelScaled(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

GlGraphRenderingParameters *QuickAccessBar::renderingParameters() const {
  return scene()->getGlGraphComposite()->getRenderingParametersPointer();
}

GlGraphInputData *QuickAccessBar::inputData() const {
  return scene()->getGlGraphComposite()->getInputData();
}

GlScene *QuickAccessBar::scene() const {
  return _mainView->getGlMainWidget()->getScene();
}

void QuickAccessBarImpl::selectFont() {
  TulipFontDialog dlg(_mainView->graphicsView()->window());
  dlg.selectFont(TulipFont::fromFile(inputData()->getElementFont()->getNodeDefaultValue().c_str()));

  if (dlg.exec() != QDialog::Accepted || !dlg.font().exists())
    return;

  _mainView->graph()->push();

  Observable::holdObservers();

  inputData()->getElementFont()->setAllNodeValue(QStringToTlpString(dlg.font().fontFile()));
  inputData()->getElementFont()->setAllEdgeValue(QStringToTlpString(dlg.font().fontFile()));
  inputData()->getElementFontSize()->setAllNodeValue(dlg.fontSize());
  inputData()->getElementFontSize()->setAllEdgeValue(dlg.fontSize());

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  updateFontButtonStyle();
  emit settingsChanged();
}

void QuickAccessBarImpl::updateFontButtonStyle() {
  QString fontFile = inputData()->getElementFont()->getNodeDefaultValue().c_str();
  TulipFont selectedFont = TulipFont::fromFile(fontFile);
  _ui->fontButton->setStyleSheet("font-family: " + selectedFont.fontFamily() + "; " +
                                 (selectedFont.isItalic() ? "font-style: italic; " : "") +
                                 (selectedFont.isBold() ? "font-weight: bold; " : ""));
}
