from __future__ import print_function
import sys
from tulip import *
from tulipgui import *
# pip install tabulate
import tabulate

f = open('tulippluginsdocumentation.rst','w')

print("""
.. |br| raw:: html

   <br />

.. |bstart| raw:: html

   <b>

.. |bend| raw:: html

   </b>

.. |istart| raw:: html

   <i>

.. |iend| raw:: html

   </i>

.. |listart| raw:: html

   <li>

.. |liend| raw:: html

   </li>

.. |ulstart| raw:: html

   <ul>

.. |ulend| raw:: html

   </ul>

""", file=f)

print('.. py:currentmodule:: tulip\n', file=f)

print('.. _tulippluginsdoc:\n', file=f)

def writeSection(title, sectionChar):
	print(title, file=f)
	underline = ''
	for i in range(len(title)):
		underline += sectionChar
	print(underline+'\n', file=f)

def formatSphinxDoc(doc):
	doc = doc.replace('<br/>', ' |br| ').replace('<br>', ' |br| ')
	doc = doc.replace('<BR>' , ' |br| ').replace('</BR>' , ' |br| ')
	doc = doc.replace('<b>', ' |bstart| ').replace('</b>', ' |bend| ')
	doc = doc.replace('<i>', ' |istart| ').replace('</i>', ' |iend| ')
	doc = doc.replace('<li>', ' |listart| ').replace('</li>', ' |liend| ')
	doc = doc.replace('<ul>', ' |ulstart| ').replace('</ul>', ' |ulend| ')
	return doc
	

def getTulipPythonType(tulipType):
	if tulipType == 'BooleanProperty':
		return ':class:`tlp.BooleanProperty`'
	elif tulipType == 'ColorProperty':
		return ':class:`tlp.ColorProperty`'	
	elif tulipType == 'DoubleProperty':
		return ':class:`tlp.DoubleProperty`'
	elif tulipType == 'IntegerProperty':
		return ':class:`tlp.IntegerProperty`'
	elif tulipType == 'LayoutProperty':
		return ':class:`tlp.LayoutProperty`'	
	elif tulipType == 'SizeProperty':
		return ':class:`tlp.SizeProperty`'	
	elif tulipType == 'StringProperty':
		return ':class:`tlp.StringProperty`'
	elif tulipType == 'NumericProperty':
		return ':class:`tlp.NumericProperty`'	
	elif tulipType == 'PropertyInterface':
		return ':class:`tlp.PropertyInterface`'
	elif tulipType == 'StringCollection':
		return ':class:`tlp.StringCollection`'
	elif tulipType == 'ColorScale':
		return ':class:`tlp.ColorScale`'
	elif tulipType == 'Color':
		return ':class:`tlp.Color`'		
	else:
		return tulipType

writeSection('Tulip plugins documentation', '=')

print("""
In this section, you can find some documentation regarding the C++ algorithm plugins bundled in the Tulip 
software but also with the Tulip Python modules installable through the pip tool.
In particular, an exhaustive description of the input and output parameters for each plugin is given.
To learn how to call all these algorithms in Python, you can refer to the :ref:`Applying an algorithm on a graph <applyGraphAlgorithm>` section.
The plugins documentation is ordered according to their type.

.. warning:: If you use the Tulip Python bindings trough the classical Python interpreter, some plugins
             (Color Mapping, Convolution Clustering, File System Directory, GEXF, SVG Export, Website)
             require the :mod:`tulipgui` module to be imported before they can be called as they use Qt under the hood.
""" + '\n', file=f)

pluginsNames = tlp.PluginLister.availablePlugins()
plugins = {}	
for pluginName in pluginsNames:
	plugin = tlp.PluginLister.pluginInformation(pluginName)
	if not plugin.category() in plugins:
		plugins[plugin.category()] = []
	plugins[plugin.category()].append(plugin)
