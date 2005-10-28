array set LANGS {
	csharp {
		TYPES {
			Agraph_t* SWIGTYPE_p_Agraph_t
                	Agnode_t* SWIGTYPE_p_Agnode_t
                	Agedge_t* SWIGTYPE_p_Agedge_t
                	Agsym_t* SWIGTYPE_p_Agsym_t
                	char* string
                	FILE* SWIGTYPE_p_FILE
                	bool bool
                	void** SWIGTYPE_p_p_void
                	void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
		}
		USAGE {
		}
	}
	guile {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
                	void** data_handle
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
			{(load-extension "./libgv.so" "SWIG_init")}
		}
		USAGE {
		}
	}
	io {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
		}
		USAGE {
		}
	}
	java {
		TYPES {
			Agraph_t* SWIGTYPE_p_Agraph_t
			Agnode_t* SWIGTYPE_p_Agnode_t
			Agedge_t* SWIGTYPE_p_Agedge_t
			Agsym_t* SWIGTYPE_p_Agsym_t
			char* string
			FILE* SWIGTYPE_p_FILE
			bool bool
			void** SWIGTYPE_p_p_void
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
			{System.loadLibrary("gv");}
		}
		USAGE {
		}
	}
	lua {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
			{#!/usr/bin/lua}
			{lib=loadlib('/usr/lib/graphviz/lua/libgv_lua.so','Gv_Init')}
			{assert(lib)()}
		}
		USAGE {
		}
	}
	ocaml {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
		}
		USAGE {
		}
	}
	perl {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv:: (  {, }   {);}
		}
		SYNOPSIS {
			{#!/usr/bin/perl}
			{use gv;}
		}
		USAGE {
		}
	}
	php {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv:: (  {, }   {);}
		}
		SYNOPSIS {
			{dl("libgv_php.so");}
			{include("gv.php")}
		}
		USAGE {
		}
	}
	python {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			gv.  (  {, }   {);}
		}
		SYNOPSIS {
			{#!/usr/bin/python}
			{import sys}
			{sys.path.append('/usr/lib/graphviz/python')}
			{import gv}
		}
		USAGE {
		}
	}
	ruby {
		TYPES {
			{Agraph_t* g} graph_handle
			{Agraph_t* sg} subgraph_handle
			{Agnode_t* n} node_handle
			{Agnode_t* t} tail_node_handle
			{Agnode_t* h} head_node_handle
			{Agedge_t* e} edge_handle
			{Agsym_t* a} attr_handle
			{char* gne} type
			{char* name} name
			{char* tname} tail_name
			{char* hname} head_name
			{char* attr} attr_name
			{char* val} attr_value
			{char* filename} filename
			{char* engine} engine
			{char* string} string
			{char* format} format
			{FILE* f} channel
			{void** data} data_handle
			Agraph_t* graph_handle
			Agnode_t* node_handle
			Agedge_t* edge_handle
			Agsym_t* attribute_handle
			char* string
			FILE* channel
			bool bool
			void** data_handle
			void {}
		}
		SYNTAX {
			Gv.  (  {, }   {);}
		}
		SYNOPSIS {
			{export RUBYLIB=/usr/lib/graphviz/ruby}
			{}
			{#!/usr/bin/ruby}
			{require 'gv'}
		}
		USAGE {
		}
	}
	tcl {
		TYPES {
			{Agraph_t* g} <graph_handle>
			{Agraph_t* sg} <subgraph_handle>
			{Agnode_t* n} <node_handle>
			{Agnode_t* t} <tail_node_handle>
			{Agnode_t* h} <head_node_handle>
			{Agedge_t* e} <edge_handle>
			{Agsym_t* a} <attr_handle>
			{char* gne} <type>
			{char* name} <name>
			{char* tname} <tail_name>
			{char* hname} <head_name>
			{char* attr} <attr_name>
			{char* val} <attr_value>
			{char* filename} <filename>
			{char* engine} <engine>
			{char* string} <string>
			{char* format} <format>
			{FILE* f} <channel>
			{void** data} <data_handle>
			Agraph_t* <graph_handle>
			Agnode_t* <node_handle>
			Agedge_t* <edge_handle>
			Agsym_t* <attr_handle>
			char* <string>
			FILE* <channel>
			bool <boolean_string>
			void** <data_handle>
			void {}
		}
		SYNTAX {
			gv:: {} { } {}
		}
		SYNOPSIS {
			{#!/usr/bin/tclsh}
			{load /usr/lib/graphviz/tcl/gv.so}
		}
		USAGE {
			{Requires tcl7.6 or later.}
		}
	}
}
