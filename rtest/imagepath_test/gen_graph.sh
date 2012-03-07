#! /bin/ksh

#directory where the executable is located
project_dir=`pwd | sed -f delim.sed`

#directory pointed to by the image attribute
image_dir=`pwd | sed -e s:$:\\/image_dir: | sed -f delim.sed`

#directory pointed to by the imagepath attribute
imagepath_dir=`pwd | sed -e s:$:\\/imagepath_dir: | sed -f delim.sed`

#an invalid absolute path
inv_dir=`echo -n "/inv_dir" | sed -f delim.sed`

#echo "project_dir=${project_dir}"
#echo "abs_dir=${abs_dir}"
#echo "rel_dir=${rel_dir}"
#echo "cur_dir=${cur_dir}"
#echo "inv_dir=${inv_dir}"

#input parameters: 
#	a - image attribute directory type: val (valid), inv (invalid), nul (not present)
#	b - imagepath attribute directory type: val (valid), inv (invalid), nul (not present)
#	c - expectation
# 	d - name of output graph

read a b c d

#set image attribute directory
case $a in
	"val") image="${image_dir}\/";
		image_attr="valid absolute pathname";;
	"inv") image="${inv_dir}\/";
		image_attr="invalid absolute pathname";;
	"nul") image="";
		image_attr="null";;
	*)	echo "Missing image attribute";
		echo "Usage arg0 image imagepath expectation output_filename";
		echo "image and imagepath valid values: val, inv, nul"
		echo "expectation - expected outcome message"
		echo "output_filename - defaults to test"
		exit 1;;
esac

#set imagepath attribute directory
case $b in
	"val") imagepath="${imagepath_dir}\/";
		imagepath_attr="valid absolute pathname";;
	"inv") imagepath="${inv_dir}\/";
		imagepath_attr="invalid absolute pathname";;
	"nul") imagepath="";
		imagepath_attr="null";;
	*)	echo "Missing imagepath attribute";
		echo "Usage arg0 image imagepath expectation output_filename";
		echo "image and imagepath valid values: val, inv, nul"
		echo "expectation - expected outcome message"
		echo "output_filename - defaults to test"
		exit 1;;
esac

#set the expectation message that will be displayed on graph output
case $c in
	"")	echo "Missing expectation message";
		echo "Usage arg0 image imagepath expectation output_filename";
		echo "image and imagepath valid values: val, inv, nul"
		echo "expectation - expected outcome message"
		echo "output_filename - defaults to test"
		exit 1;;
	*) expect=$c;;
esac


#set the output graph filename
case $d in
	"") outfile="test";;
	*) outfile=$d;;
esac

echo
echo "case: ${a}_${b}"
echo "image attribute: ${image_attr}"
echo "imagepath attribute: ${imagepath_attr}"

#base.gv is a template for the graph being created
#construct the graph file and store in ../graphs
#space characters must be converted to '^' characters to not confuse sed
#restore '^' character to space before outputting the graph

config="<TR><TD ALIGN=\"left\"><FONT POINT-SIZE=\"30.0\" FACE=\"Helvetica-Bold\">image attribute \(${image_attr}\)<\/FONT><\/TD><\/TR><TR><TD ALIGN=\"left\"><FONT POINT-SIZE=\"30.0\" FACE=\"Helvetica-Bold\">imagepath attribute \(${imagepath_attr}\)<\/FONT><\/TD><\/TR>"
config=`echo -n $config | tr ' ' '^'`
expect=`echo -n $expect | tr ' ' '^'`
sed -e s/image_attr/image\=\"${image}image.jpg\",labelloc=b/ \
-e s/expect/${expect}/ \
-e s/config/${config}/ \
-e s/imagepath_attr/imagepath\=\"${imagepath}\"/ base.gv | tr '^' ' ' > ../graphs/${outfile}.gv

#place the image file into the nshare directory
dot -Tpng -o ../nshare/${outfile}_dot.png ../graphs/${outfile}.gv


