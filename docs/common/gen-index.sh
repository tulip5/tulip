#!/bin/sh
REP_COMMON="../common"
THIS_YEAR=$(date -d now  +%Y)
HEAD_FILE=tulipdoc.html
echo "<HTML><HEAD><LINK HREF=\"../common/tulip-doxygen.css\" REL=\"stylesheet\" TYPE=\"text/css\">" > $HEAD_FILE
echo "<LINK href=\"../../common/favicon.ico\" rel=\"SHORTCUT ICON\"><TITLE>Tulip Software Documentation</TITLE>" >> $HEAD_FILE 
echo "<META NAME=\"Keywords\" CONTENT=\"tulip,graph,drawing,information,visualization,tree,dag,opengl,3D,clustering,software,navigation,gpl,free,library\">" >> $HEAD_FILE
echo "<META NAME=\"title\" content=\"Tulip Software home page\"><META NAME=\"subject\" content=\"Tulip Software Documentation\">" >> $HEAD_FILE
echo "<META NAME=\"copyright\" content=\"$THIS_YEAR by LaBRI\"><META NAME=\"author\" content=\"Auber David, Generated by Doxygen\">" >> $HEAD_FILE
echo "</HEAD><BODY><div align=\"center\">" >> $HEAD_FILE
echo "<table cellpadding=\"0\" cellspacing=\"0\" width=\"100%\" style=\"text-decoration: none; border : solid 1px #597ba8 ; border-right-style: none; border-top-style: none; border-left-style: none;\" bgcolor=\"E5E5E5\">" >> $HEAD_FILE
echo "<tr><td valign=center>" >> $HEAD_FILE
for A in `ls */index.html`
do
    B=`dirname $A`
    B=`basename $B`
    echo "<A href=\"$A\" target=\"doc\" style=\"text-decoration: none; font-family : sans-serif; color: #000000; font-size: 12pt\">" >> $HEAD_FILE
    echo "$B</A> <font color=\"#41597A\">&nbsp;&nbsp;&nbsp;</font>" >> $HEAD_FILE
done
echo "<A href=\"allPlugins.html\" target=\"doc\" style=\"text-decoration: none; font-family : sans-serif; color: #000000; font-size: 12pt\">" >> $HEAD_FILE
echo "tulip-plugins</A> <font color=\"#41597A\">&nbsp;&nbsp;&nbsp;</font>" >> $HEAD_FILE
echo "</td>" >> $HEAD_FILE
echo "<td align=\"right\" valign=\"center\"><img src=\"$REP_COMMON/logo.png\" align=\"right\" width=\"32\" height=\"32\" border=\"0\"></td></tr></table>" >> $HEAD_FILE

echo "</div></body></html>" >> $HEAD_FILE