for cat in sorted(plugins.keys()):
	if cat == 'Perspective' or cat == 'Panel' or cat == 'Node shape' or cat == 'Edge extremity' or cat == 'Interactor':
		continue
	writeSection(cat, '-')
	if cat == 'Algorithm':
		print('.. _algorithmpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyAlgorithm` method. See also :ref:`Calling a general algorithm on a graph <callGeneralAlgorithm>` for more details.\n', file=f)		
	elif cat == 'Coloring':
		print('.. _colorpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyColorAlgorithm` method. See also for more details.\n', file=f)		
	elif cat == 'Export':
		print('.. _exportpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :func:`tlp.exportGraph` function.\n', file=f)			
	elif cat == 'Import':
		print('.. _importpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :func:`tlp.importGraph` function.\n', file=f)		
	elif cat == 'Labeling':
		print('.. _stringpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyStringAlgorithm` method. See also :ref:`Calling a property algorithm on a graph <callPropertyAlgorithm>` for more details.\n', file=f)		
	elif cat == 'Layout':
		print('.. _layoutpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyLayoutAlgorithm` method. See also :ref:`Calling a property algorithm on a graph <callPropertyAlgorithm>` for more details.\n', file=f)		
	elif cat == 'Measure':
		print('.. _doublepluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyDoubleAlgorithm` method. See also :ref:`Calling a property algorithm on a graph <callPropertyAlgorithm>` for more details.\n', file=f)		
	elif cat == 'Resizing':
		print('.. _sizepluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applySizeAlgorithm` method. See also :ref:`Calling a property algorithm on a graph <callPropertyAlgorithm>` for more details.\n', file=f)			
	elif cat == 'Selection':
		print('.. _booleanpluginsdoc:\n', file=f)
		print('To call these plugins, you must use the :meth:`tlp.Graph.applyBooleanAlgorithm` method. See also :ref:`Calling a property algorithm on a graph <callPropertyAlgorithm>` for more details.\n', file=f)			
	
	for p in sorted(plugins[cat], key=lambda p : p.name()):
		writeSection(p.name(), '^')
		writeSection('Description', '"')
		infos = formatSphinxDoc(p.info())
		print(infos+'\n', file=f)
		
		params = tlp.PluginLister.getPluginParameters(p.name())
		headers = ["name", "type", "default", "direction", "description"]
		paramsTable = []
		for param in params.getParameters():
			paramHelpHtml = param.getHelp()
			pattern = '<p class="help">'
			pos = paramHelpHtml.find(pattern)
			paramHelp = ''
			if pos != -1:
				pos2 = paramHelpHtml.rfind('</p>')
				paramHelp = paramHelpHtml[pos+len(pattern):pos2]
				paramHelp = formatSphinxDoc(paramHelp)
			if sys.version_info[0] == 2:
				paramHelp = paramHelp.decode('utf-8')
			pattern = '<b>type</b><td class="b">'
			pos = paramHelpHtml.find(pattern)
			paramType = ''
			if pos != -1:
				pos2 = paramHelpHtml.find('</td>', pos)
				paramType = paramHelpHtml[pos+len(pattern):pos2].replace(' (double precision)', '')
			if param.getName() == 'result' and 'Property' in paramType:
				continue
			paramDir = 'input / output'
			if param.getDirection() == tlp.IN_PARAM:
				paramDir = 'input'
			elif param.getDirection() == tlp.OUT_PARAM:
				paramDir = 'output'
			pattern = '<b>values</b><td class="b">'
			pos = paramHelpHtml.find(pattern)
			paramValues = ''
			if pos != -1:
				pos2 = paramHelpHtml.find('</td>', pos)
				paramValues = paramHelpHtml[pos+len(pattern):pos2]
				paramValues = formatSphinxDoc(paramValues)
			paramDefValue = param.getDefaultValue()
			if paramType == 'StringCollection':
				paramDefValue = paramDefValue[0:paramDefValue.find(';')]
			else:
				paramDefValue = paramDefValue.replace('false', ':const:`False`')
				paramDefValue = paramDefValue.replace('true', ':const:`True`')
			if len(paramValues) > 0:
				paramDefValue += u' |br| |br| Values\xa0for\xa0that\xa0parameter: |br| ' + paramValues
			valuesDoc = False
			nonBreakingSpace = u'\xa0'
			paramName = param.getName().replace('file::', '').replace('dir::', '')
			paramName = paramName.replace(' ', nonBreakingSpace)
			paramType = getTulipPythonType(paramType.replace(' ', nonBreakingSpace))
			paramDir = paramDir.replace(' ', nonBreakingSpace)
			if sys.version_info[0] == 2:
				paramName = paramName.decode('utf-8')
				paramType = paramType.decode('utf-8')
				paramDefValue = paramDefValue.decode('utf-8')
				paramDir = paramDir.decode('utf-8')
				paramHelp = paramHelp.decode('utf-8')
			paramsTable.append([paramName, paramType, paramDefValue, paramDir, paramHelp])
		if len(paramsTable) > 0:
			writeSection('Parameters', '"')
			print(tabulate.tabulate(paramsTable, headers, tablefmt="grid")+'\n', file=f)


f.close()