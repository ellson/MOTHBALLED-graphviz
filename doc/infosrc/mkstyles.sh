#! /bin/ksh
#
# usage: mkstyles.sh <styles>
#
# where gradient has the form s_<o>_<style>.fmt
# where .fmt gives the desired output format.

if (( $# == 0 ))
then
  echo "mkstyles: missing style name argument"
  exit 1
fi

TGT=$1               # s_o_style.fmt
BASE=${1%.*}         # s_o_style
FMT=${1#$BASE.}      # fmt
F=$BASE.gv           # s_o_style.gv
OBJ=${BASE%_*}       # s_e,s_n or s_c
STYLE=${BASE#${OBJ}_}  # style

case "$STYLE" in
 filled* )
  COLOR="fillcolor=lightblue"
  ;;
 striped* )
  COLOR="fillcolor=\"blue:red;.3:yellow:green;.2\""
  SHAPE="shape=box"
  ;;
 rounded*  | diagon* )
  SHAPE="shape=box"
  ;;
 wedged* )
  COLOR="fillcolor=\"blue:red;.3:yellow:green;.2\""
  ;;
esac

exec 3> $F

echo "graph G {" >&3

case "$OBJ" in
 s_n* )
    echo "  A[$SHAPE style=$STYLE $COLOR label=\"\"]" >&3
  ;;
 s_e* )
    echo "  rankdir=LR" >&3
    echo "  node[shape=point]" >&3
    echo "  edge[dir=forward arrowhead=none style=$STYLE]" >&3
    echo "  a -- b " >&3
  ;;
 s_c* )
    echo "  node[style=filled label=\"\"]" >&3
    echo "  subgraph cluster0 {" >&3
    echo "    $COLOR" >&3
    echo "    style=$STYLE" >&3
    echo "    H " >&3
    echo "  }" >&3
  ;;
esac
echo "}" >&3

exec 3>&-

    dot -T$FMT $F > $TGT
    rm -f $F


