/*
 * detnet.cc
 *
 *  Created on: Dec 4, 2018
 *      Author: vamsi
 */

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/animation-interface.h"
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/csma-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

#include <unistd.h> //for sleep

using namespace ns3;

#define QU

//#define REDQU
//#define FIFOQU
#define DETNETQU

#define N_FLOWS 4

NS_LOG_COMPONENT_DEFINE ("DetnetSimulator");
FlowMonitorHelper flowHelper;
Ptr<FlowMonitor> flowmon;

inline void Flow_Monitor(Ptr<FlowMonitor> flowMonitor,FlowMonitorHelper flowHelper){
	flowMonitor = flowHelper.InstallAll();
}

void K(){
	flowmon->ClearFlowStats();
}
void F(){
	std::ofstream outfilerx;
	outfilerx.open("results/rx.dat", std::ios_base::app);
	std::ofstream outfilelat;
	outfilelat.open("results/delay.dat", std::ios_base::app);
	std::ofstream outfiletx;
	outfiletx.open("results/tx.dat", std::ios_base::app);


	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats ();
	uint32_t n=0;
	double total = 0;
	double total_delay=0;
	uint32_t total_packets=0;
	double total_jitter=0;

	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	  {
		//  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	    {
	        //std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\t";
	        //std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	        //std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
	        total+=i->second.rxBytes * 8.0 / (Seconds(0.8).GetSeconds())/(1000);
	    	  outfilerx << "  ThroughputRx: " << i->second.rxBytes * 8.0 / (Seconds(0.8).GetSeconds())/(1000)  << " kbps\t";
	    	  outfilelat << "meandelay: " << (i->second.delaySum.GetSeconds() / i->second.rxPackets) << " sec\t" << "meanJitter: " << (i->second.jitterSum.GetSeconds()/i->second.rxPackets) << " sec\t"; //i->second.delaySum.GetSeconds() / i->second.rxPackets
	    	  total_delay+=i->second.delaySum.GetSeconds();
	    	  total_jitter+=i->second.jitterSum.GetSeconds();
	    	  total_packets+=i->second.rxPackets;
	    	  //std::cout << "  ThroughputTx: " << i->second.txBytes * 8.0 / (i->second.timeLastTxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1000000  << " mbps\t";
	    }
	    n++;
	   }
	while(n<N_FLOWS){
		outfilerx << "  ThroughputRx: " << "0"  << " kbps\t";
		outfilelat << "meandelay: " << 0 << " sec\t" << "meanJitter: " << 0 << " sec\t";
	   	n++;
	}

	outfilerx << "  ThroughputRx: " << total  << " kbps\t";
	outfilelat << "meanTotaldelay: " << total_delay/total_packets << " sec\t"<< "meanTotalJitter: "<< total_jitter/total_packets << " sec\t";
	outfilerx << std::endl;
	outfilelat << std::endl;
	n=0;
	total=0;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
		  {
			//  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
		    {
		        //std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\t";
		        //std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
		        //std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
		    	  //std::cout << "  ThroughputRx: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstRxPacket.GetSeconds())/1000000  << " mbps\t";
		    	total+=i->second.txBytes * 8.0 /(Seconds(0.8).GetSeconds())/(1000);
		    	  outfiletx << "  ThroughputTx: " << i->second.txBytes * 8.0 /(Seconds(0.8).GetSeconds())/(1000)  << " kbps\t";
		    }
		    n++;
		   }
	while(n<N_FLOWS){
	   	outfiletx << "  ThroughputTx: " << "0"  << " kbps\t";
	   	n++;
	}
	outfiletx << "  ThroughputTx: " << total  << " kbps\t";
	outfiletx << std::endl;
	//sleep(2); 
 
	//flowmon->ClearFlowStats(); // @suppress("Invalid arguments")
}
int
main (int argc, char *argv[])
{
	Time::SetResolution(Time::NS);
	//LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_ALL);
	//LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_ALL);

	//Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (1500));
	//Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));

	//Config::SetDefault("ns3::TcpL4Protocol::SocketType",StringValue("ns3:TcpNewReno"));
	//Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName ("TcpHighSpeed")));
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1472));
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

	std::string bandwidth = "10Mbps";
	std::string delay = "5ms";
	std::string queueDiscType = "BwResv";

	#ifdef REDQU
	//uint32_t meanPktSize = 500;
	//uint32_t queueDiscSize = 1000;
	bool bql = false;
	#endif
	//uint32_t netdevicesQueueSize = 50;
	//bool bql = false;

	std::string flowsDatarate = "20Mbps";
