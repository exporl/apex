#!/bin/bash -e

[ -d '.git' ] || cd `dirname $0`/..

if git grep -n '^[^#].*\(qDebug\|qWarning\|qCritical\)' src; then
    echo "FAIL: Please replace qDebug/qWarning/qCritical by qCDebug/qCWarning/qCCritical"
    exit 1
fi
if git grep -I $(printf "\r"); then
    echo "FAIL: Please replace MSDOS CRLF line endings by standard LF"
    exit 1
fi
if git grep '<assert\.h>' src; then
    echo "FAIL: Please replace assert by Q_ASSERT"
    exit 1
fi
if git ls-files | grep ' '; then
    echo "FAIL: Please replace spaces in filenames with dashes"
    exit 1
fi
if git grep -I "$(printf "\t")" $(git ls-files src tools examples | grep -v '\.gitmodules\|tools/debian/rules'); then
    echo "FAIL: Please replace tab characters in src, tools and examples by spaces"
    exit 1
fi
if git grep -I ' \+$' src tools; then
    echo "FAIL: Please remove trailing whitespace before committing"
    exit 1
fi
