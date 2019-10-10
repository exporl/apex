#!/bin/bash -e

# A lot of the submodule logic can go away if the support for recursive
# ls-files (git 2.11) and grep (git 2.12) is broadly available

ORIGINALPWD=$(pwd)

cd "$(dirname ${BASH_SOURCE[0]})/../.."

ROOT=$(pwd)
SUBMODULE=
EXCLUDES=documentation
CHECKS=
CONTINUE=false
RECURSING=false
FIX=false
FAILED=false
FIXED=false

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --submodule) # [SUBMODULE]
            # only check the specified submodule
            SUBMODULE=$2
            shift
            ;;
        --exclude) # [SUBMODULES]
            # exclude the specified submodules
            # pass as comma-separated list "module1, module2"
            IFS=", " read -r -a EXCLUDES <<< "$2"
            shift
            ;;
        --checks) # [qdebug|crlf|assert|filenamespaces|tabs|trailingspaces|clangformat|eslint]
            # type of checks to run
            CHECKS=$2
            shift
            ;;
        --continue) #
            # do not stop after the first error
            CONTINUE=true
            ;;
        --fix) #
            # attempt to fix any detected problems
            FIX=true
            ;;
        --recursing)
            SUBMODULE=$ORIGINALPWD
            RECURSING=true
            ;;
        -h|--help) #
            # this help
            echo "Usage: $0 [OPTION]..."
            printf "\nCommand line arguments:\n"
            sed -rn '/CMDSTART/,/CMDEND/{/\) \#|^ +# /{s/\)? #//g;s/^    //;p}}' "$0"
            exit 0
            ;;
        *)
            echo "Unknown parameter $1"
            exit 1
            ;;
        esac
        #CMDEND
        shift
    done
}

createtemp() {
    CANDIDATEFILE="$(mktemp)"
    FOUNDFILE="$(mktemp)"
    RESULTFILE="$(mktemp)"
}

removetemp() {
    [ -f "$CANDIDATEFILE" ] && rm -f "$CANDIDATEFILE"
    [ -f "$FOUNDFILE" ] && rm -f "$FOUNDFILE"
    [ -f "$RESULTFILE" ] && rm -f "$RESULTFILE"
    return 0
}

exitappropriately() {
    removetemp
    if [ "$RECURSING" = "true" ]; then
        [ "$FAILED" = "true" ] && echo "RECURSING-FAILED-TAG"
        [ "$FIXED" = "true" ] && echo "RECURSING-FIXED-TAG"
    else
        [ "$FAILED" = "true" ] && exit 1
        [ "$FIXED" = "true" ] && exit 1
    fi
    exit 0
}

# in response to a failed check, mark the check result as either fixed or failed
# pass fixable as first parameter if the script knowns how to fix the error automatically
# does not return if this is a fatal error
# returns 0 if the fix should be attempted
# returns 1 if the error is unfixable/should not be fixed but execution should continue
mark() {
    if [ "$FIX" = "true" -a "$1" = "fixable" ]; then
        FIXED=true
        return 0
    else
        FAILED=true
        if [ -n "$FAILMSG" ]; then
            if [ -n "$2" ]; then
                echo "$2" > "$RESULTFILE"
            fi
            (
                IFS=:
                while read FILE LINE TEXT; do
                    echo "${SUBMODULE##*/}${SUBMODULE:+/}$FILE:${LINE:-1}: error: $FAILMSG"
                done < "$RESULTFILE"
            )
        fi
        [ "$CONTINUE" != "true" ] && exitappropriately
        return 1
    fi
}

zero-git-ls() {
    git ls-files -z --full-name "$@"
}

zero-xargs() {
    xargs -0 --no-run-if-empty "$@"
}

zero-grep() {
    grep -z "$@"
}

export -f zero-git-ls zero-xargs zero-grep

ORIGINALARGS=("$@")
parsecmd "$@"

if [ ! -z "$SUBMODULE" ] && [[ "${EXCLUDES[@]}" =~ "$(basename "$SUBMODULE")" ]]; then
    echo "Skipping submodule $(basename "$SUBMODULE")"
    exitappropriately
fi

if [ -z "$CHECKS" ]; then
    CHECKS="qdebug crlf assert filenamespaces tabs trailingspaces clangformat eslint"
fi
if [ "$RECURSING" = "false" ]; then
    echo "Submodule: $SUBMODULE"
    echo "Checks: $CHECKS"
    echo "Continue: $CONTINUE"
    echo "Fix automatically: $FIX"
fi

