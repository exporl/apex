#!/bin/sh

p=/usr/local/oxygen
GENERATE_CHUNKS=true
USE_HASH_CODES=false
HIDE_COMMENTS=false
HIDE_ANNOTATIONS=true
DIAGRAMS_EXTENSION=.png


IMAGES_FOLDER="images"
MAIN_OUTPUT_DIR="./generated"
       HTML_FILE_NAME="experiment.html"
       DIAGRAMS_EXTENSION=".png"
       USE_HASH_CODES="false"
       GENERATE_CHUNKS="true"
       HIDE_COMMENTS="false"
       HIDE_ANNOTATIONS="true"
#XSD_PATH=/apex-schema.xsd"

SCHEMA_PATH="../../data/schemas"

for name in $SCHEMA_PATH/*.xsd; do

if [ "$name" = "$SCHEMA_PATH/apexconfig.xsd" ];
then
    echo "Skipping mainconfig"
    continue
fi

echo "Processing $name"

XSD_PATH=$name

java -cp "$p/lib/saxon8.jar" net.sf.saxon.Transform -o "$MAIN_OUTPUT_DIR/$HTML_FILE_NAME" "$XSD_PATH" "xs3p.xsl"\
 showDiagrams=false\
 imagesBaseURL="$IMAGES_FOLDER"\
 sortByComponent=true\
 useJavaScript=false\
 printAllSuperTypes=false\
 printAllSubTypes=false\
 printLegend=false\
 printGlossary=false\
 printNSPrefixes=false\
 mainSchemaOutputFile="$HTML_FILE_NAME"\
 showLogicalDiagrams=false\
 mainDocBase="$MAIN_OUTPUT_DIR"\
 mainDocFile="$HTML_FILE_NAME"\
 diagramImagesExtension=$DIAGRAMS_EXTENSION\
 useHashcodes=$USE_HASH_CODES\
 generateChunks=$GENERATE_CHUNKS\
 searchIncludedSchemas=true\
 linksFile="links.xml"
# externalCSSURL="$CSS_URL"

done
