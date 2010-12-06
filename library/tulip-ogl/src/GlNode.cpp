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
#include "tulip/OpenGlConfigManager.h"

#include "tulip/GlNode.h"

#include <tulip/Coord.h>
#include <tulip/LayoutProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/PreferenceManager.h>

#include "tulip/GlTools.h"
#include "tulip/GlyphManager.h"
#include "tulip/GlDisplayListManager.h"
#include "tulip/OcclusionTest.h"
#include "tulip/TextRenderer.h"
#include "tulip/GlTLPFeedBackBuilder.h"
#include "tulip/GlSceneVisitor.h"
#include "tulip/GlGraphRenderingParameters.h"
#include "tulip/GlRenderer.h"
#include "tulip/GlTextureManager.h"
#include "tulip/GlVertexArrayManager.h"
#include "tulip/GlLabel.h"

#include <iostream>

//====================================================
#ifdef _WIN32
#ifdef DLL_EXPORT
tlp::GlLabel* tlp::GlNode::label=0;
#endif
#else
tlp::GlLabel* tlp::GlNode::label=0;
#endif

using namespace std;

namespace tlp {

  GlNode::GlNode(unsigned int id):id(id) {
    if(!label)
      label=new GlLabel();
  }

  BoundingBox GlNode::getBoundingBox(GlGraphInputData* data) {
    node n=node(id);
    if(data->elementRotation->getNodeValue(n)==0){
      BoundingBox box;
      box.expand(data->elementLayout->getNodeValue(n)-data->elementSize->getNodeValue(n)/2.f);
      box.expand(data->elementLayout->getNodeValue(n)+data->elementSize->getNodeValue(n)/2.f);
      assert(box.isValid());
      return box;
    }else{
      float cosAngle=cos((float)data->elementRotation->getNodeValue(n)/180.*M_PI);
      float sinAngle=sin((float)data->elementRotation->getNodeValue(n)/180.*M_PI);
      Coord tmp1(data->elementSize->getNodeValue(n)/2.f);
      Coord tmp2(tmp1[0] ,-tmp1[1], tmp1[2]);
      Coord tmp3(-tmp1[0],-tmp1[1],-tmp1[2]);
      Coord tmp4(-tmp1[0], tmp1[1],-tmp1[2]);
      tmp1=Coord(tmp1[0]*cosAngle-tmp1[1]*sinAngle,tmp1[0]*sinAngle+tmp1[1]*cosAngle,tmp1[2]);
      tmp2=Coord(tmp2[0]*cosAngle-tmp2[1]*sinAngle,tmp2[0]*sinAngle+tmp2[1]*cosAngle,tmp2[2]);
      tmp3=Coord(tmp3[0]*cosAngle-tmp3[1]*sinAngle,tmp3[0]*sinAngle+tmp3[1]*cosAngle,tmp3[2]);
      tmp4=Coord(tmp4[0]*cosAngle-tmp4[1]*sinAngle,tmp4[0]*sinAngle+tmp4[1]*cosAngle,tmp4[2]);
      BoundingBox bb;
      bb.expand(data->elementLayout->getNodeValue(n)+tmp1);
      bb.expand(data->elementLayout->getNodeValue(n)+tmp2);
      bb.expand(data->elementLayout->getNodeValue(n)+tmp3);
      bb.expand(data->elementLayout->getNodeValue(n)+tmp4);
      return bb;
    }
  }

  void GlNode::acceptVisitor(GlSceneVisitor *visitor) {
    visitor->visit(this);
  }

  void GlNode::draw(float lod,GlGraphInputData* data,Camera*) {
    const Color& colorSelect2=data->parameters->getSelectionColor();

    glEnable(GL_CULL_FACE);
    GLenum error = glGetError();
    if(GlDisplayListManager::getInst().beginNewDisplayList("selection")) {
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_LIGHTING);
      glDepthFunc(GL_LEQUAL);
      glLineWidth(3);
      tlp::cube(GL_LINE_LOOP);
      glPopAttrib();
      GlDisplayListManager::getInst().endNewDisplayList();
    }

    node n=node(id);
    /*if(data->parameters->isElementZOrdered()){
      if(data->elementColor->getNodeValue(n)[3]!=255){
        GlPointManager::getInst().endRendering();
        GlPointManager::getInst().beginRendering();
      }
    }*/

