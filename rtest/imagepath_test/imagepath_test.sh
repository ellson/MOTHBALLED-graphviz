#!/home/dperry/work_related/packages/arch/linux.i386-64/src/cmd/ksh93/ksh
#! /bin/ksh

let count=1
while [ $count -lt `wc -l test_cases` ]
do
cat  test_cases | sed -ne ${count}p | ./gen_graph.sh
let count++
done

