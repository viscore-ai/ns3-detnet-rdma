#!/bin/bash
# usage run

CONF=${1:-''}

NS3_SRC=../ns-3.33
NS3_BLD=$NS3_SRC/build
NS3_LIB=$(realpath $NS3_BLD)/lib
LOG_LEV=level_all

echo "-- Config file is \"$CONF\""
echo "-- Runing ns3::sim-detnet ..."
# $NS3_SRC/waf --run "scratch/sim-detnet $CONF "
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NS3_LIB
export NS_LOG=DetnetSimulator=$LOG_LEV
gdb --args $NS3_BLD/scratch/sim-detnet $CONF
# $NS3_BLD/scratch/sim-detnet $CONF
