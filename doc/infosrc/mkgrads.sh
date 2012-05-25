#! /bin/ksh
#
# usage: mkgrads.sh <gradient>
#
# where gradient has the form g_lin<angle>.fmt or g_rad<angle>.fmt
# where .fmt gives the desired output format.

if (( $# == 0 ))
then
  echo "mkgrads: missing gradient name argument"
  exit 1
fi

TGT=$1               # g_type.fmt
GRAD=${1%.*}         # g_type
FMT=${1#$GRAD.}      # fmt
F=$GRAD.dot          # g_type.dot

case "$GRAD" in
 g_lin* )
  ANG=${GRAD#g_lin}
  STYLE=
  ;;
 g_rad* )
  ANG=${GRAD#g_rad}
  STYLE=-Nstyle=radial
  ;;
esac

    exec 3> $F

    echo "digraph G {" >&3
    echo "  node[style=filled fillcolor=\"yellow:blue\" shape=box label=\"\"]" >&3
    echo "  H " >&3
    echo "}" >&3

    exec 3>&-

    dot -T$FMT $STYLE -Ngradientangle=$ANG $F > $TGT
    rm -f $F


