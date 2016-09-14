FROM centos:7

# Install Development tools
RUN yum -y groupinstall 'Development Tools'

# Instal autotools utilities
RUN yum -y install libtool-ltdl-devel ghostscript swig ksh tcl

# Install dependencies
RUN yum -y install gd gd-devel qt-devel