    if (data->elementSelected->getNodeValue(n)) {
      glDisable(GL_DEPTH_TEST);
      if(data->getGraph()->isMetaNode(n)) {
	glStencilFunc(GL_LEQUAL,data->parameters->getSelectedMetaNodesStencil(),0xFFFF);
      }else{
	glStencilFunc(GL_LEQUAL,data->parameters->getSelectedNodesStencil(),0xFFFF);
      }
    }else{
      glEnable(GL_DEPTH_TEST);
      if(data->getGraph()->isMetaNode(n)) {
	glStencilFunc(GL_LEQUAL,data->parameters->getMetaNodesStencil(),0xFFFF);
      }else{
	glStencilFunc(GL_LEQUAL,data->parameters->getNodesStencil(),0xFFFF);
      }
    }

    const Coord &nodeCoord = data->elementLayout->getNodeValue(n);
    const Size &nodeSize = data->elementSize->getNodeValue(n);

    const Color& fillColor = data->elementColor->getNodeValue(n);
    const Color& strokeColor = data->elementBorderColor->getNodeValue(n);
    const Color& textColor = data->elementLabelColor->getNodeValue(n);
    GlTextureManager::getInst().setAnimationFrame(data->elementAnimationFrame->getNodeValue(n));

    if(data->parameters->getFeedbackRender()) {
      glPassThrough(TLP_FB_COLOR_INFO);
      glPassThrough(fillColor[0]);glPassThrough(fillColor[1]);glPassThrough(fillColor[2]);glPassThrough(fillColor[3]);
      glPassThrough(strokeColor[0]);glPassThrough(strokeColor[1]);glPassThrough(strokeColor[2]);glPassThrough(strokeColor[3]);
      glPassThrough(textColor[0]);glPassThrough(textColor[1]);glPassThrough(textColor[2]);glPassThrough(textColor[3]);

      glPassThrough(TLP_FB_BEGIN_NODE);
      glPassThrough(id); //id of the node for the feed back mode
    }

    bool selected = data->elementSelected->getNodeValue(n);
    if (lod < 10.0) { //less than four pixel on screen, we use points instead of glyphs
      if (lod < 1) lod = 1;
      int size= sqrt(lod);

      if(data->getGlVertexArrayManager()->renderingIsBegin()){
        if(size<2)
          data->getGlVertexArrayManager()->activatePointNodeDisplay(this,true,selected);
        else
          data->getGlVertexArrayManager()->activatePointNodeDisplay(this,false,selected);
      }else{
        if(size>2)
          size=2;
        const Color& color = selected ? colorSelect2 : fillColor;

        OpenGlConfigManager::getInst().activateLineAndPointAntiAliasing();
        glDisable(GL_LIGHTING);
        setColor(color);
        glPointSize(size);
        glBegin(GL_POINTS);
        glVertex3f(nodeCoord[0], nodeCoord[1], nodeCoord[2]+nodeSize[2]/2.);
        glEnd();
        glEnable(GL_LIGHTING);
        OpenGlConfigManager::getInst().desactivateLineAndPointAntiAliasing();
      }
      return;

    }
    //draw a glyph or make recursive call for meta nodes
    glPushMatrix();
    glTranslatef(nodeCoord[0], nodeCoord[1], nodeCoord[2]);
    glRotatef(data->elementRotation->getNodeValue(n), 0., 0., 1.);
    glScalef(nodeSize[0], nodeSize[1], nodeSize[2]);

    data->glyphs.get(data->elementShape->getNodeValue(n))->draw(n,lod);

    if (selected) {
      //glStencilFunc(GL_LEQUAL,data->parameters->getNodesStencil()-1,0xFFFF);
      setColor(colorSelect2);
      OpenGlConfigManager::getInst().activateLineAndPointAntiAliasing();
      GlDisplayListManager::getInst().callDisplayList("selection");
      OpenGlConfigManager::getInst().desactivateLineAndPointAntiAliasing();
    }
    glPopMatrix();

    if (selected) {
      glStencilFunc(GL_LEQUAL,data->parameters->getNodesStencil(),0xFFFF);
    }

    GlTextureManager::getInst().setAnimationFrame(0);

