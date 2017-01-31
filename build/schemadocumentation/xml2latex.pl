#!/usr/bin/perl

use strict;
use XML::LibXML;
#use IO::Handle;
 use IO::File;
 use LaTeX::Encode;
 
# initialize the parser
my $parser = new XML::LibXML;

my $sourcedir = "generated/";
my $targetdir = "latex/";
 
use vars qw($recursiondepth @expandtypes %noExpands %alreadyExpanded);            # expandtypes: types still to be processed
 

# read list of types not to be expanded in place
open(F, "no-expands.txt");
foreach (<F>) {
    chomp;
    $noExpands{$_}=1;
}
close(F);


push @expandtypes, 'apex';


open(ALL, ">" . "all.tex");

while (@expandtypes) {
    my $type = pop(@expandtypes);
    my $filename='generated/apex-schema.html_element_' . $type . '.html';
    my $targetfile=$targetdir . 'type_' . $type . '.tex';
    
    if (-f $targetfile) {
        print "$targetfile already exists, skipping\n";
        next;
    }

    unless (-f $filename) {
        $filename='generated/apex-schema.html_type_' . $type . '.html';
    }
    unless (-f $filename) {
        print "Cannot open file $filename, skipping";
        next;
    }


    # open a filehandle and parse
    my $fh = new IO::File;
    unless( $fh->open( $filename, "r" )) {
        die("Cannot open file $filename");
    }

    my $doc = $parser->parse_fh( $fh );
    my $de = $doc->getDocumentElement;

    open(FID, ">".$targetfile);
    print "Creating $targetfile\n";
    foreach my $current ( $de->getChildnodes ) {
           #print "Node type: $current->nodeType \n";
           
           if ($current->nodeType eq &XML_ELEMENT_NODE) {
           
               if ($current->nodeName eq "element") {
           
                    (my $a, my $n) = showElement($current,1);
#                    if ($n ne "...") {
#                        print FID "\\section{Element $n}";    
 #                   }
                    print FID $a;
                
                } elsif ($current->nodeName eq "section") {
                    my $name = $current->firstChild->textContent;
                    my $label = getLabelName($name);
                    print FID "\\section{$name}\n";
                    print FID "\\label{$label}\n";
                } else {
                    print "Unknown tag: " . $current->nodeName . "\n";
                }
            }
    }
    close(FID);
    print ALL "\\input{$targetfile}\n";
    
    $fh->close;
}
close(ALL);



sub showElement {
    my $node = $_[0];
    my $showdescription = $_[1];            # add \description around items
    
    if ($node->nodeName ne "element") {
        print "Error: invalid node name\n";
        return;
    }
    
    my $name;
    my $section;
    my $type;
    my $occurence;
    my $doc;
    my @subtypes;
    my $parent;
    my @attributes;
    my @elements;
    my $expanded_type=0;
    
    
    foreach my $current ( $node->getChildnodes ) {
        if ( $current->nodeType ne &XML_ELEMENT_NODE) {
#            print "Skipping non-element";
            next;
        }
    
        my $tag = $current->nodeName;
        next unless ($current->firstChild);
        my $value = $current->firstChild->textContent;
        
                
        if ($tag eq "type") {
            if ($value=~/ref:(.*)/) {
                ($type,$expanded_type) = expandType($1); 
            } else {
                $type = $value;
            }
        } elsif ($tag eq "name") {
            $name = latex_encode($value);
        } elsif ($tag eq "occurence") {
            $occurence = occursToText($value);
        } elsif ($tag eq "documentation") {
            $doc = latex_encode($value);
        } elsif ($tag eq "section") {
            $section = latex_encode($value);
        } elsif ($tag eq "subtype") {
            push(@subtypes, referType(getSubTypeString($current)));
        } elsif ($tag eq "parent") {
            $parent = referType(getSubTypeString($current));
        } elsif ($tag eq "attribute") {
            (my $a, my $n) = showAttribute($current);
            push @attributes, {"name" => $n, "value" => $a};
        } elsif ($tag eq "element") {
            (my $a, my $n) = showElement($current,1);
            push @elements, {"name" => $n, "value" => $a};
        } else {
            print "Unknown node $tag\n";
        }
        
        
    }
    
            
          
          my $result;
    
    if ($section) {
        my $labelname=getLabelName($type);
        $result .= <<"VERDER";
\\section{$section}
\\label{$labelname}

VERDER
    }
       
       
#       \hspace{1cm}\begin{minipage}{\textwidth}
# text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla text bla bla
#\end{minipage}

my $namestring;
if ($name ne "...") {
    $namestring = "\\textbf{\$\<\$$name\$\>\$}";
}

my $typestring;
if (!$expanded_type and $type) {
    $typestring = "\\textbf{type} $type"; 
}
my $docstring;
if ($doc) {
    $docstring="\\item[doc] $doc";
}

$result .= <<"VERDER"; 
\\begin{tabular}{p{4cm}p{2cm}l}
$namestring  & \\emph{$occurence} & $typestring
\\end{tabular}
VERDER


my $dcontents;

if ($parent) {
   $dcontents .= "\\item[parent] $parent\n";
}

foreach my $subtype (@subtypes) {
    $dcontents .= "\\item[subtype] $subtype\n";
}


$dcontents.=$docstring;

foreach my $attrib (@attributes) {
    $dcontents .= <<"VERDER";
    \\item[attrib] $$attrib{value}
VERDER
}

foreach my $element (@elements) {
#[\$<\$$$element{name}\$>\$]
    $dcontents .= <<"VERDER";
    \\item $$element{value}
VERDER
}

if ($expanded_type) {
    $dcontents.=$type;
}


if ($dcontents) {
    if ($showdescription) {
        $result .= "\\begin{description}\n";
     }
     $result.=$dcontents;
     if ($showdescription) {
        $result.="\\end{description}\n";
     }
}
    return ($result, $name);

}


