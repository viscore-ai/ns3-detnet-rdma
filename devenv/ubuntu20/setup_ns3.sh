#!/bin/bash
#
# Setup development environments in ubuntu
# See [ns-3 Installation](https://www.nsnam.org/wiki/Installation)

```Bash
# build tools
sudo apt install -y g++ python3 python3-dev pkg-config cmake castxml
# documents
sudo apt install -y doxygen graphviz imagemagick
# state database
sudo apt install -y sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev
# GSL
sudo apt install -y gsl-bin libgsl-dev libgslcblas0 libgsl23
# bake
sudo apt install -y libc6-dev libc6-dev-i386 libclang-dev llvm-dev automake
# ns-3-pyviz
sudo apt install -y libgtk-3-dev gir1.2-goocanvas-2.0 graphviz libgraphviz-dev
# MPI
sudo apt install -y openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
# pcap
sudo apt install -y tcpdump

# ----------------- Setup Python Modules ------------------

# python3 -m pip install -U --user pybindgen pygccxml cxxfilt mako CastXml
sudo python3 -m pip install -U pybindgen pygccxml cxxfilt mako CastXml
# poetry python3-gi-cairo
# python3 -m pip install -U --user pygi pycairo pygraphviz
sudo python3 -m pip install -U pygi pycairo pygraphviz
