#!/bin/bash

RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
TEAL=$(tput setaf 6)
NORMAL=$(tput sgr0)
BOLD=$(tput bold)
MODEST=${YELLOW}${BOLD}


# $1:=environment-variable
function validateEnvironmentVariable {
    if env | grep -q ^$1= ; then
        writeStatus "  - Environment variable $1 found" 0
        return 0
    else
        writeStatus "  - Environment variable $1 not set" 1
        return 1
    fi
}
function writeStatus {
    if [[ $2 -eq 0 ]] ; then
        color=$GREEN
    elif [[ $2 -eq 1 ]] ; then
        color=$YELLOW
    else
        color=$RED
    fi
    let COL=$(tput cols)-${#1}+${#color}+${#NORMAL}

    if [[ $2 -eq 0 ]] && [[ ! $isQuiet ]] ; then
        printf "%s%${COL}s" "$1" "$color[OK]$NORMAL        "
    elif [[ $2 -eq 1 ]] && [[ ! $isQuiet ]] ; then
        printf "%s%${COL}s" "$1" "$color[WARNING]$NORMAL   "
    elif [[ $2 -eq 2 ]] ; then
        printf "%s%${COL}s" "$1" "$color[FAILED]$NORMAL    "
    fi
}


# $1: Command to execute
function interactiveOptional {
    if [[ $disableInteractive ]] ; then return ; fi

    while true; do
        read -p "    Do this now? [y/n] " yn
        case $yn in
            [Yy]* )
                eval "$1";
                return 0
                ;;
            [Nn]* )
                return 1;;
            * ) echo "Please answer yes or no.";;
        esac
    done
}


# $1=text
function message {
    if [[ ! $isQuiet ]] ; then
        echo -e "$1"
    fi
}


# $1=name (e.g. boost) $2=searchFile  $3=requiredMatch [$4=specialExecutableArguments]
# If $4 is set, it assumes $3 in an executable finds paths with  `$2 $4`
# Both path/$2 and path/$3 must exist for the path to be valid.
# Value stored in $retvalue, if it finds a match.
function locateAny {
    message "  - Attempting to automatically find $1 (may take some time)"
    retvalue=
    searchFile=$2
    requiredMatch=$3
    possible=`locate "$searchFile" | grep ${searchFile}$ |  sed 's/.\{'${#searchFile}'\}$//'`
    if [[ $possible ]] ; then
        sorted=`echo "$possible" | awk '{ print length($0) " " $0; }' | sort -n | cut -d ' ' -f 2-`
        for i in $sorted ; do
            if [[ $# -eq 4 ]] ; then
                iexec=$i/$searchFile
                if [[ -x $iexec ]] && [[ -e `$iexec $4`/${requiredMatch} ]] ; then
                    retvalue=`$iexec $4`
                    return 0
                fi
            else
                if [[ -e ${i}/${requiredMatch} ]] ; then
                    retvalue=$i
                    return 0
                fi
            fi
        done
    fi
    return 1
}


accumVarDefines=
# $1=name (e.g. boost) $2=variableNameSet [$3=value, ${!TMP{2}} if not set]
function setEnvVariable {
    newPath=$3
    if [[ $# -eq 2 ]] ; then
        tmpprefix=TMP${2}
        newPath=${!tmpprefix}
    fi

    # echo ${!tmp}
    message "  - $1 found at ${BLUE}${newPath}${NORMAL}"
    message "  - Setting environment variable: $2=${newPath}"
    export ${2}=${newPath}
    accumVarDefines=${accumVarDefines}"\nexport "${2}=${newPath}
}



function findBoost {
    message "${GREEN}[boost library]${NORMAL}"
    TMPBOOST_DIR=
    if ! validateEnvironmentVariable BOOST_DIR ; then
        if validateEnvironmentVariable BOOST_ROOT ; then
            TMPBOOST_DIR=$BOOST_ROOT
        elif validateEnvironmentVariable BOOSTROOT ; then
            TMPBOOST_DIR=$BOOSTROOT
        else
            if locateAny "boost" \
                "/include/boost/algorithm/string.hpp" \
                "/include/boost/version.hpp" ; then
                TMPBOOST_DIR=$retvalue
            fi
        fi
        if [[ $TMPBOOST_DIR ]] ; then
            setEnvVariable "Boost library" "BOOST_DIR"
        else
            writeStatus "  - Failed to find boost library" 2
            echo " ${YELLOW}consider:${NORMAL} sudo apt-get install "\
"libboost-all-dev && sudo updatedb"
            if interactiveOptional "sudo apt-get install libboost-all-dev" ; then
                echo "Done. Make sure to re-run script for detection."
            else
                hasFailed=true
            fi

            return
        fi
    fi

    # Test the actual content of the boost directory
    pathStatusCode=0
    [[ ! -e $BOOST_DIR/include/boost/version.hpp ]] && pathStatusCode=2
    writeStatus "  - Checking boost library ($BOOST_DIR)" $pathStatusCode
    [[ $pathStatusCode -eq 2 ]] && hasFailed=true
}



findBoost


if [[ $accumVarDefines ]] ; then
    message "${YELLOW}TIP:${NORMAL}
To speed up this script, (or better, make it unneccesary), you can add the
following lines to ${BLUE}~/.bashrc${NORMAL} (or similar), and then restart the terminal:"
    message "${accumVarDefines}\n\n"
fi