sub showAttribute {
    my $node = $_[0];
    
     my $name;
    my $type;
    my $occurence;
    my $doc;
    
    
    foreach my $current ( $node->getChildnodes ) {
        if ( $current->nodeType ne &XML_ELEMENT_NODE) {
#            print "Skipping non-element";
            next;
        }
    
        my $tag = $current->nodeName;
        my $value = $current->firstChild->textContent;
        
                
        if ($tag eq "type") {
            $type = $value;
        } elsif ($tag eq "name") {
            $name = latex_encode($value);
        } elsif ($tag eq "occurence") {
            $occurence = occursToText($value);
        } elsif ($tag eq "documentation") {
            $doc = latex_encode($value);
        } else {
            print "Unknown node $tag\n";
        }
        
        
    }
    
    
 my $typestring;
if ($type) {
    $typestring = "\\textbf{type} $type"; 
}
    
            my $result = <<"VERDER";
\\begin{tabular}{p{4cm}p{2cm}l}
\\textbf{\$\<\$$name\$\>\$}  & \\emph{$occurence} & $typestring
\\end{tabular}
VERDER

if ($doc) {
$result .= <<"VERDER"

%\\hspace{0.5cm}\\begin{minipage}{\\textwidth}
\\begin{docindent}
$doc
\\end{docindent}
%\\end{minipage}
VERDER
}

    return ($result, $name);

}


sub referType {
    my $name = $_[0];
    my $label=getLabelName($name);
    push @expandtypes, $name;
    return "$name (see p~\\pageref{$label})";   

}


# try to find a file containing a type definition
sub expandType {
    my $name = $_[0];
    
    

    if ($noExpands{$name} || $alreadyExpanded{$name}) {                # write reference
         return (referType($name),0);
    } else {                # expand 
        $alreadyExpanded{$name}=1;
    
        my $filename = "generated/apex-schema.html_type_" . $name . ".html";
        #print $filename;
    
        unless (-e $filename) {
            print "** cannot open file $filename for resolving type $name\n";
            return "";
        }
        
        my $nparser = new XML::LibXML;
     
        # open a filehandle and parse
        my $fh = new IO::File;
        unless( $fh->open( $filename, "r" )) {
            die("Cannot open file $filename");
        }
        
        my $doc = $nparser->parse_fh( $fh );
    
        my $de = $doc->getDocumentElement;
        my @nodes = $de->getChildrenByTagName("element");
        
        my $result;
        
        foreach my $current ( $de->getChildnodes ) {
               #print "Node type: $current->nodeType \n";
               
              if ($current->nodeType eq &XML_ELEMENT_NODE) {
           
               if ($current->nodeName eq "element") {
           
                    (my $a, my $n) = showElement($current,0);
#                    if ($n ne "...") {
#                        print FID "\\section{Element $n}";    
 #                   }
                    $result .= $a;
                
                } elsif ($current->nodeName eq "section") {
                    my $name = $current->firstChild->textContent;
                    my $label = getLabelName($name);
#                    $result .= "\\section{$name}\n";
#                    $result .= "\\label{$label}\n";
                } else {
                    print "Unknown tag: " . $current->nodeName . "\n";
                }
            }
        }
        
        $fh->close;
        return ($result, 1);
        
    } 
    
}


sub getLabelName {
    my $name=$_[0];
    
    return "type:$name";
}

sub occursToText {
    my $s = $_[0];

    my $result;
    if ($s eq "[1]") {
        $result = "required";    
    } elsif ($s eq "[0..1]") {
        $result = "optional";    
    } elsif ($s eq "[1..*]") {
        $result = "one or more";    
    } elsif ($s eq "[0..*]") {
        $result = "any";    
    } else {
        $result = $s;
    }
    
    return $result;
}


sub getSubTypeString {
    my $element = $_[0];
    
    foreach my $current ( $element->getChildnodes ) {
        if ( $current->nodeName eq "type") {
            my $s = $current->firstChild->textContent;
            if ($s=~/ref:(.*)/) {
                return $1;
             } else {
                 return $s;
             }
        }    
    }
    return "";
}

