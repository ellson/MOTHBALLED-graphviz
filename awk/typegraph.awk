# Copyright (c) AT&T Corp. 1994, 1995.
# This code is licensed by AT&T Corp.  For the
# terms and conditions of the license, see
# http://www.research.att.com/orgs/ssr/book/reuse

BEGIN	{
			n_names = 0;
			printf("strict digraph typeref {\n");
			printf("	node [shape=record];\n");
		}

(NF == 1) {		# starts a new node in the node file
			Name[n_names++] = name = $1;
			n_fields = 0;
		}

(NF == 2) {		# enters a field in the node file
			if (($2 == "v") || (PROCS && ($2 == "p"))) {
				Field[name,n_fields] = $2 ": " $1;
				Field[name,$1] = n_fields;
				n_fields++;
			}
		}

(NF == 6) {		# an entry in the edge file. example:
				# v BinaryNode left t - Tree
			kind = $1;
			if ((kind == "v") || (PROCS && (kind == "p"))) {
				if (($2 != "-") && ($2 != $3))
					from = "\"" $2 "\".f0_1_" Field[$2,$3];
				else from = "\"" $3 "\"";

				if (($5 != "-") && ($5 != $6))
					to = "\"" $5 "\".f0_1_" Field[$5,$6];
				else to = "\"" $6 "\"";

				printf("\t%s -> %s;\n",from, to);
			}
		}

END 	{
			for (i in Name) {
				name = Name[i];
				printf("\t\"%s\" [label=\"{%s{",name,name);
				for (j = 0; Field[name,j] != ""; j++) {
					if (j > 0) printf("|");
					printf("%s",Field[name,j]);
				}
				printf("}}\"];\n");
			}
			printf("}\n");
		}
