#!/bin/ksh

FILES=""
MLEVEL="0"
LMODE="async"

function usage {
    print "usage: lneato [-V] [-lm (sync|async)] [-el (0|1)] <filename>"
}

function processoptions {
    while [[ $# > 0 ]] do
        case $1 in
        -V)
            print "lneato version 96b (06-24-96)"
            shift
            ;;
        -lm)
            shift
            LMODE=$1
            if [[ $LMODE != 'sync' && $LMODE != 'async' ]] then
                usage
                exit 1
            fi
            shift
            ;;
        -el)
            shift
            MLEVEL=$1
            if [[ $MLEVEL != '0' && $MLEVEL != '1' ]] then
                usage
                exit 1
            fi
            shift
            ;;
        -)
            FILES="$(print $FILES "'"$1"'")"
            shift
            ;;
        -*)
            usage
            exit 1
            ;;
        *)
            FILES="$(print $FILES "'"$1"'")"
            shift
            ;;
        esac
    done
}

if [[ $DOTTYOPTIONS != '' ]] then
    processoptions $DOTTYOPTIONS
fi
processoptions "$@"

if [[ $DOTTYPATH != '' ]] then
    LEFTYPATH="$DOTTYPATH:$LEFTYPATH"
fi

CMDS=""

CMDS="dotty.protogt.layoutmode = '$LMODE';"

CMDS="$(print $CMDS dotty.mlevel = $MLEVEL";")"

if [[ $FILES == '' ]] then
    FILES=null
fi
FUNC="dotty.createviewandgraph"
for i in $FILES; do
    CMDS="$(print $CMDS $FUNC \($i, "'"file"'", null, null\)";")"
done

lefty -e "
load ('dotty.lefty');
dotty.protogt.lserver = 'neato';
dotty.protogt.graph.type = 'graph';
dotty.init ();
monitorfile = dotty.monitorfile;
$CMDS
txtview ('off');
"
