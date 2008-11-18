#ifdef WIN32
    // Under windows avoid including <windows.h> is overrated. 
    // Sure, it can be avoided and "name space pollution" can be
    // avoided, but why? It really doesn't make that much difference
    // these days.
    #define  WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #ifndef __gl_h_
        #include <GL/gl.h>
        #include <GL/glu.h>
    #endif
#else
    // Non windows platforms - don't require nonsense as seen above :-)    
    #ifndef __gl_h_
       #if defined(__APPLE__)
         #include <OpenGL/gl.h>
         #include <OpenGL/glu.h>
       #else
         #include <GL/gl.h>
         #include <GL/glu.h>
       #endif
    #endif
#endif
#include "tulip/GlComplexPolygon.h"
#include "tulip/GlTools.h"
#include "tulip/GlLayer.h"
#include "tulip/GlTextureManager.h"
#include "tulip/Bezier.h"

#include <tulip/TlpTools.h>

#ifndef CALLBACK
#define CALLBACK
#endif

#ifdef __APPLE_CC__
    typedef GLvoid (*GLUTesselatorFunction)(...);
#elif defined( __mips ) || defined( __linux__ ) || defined( __FreeBSD_kernel__) || defined( __FreeBSD__ ) || defined( __OpenBSD__ ) || defined( __sun ) || defined (__CYGWIN__)
    typedef GLvoid (*GLUTesselatorFunction)();
#elif defined ( WIN32)
    typedef GLvoid (CALLBACK *GLUTesselatorFunction)();
#else
    #error "Error - need to define type GLUTesselatorFunction for this platform/compiler"
#endif


using namespace std;

namespace tlp {

  void beginCallback(GLenum which)
  {
    glBegin(which);
  }

  void errorCallback(GLenum errorCode)
  {
    const GLubyte *estring;

    estring = gluErrorString(errorCode);
    cout << "Tessellation Error: " << estring << endl;
  }

  void endCallback(void)
  {
    glEnd();
  }

  void vertexCallback(GLvoid *vertex)
  {
    const GLdouble *pointer;

    pointer = (GLdouble *) vertex;
    Color color=Color(pointer[3],pointer[4],pointer[5],pointer[6]);
    setMaterial(color);
    glColor4ubv((unsigned char *)&color);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(pointer[0]/10., pointer[1]/10.);
    glVertex3dv((GLdouble *)vertex);
  }