    if(data->parameters->getFeedbackRender()) {
      glPassThrough(TLP_FB_END_NODE);
    }
    if ( error != GL_NO_ERROR)
    cerr << "end [OpenGL Error] => " << gluErrorString(error) << endl << "\tin : " << __PRETTY_FUNCTION__ << endl;
  }

  void GlNode::drawLabel(bool drawSelect,OcclusionTest* test,TextRenderer* renderer,GlGraphInputData* data,float lod) {
    node n=node(id);
    bool selected=data->elementSelected->getNodeValue(n);
    if(drawSelect!=selected)
      return;

    drawLabel(test,renderer,data,lod);
  }

  void GlNode::drawLabel(OcclusionTest* test,TextRenderer* renderer,GlGraphInputData* data) {
    GlNode::drawLabel(test,renderer,data,1000.);
  }

  void GlNode::drawLabel(OcclusionTest* test,TextRenderer* renderer,GlGraphInputData* data,float lod, Camera *camera) {

    node n=node(id);

    // If glyph can't render label : return
    if(data->glyphs.get(data->elementShape->getNodeValue(n))->renderLabel())
      return;

    // node is selected
    bool selected=data->elementSelected->getNodeValue(n);
    // Color of the label : selected or not
    const Color& fontColor = selected ? data->parameters->getSelectionColor() :data->elementLabelColor->getNodeValue(n);
    // Size of the node
    Size size=data->elementSize->getNodeValue(n);

    // If we have transparent label : return
    if(fontColor.getA()==0)
      return;

    // Node text
    const string &tmp = data->elementLabel->getNodeValue(n);
    if (tmp.length() < 1)
      return;

    if(!data->getGraph()->isMetaNode(n)){
      if(selected) label->setStencil(data->parameters->getSelectedNodesStencil());
      else label->setStencil(data->parameters->getNodesLabelStencil());
    }else{
      if(selected) label->setStencil(data->parameters->getSelectedMetaNodesStencil());
      else label->setStencil(data->parameters->getMetaNodesLabelStencil());
    }

    int fontSize=data->elementFontSize->getNodeValue(n);
    if(selected)
      fontSize+=2;

    const Coord &nodeCoord = data->elementLayout->getNodeValue(n);
    const Size  &nodeSize  = data->elementSize->getNodeValue(n);
    int labelPos = data->elementLabelPosition->getNodeValue(n);


    BoundingBox includeBB;
    data->glyphs.get(data->elementShape->getNodeValue(n))->getTextBoundingBox(includeBB,n);
    Coord centerBB(includeBB.center());
    Vec3f sizeBB = includeBB[1]-includeBB[0];

    label->setText(tmp);
    label->setFontNameSizeAndColor(data->elementFont->getNodeValue(n),fontSize,fontColor);
    label->setRenderingMode(GlLabel::POLYGON_MODE);
    label->setTranslationAfterRotation(centerBB*nodeSize);
    label->setSize(Coord(nodeSize[0]*sizeBB[0],nodeSize[1]*sizeBB[1],0));
    label->setSizeForOutAlign(Coord(nodeSize[0],nodeSize[1],0));
    label->rotate(0,0,data->elementRotation->getNodeValue(n));
    label->setAlignment(labelPos);
    label->setScaleToSize(data->parameters->isLabelScaled());
    label->setUseLODOptimisation(true);
    label->setLabelOcclusionBorder(data->parameters->getLabelsBorder());
    if(!data->parameters->isLabelOverlaped())
      label->setOcclusionTester(test);
    else
      label->setOcclusionTester(NULL);

    if(includeBB[1][2]!=0)
      label->setPosition(Coord(nodeCoord[0],nodeCoord[1],nodeCoord[2]+nodeSize[2]/2.));
    else
      label->setPosition(Coord(nodeCoord[0],nodeCoord[1],nodeCoord[2]));

    label->drawWithStencil(lod,camera);
  }

  void GlNode::getPointAndColor(GlGraphInputData *inputData,std::vector<Coord> &pointsCoordsArray,std::vector<Color> &pointsColorsArray){
    node n=node(id);
    const Coord &nodeCoord = inputData->elementLayout->getNodeValue(n);
    const Color& fillColor = inputData->elementColor->getNodeValue(n);
    pointsCoordsArray.push_back(nodeCoord);
    pointsColorsArray.push_back(fillColor);
  }

  void GlNode::getColor(GlGraphInputData *inputData,std::vector<Color> &pointsColorsArray){
    node n=node(id);
    const Color& fillColor = inputData->elementColor->getNodeValue(n);
    pointsColorsArray.push_back(fillColor);
  }
}