for CHECK in $CHECKS; do
    if [ -n "$SUBMODULE" ]; then
        pushd "$SUBMODULE" > /dev/null
    fi
    if [ "$RECURSING" = "false" ]; then
        echo "Checking $CHECK..."
    fi
    createtemp
    case "$CHECK" in
    qdebug)
        FAILMSG="please replace qDebug/qWarning/qCritical by qCDebug/qCWarning/qCCritical"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo src || echo .)
        EXPRESSION='^[^#].*\(qDebug\|qWarning\|qCritical\)'
        zero-git-ls $CANDIDATES | zero-grep '\.h$\|\.cpp$' > "$CANDIDATEFILE" || true
        zero-xargs git grep -Ilz "$EXPRESSION" < "$CANDIDATEFILE" > "$FOUNDFILE" || true
        zero-xargs git grep -In "$EXPRESSION" < "$FOUNDFILE" > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark manual \
            || true
        ;;
    crlf)
        FAILMSG="please replace MSDOS CRLF line endings by standard LF"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo . || echo .)
        EXPRESSION="$(printf "\r")"
        zero-git-ls $CANDIDATES > "$CANDIDATEFILE" || true
        zero-xargs git grep -Ilz "$EXPRESSION" < "$CANDIDATEFILE" > "$FOUNDFILE" || true
        zero-xargs git grep -In "$EXPRESSION" < "$FOUNDFILE" > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark fixable \
            && zero-xargs sed -i 's/\r//g' < "$FOUNDFILE" \
            || true
        ;;
    assert)
        FAILMSG="please replace assert by Q_ASSERT"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo . || echo .)
        EXPRESSION='<assert\.h>'
        zero-git-ls $CANDIDATES > "$CANDIDATEFILE" || true
        zero-xargs git grep -Ilz "$EXPRESSION" < "$CANDIDATEFILE" > "$FOUNDFILE" || true
        zero-xargs git grep -In "$EXPRESSION" < "$FOUNDFILE" > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark manual \
            || true
        ;;
    tabs)
        FAILMSG="please replace tab characters by spaces"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo src tools examples || echo .)
        EXPRESSION="$(printf "\t")"
        zero-git-ls $CANDIDATES | zero-grep -v '\.gitmodules\|tools/debian/rules\|external/.*\.js\' > "$CANDIDATEFILE" || true
        zero-xargs git grep -Ilz "$EXPRESSION" < "$CANDIDATEFILE" > "$FOUNDFILE" || true
        zero-xargs git grep -In "$EXPRESSION" < "$FOUNDFILE" > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark manual \
            || true
        ;;
    trailingspaces)
        FAILMSG="please remove trailing whitespace"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo src tools || echo .)
        EXPRESSION=' \+$'
        zero-git-ls $CANDIDATES > "$CANDIDATEFILE" || true
        zero-xargs git grep -Ilz "$EXPRESSION" < "$CANDIDATEFILE" > "$FOUNDFILE" || true
        zero-xargs git grep -In "$EXPRESSION" < "$FOUNDFILE" > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark fixable \
            && zero-xargs sed -i 's/ \+$//g' < "$FOUNDFILE" \
            || true
        ;;
    filenamespaces)
        FAILMSG="please replace spaces in filenames with dashes"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo . || echo .)
        zero-git-ls $CANDIDATES | zero-grep ' ' | tr '\0' '\n' > "$RESULTFILE" || true
        [ -s "$RESULTFILE" ] \
            && mark manual \
            || true
        ;;
    clangformat)
        FAILMSG="please format your sources with clang-format; rerun linux-check.sh with --fix"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo src data || echo .)
        cp "$ROOT/common/tools/clang-format" "$ROOT/.clang-format"
        zero-git-ls $CANDIDATES | zero-grep '\.java$\|\.h$\|\.cpp$' > "$CANDIDATEFILE" || true
        while read -r -d $'\0' i; do
            extension="${i##*.}"
            ! diff -u "$i" <(clang-format-3.9 "$i") > /dev/null \
                && mark fixable "$i" \
                && clang-format-3.9 -i "$i" \
                || true
        done < "$CANDIDATEFILE"
        ;;
    eslint)
        FAILMSG="please fix your javascript files to conform to the eslint:recommended profile"
        CANDIDATES=$([ -z "$SUBMODULE" ] && echo examples data || echo .)
        TAPOUTPUT="$ROOT/eslint-errors${SUBMODULE:+.${SUBMODULE##*/}}.tap"
        # For now, only check changed files
        # zero-git-ls $CANDIDATES \
        # --diff-filter=d => ignore deleted files
        git show -z HEAD --name-only '--pretty=format:' --diff-filter=d \
            | zero-grep '\.js$' > "$CANDIDATEFILE" || true
        cp "$ROOT/common/tools/eslintrc" "$ROOT/.eslintrc"
        echo "ESLint output can be found in $TAPOUTPUT"
        zero-xargs "$ROOT/node_modules/.bin/eslint" --format "tap" < "$CANDIDATEFILE" | tee "$TAPOUTPUT"
        test ${PIPESTATUS[0]} -ne 0 \
            && mark manual \
            || true
        ;;
    *)
        FAILMSG="unknown check: $CHECK"
        if [ -z "$SUBMODULE" ]; then
            echo "${BASH_SOURCE[0]}" > "$RESULTFILE" \
                && mark manual \
                || true
        fi
        ;;
    esac
    removetemp
    if [ -n "$SUBMODULE" ]; then
        popd > /dev/null
    fi

    if [ -z "$SUBMODULE" ]; then
        FAILMSG=
        OUTPUTFILE="$(mktemp)"
        git submodule foreach "$ROOT/common/tools/linux-check.sh" "${ORIGINALARGS[@]}" --recursing --checks "$CHECK" |& tee "$OUTPUTFILE" | grep -v "RECURSING-\(FAILED\|FIXED\)-TAG" || true
        RESULT="$(cat "$OUTPUTFILE")"
        rm -f "$OUTPUTFILE"
        echo "$RESULT" | grep -q "RECURSING-FAILED-TAG" && mark manual || true
        echo "$RESULT" | grep -q "RECURSING-FIXED-TAG" && mark fixable || true
    fi
done

exitappropriately
