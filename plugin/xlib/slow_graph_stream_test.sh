#!/bin/bash

# intended to test the behavior of 'dot -Tx11' to a succession of graphs on stdin
# with an interval in between, which should be enough to allow each graph to be seen

# usage:   ./slow_graph_stream_test.sh | dot -Tx11

while true; do
    echo "digraph {rankdir=RL; 0->{1 2}}"
    sleep 2
    echo "digraph {rankdir=LR; 0->{1 2}}"
    sleep 2
done