  GlComplexPolygon::GlComplexPolygon(vector<Coord> &coords,Color fcolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(false),
    fillColor(fcolor),
    textureName(textureName){
    createPolygon(coords,bezier);
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<Coord> &coords,Color fcolor,Color ocolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(true),
    fillColor(fcolor),
    outlineColor(ocolor),
    textureName(textureName) {
    createPolygon(coords,bezier);
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<vector<Coord> >&coords,Color fcolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(false),
    fillColor(fcolor),
    textureName(textureName){
    for(int i=0;i<coords.size();++i) {
      createPolygon(coords[i],bezier);
      beginNewHole();
    }
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<vector<Coord> >&coords,Color fcolor,Color ocolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(true),
    fillColor(fcolor),
    outlineColor(ocolor),
    textureName(textureName) {
    for(int i=0;i<coords.size();++i) {
      createPolygon(coords[i],bezier);
      beginNewHole();
    }
  }
  //=====================================================
  GlComplexPolygon::~GlComplexPolygon() {
  }
  //=====================================================
  void GlComplexPolygon::createPolygon(vector<Coord> &coords,int bezier) {
    points.push_back(vector<Coord>());
    if(bezier==0) {
      for(vector<Coord>::iterator it=coords.begin();it!=coords.end();++it) {
	addPoint(*it);
      }
    }else{
      double dCoords[coords.size()][3];
      int i=0;
      for(vector<Coord>::iterator it=coords.begin();it!=coords.end();++it) {
	    dCoords[i][0]=(*it)[0];dCoords[i][1]=(*it)[1];dCoords[i][2]=(*it)[2];
	    i++;
      }
      addPoint(coords[0]);

      double result[3];
      double dec=1./(bezier*coords.size());
      for(i=0;i+3<coords.size();i+=3) {
        for(double j=dec;j<1;j+=dec) {
	      Bezier(result,&(dCoords[i]),4,j);
	      addPoint(Coord(result[0],result[1],result[2]));
        }
      }
      addPoint(coords[coords.size()-1]);
    }
  }
  //=====================================================
  void GlComplexPolygon::setOutlineMode(const bool outlined) {
    this->outlined = outlined;
  }
  //=====================================================
  void GlComplexPolygon::addPoint(const Coord& point) {
    points[currentVector].push_back(point);
    boundingBox.check(point);
  }
  //=====================================================
  void GlComplexPolygon::beginNewHole() {
    currentVector++;
  }
  //=====================================================
  void GlComplexPolygon::draw(float lod,Camera *camera) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);

    if(GlTextureManager::getInst().activateTexture(textureName));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    GLUtesselator *tobj;
    tobj = gluNewTess();

    gluTessCallback(tobj, GLU_TESS_VERTEX,
		    (GLUTesselatorFunction) vertexCallback);
    gluTessCallback(tobj, GLU_TESS_BEGIN,
		    (GLUTesselatorFunction)beginCallback);
    gluTessCallback(tobj, GLU_TESS_END,
		    (GLUTesselatorFunction)endCallback);
    gluTessCallback(tobj, GLU_TESS_ERROR,
		    (GLUTesselatorFunction)errorCallback);

    glShadeModel(GL_SMOOTH);

    gluTessBeginPolygon(tobj, NULL);
    for(int v=0;v<points.size();++v) {
      gluTessBeginContour(tobj);
      for(unsigned int i=0; i < points[v].size(); ++i) {
	GLdouble *tmp=new GLdouble[7];
	tmp[0]=points[v][i][0];
	tmp[1]=points[v][i][1];
	tmp[2]=points[v][i][2];
	tmp[3]=fillColor[0];
	tmp[4]=fillColor[1];
	tmp[5]=fillColor[2];
	tmp[6]=fillColor[3];
	gluTessVertex(tobj,tmp,tmp);
      }
      gluTessEndContour(tobj);
    }
    gluTessEndPolygon(tobj);
    gluDeleteTess(tobj);
    GlTextureManager::getInst().desactivateTexture();

    if (outlined) {
      for(int v=0;v<points.size();++v) {
	glBegin(GL_LINE_LOOP);
	for(unsigned int i=0; i < points[v].size(); ++i) {
	  setMaterial(outlineColor);
	  glColor4ubv((unsigned char *)&outlineColor);
	  glVertex3fv((float *)&points[v][i]);
	}
	glEnd();
      }
    }

    glTest(__PRETTY_FUNCTION__);
  }
  //===========================================================
  void GlComplexPolygon::translate(const Coord& mouvement) {
    boundingBox.first+=mouvement;
    boundingBox.second+=mouvement;

    for(vector<vector<Coord> >::iterator it=points.begin();it!=points.end();++it){
      for(vector<Coord>::iterator it2=(*it).begin();it2!=(*it).end();++it2) {
	(*it2)+=mouvement;
      }
    }
  }
  //===========================================================
  void GlComplexPolygon::getXML(xmlNodePtr rootNode) {

    GlXMLTools::createProperty(rootNode, "type", "GlComplexPolygon");

    getXMLOnlyData(rootNode);

  }
  //===========================================================
  void GlComplexPolygon::getXMLOnlyData(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    GlXMLTools::getDataNode(rootNode,dataNode);

    GlXMLTools::getXML(dataNode,"numberOfVector",points.size());
    for(int i=0;i<points.size();++i){
      stringstream str;
      str << i ;
      GlXMLTools::getXML(dataNode,"points"+str.str(),points[i]);
    }
    GlXMLTools::getXML(dataNode,"fillColor",fillColor);
    GlXMLTools::getXML(dataNode,"outlineColor",outlineColor);
    GlXMLTools::getXML(dataNode,"outlined",outlined);
    GlXMLTools::getXML(dataNode,"textureName",textureName);
  }
  //============================================================
  void GlComplexPolygon::setWithXML(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    GlXMLTools::getDataNode(rootNode,dataNode);

    // Parse Data
    if(dataNode) {
      int numberOfVector;
      GlXMLTools::setWithXML(dataNode,"numberOfVector",numberOfVector);
      for(int i=0;i<numberOfVector;++i) {
        stringstream str;
        str << i ;
        points.push_back(vector<Coord>());
        GlXMLTools::setWithXML(dataNode,"points"+str.str(),points[i]);
      }

      GlXMLTools::setWithXML(dataNode,"fillColor",fillColor);
      GlXMLTools::setWithXML(dataNode,"outlineColor",outlineColor);
      GlXMLTools::setWithXML(dataNode,"outlined",outlined);
      GlXMLTools::setWithXML(dataNode,"textureName",textureName);

      for(vector<vector<Coord> >::iterator it= points.begin();it!=points.end();++it) {
        for(vector<Coord>::iterator it2=(*it).begin();it2!=(*it).end();++it2) {
          boundingBox.check(*it2);
        }
      }
    }
  }
}
