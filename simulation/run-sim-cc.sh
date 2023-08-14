#!/bin/bash
# usage run

CONF=${1:-'mix/config_2hosts_topo_2hosts_flow_dcqcn.txt'}
# CONF=${1:-'mix/config_topology_flow_hp95ai50.txt'}
# CONF=${1:-'mix/config_fat_flow_hp95.txt'}

NS3_SRC=../ns-3.33
NS3_BLD=$NS3_SRC/build
NS3_LIB=$(realpath $NS3_BLD)/lib
LOG_LEV=level_all

echo "-- Config file is \"$CONF\""
echo "-- Runing ns3::sim-cc ..."
# $NS3_SRC/waf --run "scratch/sim-cc $CONF "
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NS3_LIB
export NS_LOG=CongestionControlSimulator=$LOG_LEV
# gdb --args $NS3_BLD/scratch/sim-cc $CONF
$NS3_BLD/scratch/sim-cc $CONF
