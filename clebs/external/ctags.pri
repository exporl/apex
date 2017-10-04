clebsCheck(ctags) {
    win32 {
    }

    unix {
        system(which ctags 2>&1 > /dev/null):CLEBS_DEPENDENCIES *= ctags
    }
}

clebsDependency(ctags) {
    win32 {
    }

    unix {
        namespaces = cmn|coh|rba|apex
        outputfile = $${RELBASEDIR}/tags
        ctags.input = CTAGSSRCDIRS
        ctags.output = TAGS
        ctags.depend_command = echo
        ctags.commands  = echo !_TAG_FILE_FORMAT 2 dummy > $${outputfile};
        ctags.commands += echo !_TAG_FILE_SORTED 1 dummy >> $${outputfile};
        ctags.commands += sed -i \'s/ /\\t/g\' $${outputfile};
        ctags.commands += ctags -R --c++-kinds=+p-n --fields=+iaS --extra=+fq --exclude=.build --tag-relative=yes -f $${outputfile}.tmp ${QMAKE_FILE_IN};
        ctags.commands += cat $${outputfile}.tmp | sed -r "'s/(dummy|$${namespaces}):://g;s/\\tnamespace:(dummy|$${namespaces})//g'" | LC_ALL=C.UTF-8 sort >> $${outputfile};
        ctags.commands += rm -f $${outputfile}.tmp
        ctags.CONFIG += combine no_link ignore_no_exist explicit_dependencies
        QMAKE_EXTRA_COMPILERS *= ctags
        ALL_DEPS *= TAGS

        outputfile = $${RELBASEDIR}/tags-qt5
        ctags-qt5.commands = ctags -R --c++-kinds=+p-n --fields=+iaS --extra=+fq -f $${outputfile} /usr/include/*/qt5 /usr/include/qwt
        ctags-qt5.target = ctags-qt5
        QMAKE_EXTRA_TARGETS *= ctags-qt5
    }
}