//	uint32_t flowsPacketsSize = 1000;

	NS_LOG_INFO ("Create nodes.");
	NodeContainer c;
	c.Create(8);

	// node containers with respect to links
	NodeContainer n0n2 = NodeContainer(c.Get(0),c.Get(2));
	NodeContainer n1n2 = NodeContainer(c.Get(1),c.Get(2));
	NodeContainer n2n3 = NodeContainer(c.Get(2),c.Get(3));
	NodeContainer n4n3 = NodeContainer(c.Get(4),c.Get(3));
	NodeContainer n5n3 = NodeContainer(c.Get(5),c.Get(3));

	NodeContainer n2n6 = NodeContainer(c.Get(2),c.Get(6));
	NodeContainer n6n3 = NodeContainer(c.Get(6),c.Get(3));
	NodeContainer n2n7 = NodeContainer(c.Get(2),c.Get(7));
	NodeContainer n7n3 = NodeContainer(c.Get(7),c.Get(3));

	// Router node containers
	NodeContainer routers = NodeContainer(c.Get(2),c.Get(3),c.Get(6),c.Get(7));

	// end hosts node containers

	NodeContainer hosts = NodeContainer(c.Get(0),c.Get(1),c.Get(4),c.Get(5));

/*	RipHelper ripRouting;
	// Rule of thumb:
	// Interfaces are added sequentially, starting from 0
	// However, interface 0 is always the loopback...
	ripRouting.ExcludeInterface (routers.Get(0), 1);
	ripRouting.ExcludeInterface (routers.Get(0), 2);
	ripRouting.ExcludeInterface (routers.Get(1), 1);
	ripRouting.ExcludeInterface (routers.Get(1), 2);
	ripRouting.SetInterfaceMetric (routers.Get(0), 3, 10);
	Ipv4ListRoutingHelper listRH;
	listRH.Add (ripRouting, 0);
	//  Ipv4StaticRoutingHelper staticRh;
	//  listRH.Add (staticRh, 5);

	InternetStackHelper internet_rip;
	internet_rip.SetIpv6StackInstall (false);
	internet_rip.SetRoutingHelper (listRH);
	internet_rip.Install (routers);
*/
	InternetStackHelper internet;
	internet.Install(hosts);
	internet.Install(routers);
	Ipv4AddressHelper ipv4;

	// Point to point channels

	//Type-0
	PointToPointHelper type0;
	type0.SetDeviceAttribute("DataRate",StringValue("30Mbps"));
	type0.SetChannelAttribute("Delay",StringValue("2ms"));

	//type-1
	PointToPointHelper type1;
	type1.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
	type1.SetChannelAttribute("Delay",StringValue("2ms"));

	//type-2
	PointToPointHelper type2;
	type2.SetDeviceAttribute("DataRate",StringValue("21Mbps"));
	type2.SetChannelAttribute("Delay",StringValue("2ms"));

	TrafficControlHelper tchBottleneck;
	#ifdef REDQU
	// NS_LOG_INFO ("Set RED params");
 //  	Config::SetDefault ("ns3::RedQueueDisc::MaxSize", StringValue ("1000p"));
 //  	Config::SetDefault ("ns3::RedQueueDisc::MeanPktSize", UintegerValue (meanPktSize));
 //  	Config::SetDefault ("ns3::RedQueueDisc::Wait", BooleanValue (true));
 //  	Config::SetDefault ("ns3::RedQueueDisc::Gentle", BooleanValue (true));
 //  	Config::SetDefault ("ns3::RedQueueDisc::QW", DoubleValue (0.002));
 //  	Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (5));
 //  	Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (15));
	tchBottleneck.SetRootQueueDisc ("ns3::RedQueueDisc");
	//Config::SetDefault ("ns3::RedQueueDisc::ARED", BooleanValue (true));
	//Config::SetDefault ("ns3::RedQueueDisc::MaxSize",QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, queueDiscSize)));
		if (bql){
		tchBottleneck.SetQueueLimits ("ns3::DynamicQueueLimits");
	}
	#endif

	#ifdef FIFOQU
	tchBottleneck.SetRootQueueDisc ("ns3::FifoQueueDisc");
	#endif

	#ifdef DETNETQU
	tchBottleneck.SetRootQueueDisc ("ns3::BwResvQueueDisc","RateOther",DataRateValue(DataRate("1000kbps")),"RateDetnet",DataRateValue(DataRate("10000kbps")));
	#endif

	// Net Devices
	NetDeviceContainer d0d2 = type0.Install(n0n2);
	NetDeviceContainer d1d2 = type0.Install(n1n2);
	NetDeviceContainer d4d3 = type0.Install(n4n3);
	NetDeviceContainer d5d3 = type0.Install(n5n3);
	NetDeviceContainer d2d3 = type1.Install(n2n3);// Routerlink tchBottleneck.Install(d2d3);
	NetDeviceContainer d2d6 = type1.Install(n2n6);// Routerlink tchBottleneck.Install(d2d6);
	NetDeviceContainer d2d7 = type1.Install(n2n7);// Routerlink tchBottleneck.Install(d2d7);
	NetDeviceContainer d6d3 = type1.Install(n6n3);// Routerlink tchBottleneck.Install(d6d3);
	NetDeviceContainer d7d3 = type1.Install(n7n3);// Routerlink tchBottleneck.Install(d7d3);

	#ifdef QU
	tchBottleneck.Install(d2d3);
	// tchBottleneck.Install(d2d6);
	// tchBottleneck.Install(d2d7);
	// tchBottleneck.Install(d6d3);
	// tchBottleneck.Install(d7d3);
	#endif

	// IP addressing

	ipv4.SetBase("10.1.1.0","255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);

	ipv4.SetBase("10.1.2.0","255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);

	ipv4.SetBase("10.1.3.0","255.255.255.0");
	Ipv4InterfaceContainer i4i3 = ipv4.Assign(d4d3);

	ipv4.SetBase("10.1.4.0","255.255.255.0");
	Ipv4InterfaceContainer i5i3 = ipv4.Assign(d5d3);

	ipv4.SetBase("10.1.5.0","255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);

	ipv4.SetBase("10.1.6.0","255.255.255.0");
	Ipv4InterfaceContainer i2i6 = ipv4.Assign(d2d6);

	ipv4.SetBase("10.1.7.0","255.255.255.0");
	Ipv4InterfaceContainer i2i7 = ipv4.Assign(d2d7);

	ipv4.SetBase("10.1.8.0","255.255.255.0");
	Ipv4InterfaceContainer i6i3 = ipv4.Assign(d6d3);

	ipv4.SetBase("10.1.9.0","255.255.255.0");
	Ipv4InterfaceContainer i7i3 = ipv4.Assign(d7d3);

	Ptr<Ipv4StaticRouting> staticRouting;
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (hosts.Get(0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.1.1.2", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (hosts.Get(1)->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.1.2.2", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (hosts.Get(2)->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.1.3.2", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (hosts.Get(3)->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.1.4.2", 1 );

	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	// Create the OnOff application to send UDP datagrams of size
	// 210 bytes at a rate of 448 Kb/s
	NS_LOG_INFO ("Create Applications.");


	type0.EnablePcapAll("detnet_2_0");
	type1.EnablePcapAll("detnet_2_1");
	

////////////////////////
	   uint16_t port = 9;   // Discard port (RFC 863)
	   OnOffHelper onoff ("ns3::UdpSocketFactory",Address (InetSocketAddress (i4i3.GetAddress (0), port)));
	   OnOffHelper onoff1 ("ns3::UdpSocketFactory",Address (InetSocketAddress (i4i3.GetAddress (0), port)));
	   onoff.SetAttribute("PacketSize",UintegerValue(1000));
	   onoff1.SetAttribute("PacketSize",UintegerValue(1000));

	   /*
	    * ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000]"));
  m_factory.Set ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  m_factory.Set ("DataRate", DataRateValue (dataRate));
  m_factory.Set ("PacketSize", UintegerValue (packetSize));
	    */
//
//	   onoff.SetAttribute("PacketSize",UintegerValue(1000));
//	   onoff.SetAttribute("DataRate", DataRateValue("1Mbps"));
//	   onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
//	   onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.010666667]"));

	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app0
	   onoff.SetAttribute("Buffer",StringValue("0100Kbps D")); //12 bytes reserved
	   ApplicationContainer apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (2));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app1
	   onoff.SetAttribute("Buffer",StringValue("0200Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (4));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app2
	   onoff.SetAttribute("Buffer",StringValue("0300Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (5));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app3
	   onoff.SetAttribute("Buffer",StringValue("0400Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (6));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app4
	   onoff.SetAttribute("Buffer",StringValue("0500Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (7));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app5
	   onoff.SetAttribute("Buffer",StringValue("0600Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (8));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app6
	   onoff.SetAttribute("Buffer",StringValue("0700Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (9));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app7
	   onoff.SetAttribute("Buffer",StringValue("0800Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (10));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app8
	   onoff.SetAttribute("Buffer",StringValue("0900Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (11));
	   apps.Stop (Seconds (20.0));

	   onoff.SetAttribute ("Remote",AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
	   onoff.SetConstantRate (DataRate ("1000Kbps")); //app9
	   onoff.SetAttribute("Buffer",StringValue("4000Kbps D")); //12 bytes reserved
	   apps = onoff.Install (c.Get (0));
	   apps.Start (Seconds (12));
	   apps.Stop (Seconds (20.0));

	   onoff1.SetAttribute ("Remote",AddressValue (InetSocketAddress (i4i3.GetAddress (0), port)));
	   onoff1.SetConstantRate (DataRate ("8000Kbps")); //appOther
	   onoff1.SetAttribute("Buffer",StringValue("1000Kbps O")); //12 bytes reserved
	   apps = onoff1.Install (c.Get (0));
	   apps.Start (Seconds (1));
	   apps.Stop (Seconds (20.0));

	   onoff1.SetAttribute ("Remote",AddressValue (InetSocketAddress (i4i3.GetAddress (0), port)));
	   onoff1.SetConstantRate (DataRate ("8000Kbps")); //appOther
	   onoff1.SetAttribute("Buffer",StringValue("1000Kbps O")); //12 bytes reserved
	   apps = onoff1.Install (c.Get (0));
	   apps.Start (Seconds (3));
	   apps.Stop (Seconds (20.0));

	   // Create a packet sink to receive these packets

	   PacketSinkHelper sink ("ns3::UdpSocketFactory",
	                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
	   apps = sink.Install (c.Get (4));
	   apps.Start (Seconds (1.0));
	   apps.Stop (Seconds (20.0));

	   PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
	   	                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
	   apps = sink1.Install (c.Get (5));
	   apps.Start (Seconds (1.0));
	   apps.Stop (Seconds (20.0));
//////////////////////////////


//	   onoff.SetAttribute ("Remote",
//	                       AddressValue (InetSocketAddress (i5i3.GetAddress (0), port)));
//	   onoff.SetConstantRate (DataRate ("6Mbps"));
//	   apps = onoff.Install (c.Get (0));
//	   apps.Start (Seconds (2.0));
//	   apps.Stop (Seconds (10.0));
//
//	   apps = sink.Install(c.Get(5));
//	   apps.Start(Seconds(2.0));
//	   apps.Stop(Seconds(10.0));


//	   Ptr<Node> n2 = c.Get (2);
//	   Ptr<Ipv4> ipv42 = n2->GetObject<Ipv4> ();
//	   uint32_t ipv4ifIndex3 = 3;
//	   //Simulator::Schedule (Seconds (2.1), &Ipv4GlobalRoutingHelper::RecomputeRoutingTables);
//	   Simulator::Schedule (Seconds (2),&Ipv4::SetDown,ipv42, ipv4ifIndex3);
//	   Simulator::Schedule (Seconds (2.1),&Ipv4::SetUp,ipv42, ipv4ifIndex3);



//	   // Create a packet sink to receive these packets
//	   apps = sink.Install (c.Get (4));
//	   apps.Start (Seconds (1.0));
//	   apps.Stop (Seconds (10.0));

	   AnimationInterface anim ("detnet_2.xml");
	   anim.SetConstantPosition(c.Get(0),10,25);
	   anim.SetConstantPosition(c.Get(1),10,75);
	   anim.SetConstantPosition(c.Get(2),30,50);
	   anim.SetConstantPosition(c.Get(3),50,50);
	   anim.SetConstantPosition(c.Get(4),70,25);
	   anim.SetConstantPosition(c.Get(5),70,75);
	   anim.SetConstantPosition(c.Get(6),40,25);
	   anim.SetConstantPosition(c.Get(7),40,75);

	   anim.EnableIpv4RouteTracking("detnet_2_rout.xml",Seconds(2),Seconds(11),Seconds(1));
	   //Ptr<FlowMonitor> flowmon;

	   flowmon = flowHelper.InstallAll();

	   //flowmon->Start(Seconds(4));
	   Simulator::Schedule(Seconds (1.1),&K);
	   Simulator::Schedule(Seconds (1.9),&F);
	   Simulator::Schedule(Seconds (2.1),&K);
	   Simulator::Schedule(Seconds (2.9),&F);
	   Simulator::Schedule(Seconds (3.1),&K);
	   Simulator::Schedule(Seconds (3.9),&F);
	   Simulator::Schedule(Seconds (4.1),&K);
	   Simulator::Schedule(Seconds (4.9),&F);
	   Simulator::Schedule(Seconds (5.1),&K);
	   Simulator::Schedule(Seconds (5.9),&F);
	   Simulator::Schedule(Seconds (6.1),&K);
	   Simulator::Schedule(Seconds (6.9),&F);
	   Simulator::Schedule(Seconds (7.1),&K);
	   Simulator::Schedule(Seconds (7.9),&F);
	   Simulator::Schedule(Seconds (8.1),&K);
	   Simulator::Schedule(Seconds (8.9),&F);
	   Simulator::Schedule(Seconds (9.1),&K);
	   Simulator::Schedule(Seconds (9.9),&F);
	   Simulator::Schedule(Seconds (10.1),&K);
	   Simulator::Schedule(Seconds (10.9),&F);
	   Simulator::Schedule(Seconds (11.1),&K);
	   Simulator::Schedule(Seconds (11.9),&F);
	   Simulator::Schedule(Seconds (12.1),&K);
	   Simulator::Schedule(Seconds (12.9),&F);
	   Simulator::Schedule(Seconds (19.1),&K);
	   Simulator::Schedule(Seconds (19.9),&F);

//	   for (uint8_t i=2;i<11;i++){
//		   Simulator::Schedule(Seconds (i),&F);
//	   }


  Simulator::Stop (Seconds (20));
  Simulator::Run ();
  flowmon->SerializeToXmlFile(queueDiscType + "-flowMonitor.xml", true, true);
//  for (uint8_t i=0;i<11;i++){
//	  flowmon[i]->SerializeToXmlFile(queueDiscType + "-flowMonitor_" + std::to_string(i) + ".xml", true, true);
//  }
  //anim.EnablePacketMetadata(true);
  Simulator::Destroy ();

  return 0;
}



