# DetNET and RDMA simulation

Simulator for deterministic networks (DetNet) and time sensitive networks, with RDMA/RoCE transport. Analysis for Congestion Control, Lang-range Transport Rate Control and Adaptive Routings.

## RDMA and Congestion Control

RDMA and Congestion Control simulation based on [ns3-hpcc](https://github.com/alibaba-edu/High-Precision-Congestion-Control), with changes:

- Upgrades NS-3 from version 3.18 to 3.33 ([release notes](https://www.nsnam.org/releases/ns-3-33/))
- Use Python 3.0+
- Support recent version of gcc/g++ (other than gcc-5), had tested in ubuntu 20.04.6 LTS (setup scripts are in `devenv/ubuntu20/`)

## DetNET simulation

Currently we based on [ns3-detnet](https://github.com/nextvincii/detnet), which use ns-3.3x. More changes are updating, will update soon...

## Point to Point Rate Control

Using AI/ML frameworks [ns3-ai](https://github.com/hust-diangroup/ns3-ai), which enables the interaction between ns-3 and popular frameworks using Python, which mean we can train and test AI algorithms. Such as:

- RL-TCP: Apply RL(Deep Q-learning) algorithm to TCP congestion control for real-time changes in the environment of network transmission.
- LTE_CQI: 5G CQI Prediction simulation
- WiFi (802.11) Rate-Control: _we will use this sample to construct simulation for our long-rang file transport protocol_

## Simulation

The ns-3 simulation is under `ns3-3.33/`. Refer to the README.md under it for more details.

## Analysis

_To be done_

## Questions

For technical questions, please create an issue in this repo, so other people can benefit from your questions.
You may also check the issue list first to see if people have already asked the questions you have :)
