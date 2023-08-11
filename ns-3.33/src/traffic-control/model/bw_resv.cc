/*
 * bw_resv.cc
 *
 *  Created on: Jan 10, 2019
 *      Author: vamsi
 */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/net-device-queue-interface.h"
#include "bw_resv.h"
#include "queue-disc.h"


//#include "ns3/packet.h"
#include "ns3/queue-item.h"
#include "ns3/ipv4-header.h"
#include "ns3/ethernet-header.h"
#include "ns3/udp-header.h"
#include "ns3/ipv4-queue-disc-item.h"

#define PACKETSDETNET 1

#include <iostream>
#include <fstream>
#include <string>


// #include <unistd.h>

//#include "ns3/ipv4-queue-disc-item.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BwResvQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (BwResvQueueDisc);

TypeId BwResvQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BwResvQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("TrafficControl")
    .AddConstructor<BwResvQueueDisc> ()
    .AddAttribute ("MaxSize",
                   "The max queue size",
                   QueueSizeValue (QueueSize ("1000p")),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
    .AddAttribute ("SizeDetnet",
                   "Size of the first bucket in bytes",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BwResvQueueDisc::SetSizeDetnet),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SizeOther",
                   "Size of the second bucket in bytes. If null, it is initialized"
                   " to the MTU of the receiving NetDevice (if any)",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BwResvQueueDisc::SetSizeOther),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RateDetnet",
                   "Rate at which tokens enter the first bucket in bps or Bps.",
                   DataRateValue (DataRate ("1Kb/s")),
                   MakeDataRateAccessor (&BwResvQueueDisc::SetRateDetnet),
                   MakeDataRateChecker ())
    .AddAttribute ("RateOther",
                   "Rate at which tokens enter the second bucket in bps or Bps."
                   "If null, there is no second bucket",
                   DataRateValue (DataRate ("1Kb/s")),
                   MakeDataRateAccessor (&BwResvQueueDisc::SetRateOther),
                   MakeDataRateChecker ())
    .AddTraceSource ("TokensInFirstBucket",
                     "Number of First Bucket Tokens in bytes",
                     MakeTraceSourceAccessor (&BwResvQueueDisc::m_detnet_tokens),
                     "ns3::TracedValueCallback::Uint32")
    .AddTraceSource ("TokensInSecondBucket",
                     "Number of Second Bucket Tokens in bytes",
                     MakeTraceSourceAccessor (&BwResvQueueDisc::m_other_tokens),
                     "ns3::TracedValueCallback::Uint32")
  ;

  return tid;
}

BwResvQueueDisc::BwResvQueueDisc ()
  : QueueDisc (QueueDiscSizePolicy::SINGLE_CHILD_QUEUE_DISC)
{
  NS_LOG_FUNCTION (this);
}

BwResvQueueDisc::~BwResvQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

void
BwResvQueueDisc::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  QueueDisc::DoDispose ();
}

void
BwResvQueueDisc::SetSizeDetnet (uint32_t sizedetnet)
{
  NS_LOG_FUNCTION (this << sizedetnet);
  m_sizedetnet = sizedetnet;
}

uint32_t
BwResvQueueDisc::GetSizeDetnet (void) const
{
  NS_LOG_FUNCTION (this);
  return m_sizedetnet;
}

void
BwResvQueueDisc::SetSizeOther (uint32_t sizeother)
{
  NS_LOG_FUNCTION (this << sizeother);
  m_sizeother = sizeother;
}

uint32_t
BwResvQueueDisc::GetSizeOther (void) const
{
  NS_LOG_FUNCTION (this);
  return m_sizeother;
}

void
BwResvQueueDisc::SetRateDetnet (DataRate ratedetnet)
{
  NS_LOG_FUNCTION (this << ratedetnet);
  m_ratedetnet = ratedetnet;
}

DataRate
BwResvQueueDisc::GetRateDetnet (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ratedetnet;
}

void
BwResvQueueDisc::SetRateOther (DataRate rateother)
{
  NS_LOG_FUNCTION (this << rateother);
  m_rateother = rateother;
}

DataRate
BwResvQueueDisc::GetRateOther (void) const
{
  NS_LOG_FUNCTION (this);
  return m_rateother;
}

uint32_t
BwResvQueueDisc::GetTokensDetnet (void) const
{
  NS_LOG_FUNCTION (this);
  return m_detnet_tokens;
}

uint32_t
BwResvQueueDisc::GetTokensOther (void) const
{
  NS_LOG_FUNCTION (this);
  return m_other_tokens;
}

void
BwResvQueueDisc::AddDetnetActiveFlow(detnetactiveflow_t tail){
	detnetactiveflows.push_back(tail);
}

BwResvQueueDisc::detnetactiveflow_t
BwResvQueueDisc::RemoveDetnetActiveFlow(void){
	detnetactiveflow_t head = detnetactiveflows.front();
	detnetactiveflows.erase(detnetactiveflows.begin());
	return head;
}

void
BwResvQueueDisc::AddOtherActiveFlow(otheractiveflow_t tail){
	otheractiveflows.push_back(tail);
}

BwResvQueueDisc::otheractiveflow_t
BwResvQueueDisc::RemoveOtherActiveFlow(void){
	otheractiveflow_t head = otheractiveflows.front();
	otheractiveflows.erase(otheractiveflows.begin());
	return head;
}

BwResvQueueDisc::flow_table_t*
BwResvQueueDisc::ClassifyFlow(Ptr<QueueDiscItem> item,uint32_t hash,uint32_t pktsize){
	flow_table_t *flow;
	//uint32_t size = pktsize/8; // size in bytes. pktsize is previously converted to bits.
	uint32_t mod = hash%TABLESIZE;
	uint32_t a=flow_table[mod].size();
	while(a){
		flow=&flow_table[mod][a-1];
		//std::cout << a << "\t"<< flow->vqueue << "\t"<< flow->bwreq<< "\t"<<flow->type<< "\t"<<flow->hash<<  std::endl;
		if(flow->hash == hash)return flow ;
		a--;
	}
	flow= new flow_table_t[1];
	flow->hash=hash;
	flow->vqueue=0;
	Ptr<Packet> pkt = item->GetPacket();
	Ptr<Packet> c = pkt->Copy();
	UdpHeader udp;
	c->RemoveHeader(udp);
	uint8_t *buffer = new uint8_t[c->GetSize ()];
	c->CopyData(buffer, c->GetSize ());
	//std::string s = std::string(buffer);
	//std::string s = std::string(buffer, buffer+c->GetSize()-3);
	std::string s = std::string(buffer, buffer+8);
	DataRate req;
	if(s[7]=='s')
		req=s;
	else
		req=DataRate("100Kbps");
	uint64_t flow_bwreq = req.GetBitRate();
	std::string type = std::string(buffer+9, buffer+10);
	//std::cout << buffer<< "\t"<<s<<item->GetSize()<<"\t"<<type<<  std::endl;
	//std::string type = std::string(buffer+c->GetSize()-2, buffer+c->GetSize()-1);
	flow->bwreq=flow_bwreq;
	flow->type=type;
	if(flow->type=="D")
	flow->threshold=threshold;
	else
		flow->threshold=threshold;
	//flow->last_arrival=Simulator::Now ();
	flow_table[mod].push_back(*flow);
	return flow;
}

bool
BwResvQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);

  Time now = Simulator::Now ();
//  usleep(2*1e3);

   // outfilerx << (now-m_timeCheckPoint_detnet).GetNanoSeconds() <<"\t"<< now.GetSeconds() << std::endl;
  // m_timeCheckPoint_detnet=now;
  //bool retval = GetQueueDiscClass (0)->GetQueueDisc ()->Enqueue (item);
  //return retval;

  uint32_t hash = item->Hash ();
  flow_table_t *flow;
  uint32_t pktsize = (item->GetSize ()+2)*8;
//  std::cout << detnetactiveflows.size()<< std::endl;

/*
*******************************MFD ALGORITHM************************************
*/


  /* 
  Flow Classification
  */
  flow = ClassifyFlow(item,hash,pktsize);


//  uint32_t detnet_consumption=0;
  //std::cout<< detnetactiveflows.size() << "\t"<< otheractiveflows.size()<< std::endl;

//    batch++;
    
    /* 
    Loop which allocates reserved bandwidth for PRIORITY flows 
    */

    if(detnetactiveflows.size() > 0){
		  //	  m_timeCheckPoint=now;
      for(uint32_t i=0;i<detnetactiveflows.size();i++){
        detnetactiveflow_t d_flow = RemoveDetnetActiveFlow();
		    flow_table_t *flow = d_flow.flow;
		    double delta1 = (now  - flow->last_arrival).GetSeconds ();
		    flow->last_arrival=now;
        // m_timeCheckPoint=now;
		    uint32_t req  = (flow->bwreq*delta1);
		    //std::cout << req << delta<< "\t"<< flow->vqueue <<  std::endl;
		    if(flow->vqueue > req){
          flow->vqueue-=req;
          detnet_consumption+=req;
          AddDetnetActiveFlow(d_flow);
        }
		    else{
          detnet_consumption+=flow->vqueue;
          flow->vqueue=0;
        }
      }
    }
    if (batch==0){
    /*
    Credit is the total available bandwidth to be used (in bits)
    */
    double delta = (now  - m_timeCheckPoint).GetSeconds ();
    uint32_t credit = (m_ratedetnet.GetBitRate ()*delta);
//    m_timeCheckPoint=now;

    /* 
    Loop which allocates remaining unused bandwidth for NON-PRIORITY flows 
    */

    if(credit > detnet_consumption && otheractiveflows.size() > 0){
     m_timeCheckPoint=now;
	  flow_table_t *flow;
	  otheractiveflow_t o_flow;
	  float rem_credit=credit-detnet_consumption;
	  float served;
	  uint32_t old_nbl=otheractiveflows.size()+1;
	  //std::cout << otheractiveflows.size()<<std::endl;
	  while(old_nbl>otheractiveflows.size() && otheractiveflows.size()>0){
      old_nbl=otheractiveflows.size();
      served = rem_credit/old_nbl;
      rem_credit=0;
      for (uint32_t i=0;i<old_nbl;i++){
        o_flow = RemoveOtherActiveFlow();
        flow=o_flow.flow;
        if(flow->vqueue > served){
          flow->vqueue -= served;
          AddOtherActiveFlow(o_flow);
        }
        else{
          rem_credit+=served-flow->vqueue;
      	  flow->vqueue=0;
        }
      }
    }
	  detnet_consumption=0;
  }
  batch=0;

}




// Packet Accepted or Dropped
  if(flow->vqueue <=flow->threshold){
	  if(flow->vqueue == 0){
		  if(flow->type=="D"){
			  nbl_detnet++;
			  detnetactiveflow_t newflow;
			  newflow.flow=flow;
			  AddDetnetActiveFlow(newflow);
		  }
		  else{
			  nbl_other++;
			  otheractiveflow_t newflow;
			  newflow.flow=flow;
			  AddOtherActiveFlow(newflow);

		  }
	  }
	  flow->vqueue+=pktsize;
//	  flow->last_arrival=now;
	  //std::cout << flow.vqueue <<  std::endl;
//	  flow->last_arrival=Simulator::Now ();
//	  m_timeCheckPoint=now;
	  // if(flow->type=="D")
	  // flow->threshold_flow=flow->threshold_flow*1.1;
	  // else
		 //  threshold=threshold*1.1;
  }
  else{
	  DropBeforeEnqueue (item, FORCED_DROP);
	  // if(flow->type=="D")
	  // flow->threshold_flow=flow->threshold_flow*0.5;
	  // else
		 //  threshold=threshold*0.5;
	  return false;
  }



  bool retval = GetQueueDiscClass (0)->GetQueueDisc ()->Enqueue (item);
  
  
  // If Queue::Enqueue fails, QueueDisc::Drop is called by the child queue
  // disc because QueueDisc::AddQueueDiscClass sets the drop callback

  NS_LOG_LOGIC ("Current queue size: " << GetNPackets () << " packets, " << GetNBytes () << " bytes");

  return retval;
}

Ptr<QueueDiscItem>
BwResvQueueDisc::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);

  if (PACKETSDETNET==1){
     std::ofstream outfilepkt;
     outfilepkt.open("/home/vamsi/src/detnet/scripts/results/pkt.dat", std::ios_base::app);
     outfilepkt<< GetQueueDiscClass(0)->GetQueueDisc()->GetNPackets()<< "\t" << Simulator::Now().GetSeconds()  <<std::endl;
    }

  //Ptr<const QueueDiscItem> itemPeek = GetQueueDiscClass (0)->GetQueueDisc ()->Peek ();

//  if (GetQueueDiscClass (0)->GetQueueDisc ()-> IsEmpty () )
//	  return 0;
//  else{

//    std::ofstream outfilepkt;


  	  Ptr<QueueDiscItem> item = GetQueueDiscClass (0)->GetQueueDisc ()->Dequeue ();
 	//uint32_t s = GetQueueDiscClass (0)->GetQueueDisc ()->GetNPackets();
  	// if(s>=40){
  	// 	threshold=threshold*0.9;
  	// }
  	// else if ( s>=5 ){
   //    if(num==100){
  	// 	  threshold+=1000*8;
   //      num=0;
   //    }
   //    else
   //      num++;
  	// }
//  	 outfilepkt.open("/home/vamsi/src/detnet/scripts/results/pkt.dat", std::ios_base::app);
//  	  outfilepkt<< GetQueueDiscClass(0)->GetQueueDisc()->GetNPackets()<< "\t" << Simulator::Now().GetSeconds() << "\t" << threshold/(1000*8) <<std::endl;
	  //if(item!= NULL )
	  return item;
	  //else return 0;
//  }
/*
  if (itemPeek)
    {
      uint32_t pktSize = itemPeek->GetSize ();
      NS_LOG_LOGIC ("Next packet size " << pktSize);

      int64_t btoks = 0;
      int64_t ptoks = 0;
      Time now = Simulator::Now ();

      double delta = (now  - m_timeCheckPoint).GetSeconds ();
      NS_LOG_LOGIC ("Time Difference delta " << delta);

      if (m_peakRate > DataRate ("0bps"))
        {
          ptoks =  m_ptokens + round (delta * (m_rateother.GetBitRate () / 8));
          if (ptoks > m_mtu)
            {
              ptoks = m_mtu;
            }
          NS_LOG_LOGIC ("Number of ptokens we can consume " << ptoks);
          NS_LOG_LOGIC ("Required to dequeue next packet " << pktSize);
          ptoks -= pktSize;
        }

      btoks = m_btokens + round (delta * (m_rate.GetBitRate () / 8));

      if (btoks > m_burst)
        {
          btoks = m_burst;
        }

      NS_LOG_LOGIC ("Number of btokens we can consume " << btoks);
      NS_LOG_LOGIC ("Required to dequeue next packet " << pktSize);
      btoks -= pktSize;

      if ((btoks|ptoks) >= 0) // else packet blocked
        {
          Ptr<QueueDiscItem> item = GetQueueDiscClass (0)->GetQueueDisc ()->Dequeue ();
          if (!item)
            {
              NS_LOG_DEBUG ("That's odd! Expecting the peeked packet, we got no packet.");
              return item;
            }

          m_timeCheckPoint = now;
          m_btokens = btoks;
          m_ptokens = ptoks;

          NS_LOG_LOGIC (m_btokens << " btokens and " << m_ptokens << " ptokens after packet dequeue");
          NS_LOG_LOGIC ("Current queue size: " << GetNPackets () << " packets, " << GetNBytes () << " bytes");

          return item;
        }

      // the watchdog timer setup.
      // A packet gets blocked if the above if condition is not satisfied, i.e.
      //both the ptoks and btoks are less than zero. In that case we have to
      //schedule the waking of queue when enough tokens are available.
      if (m_id.IsExpired () == true)
        {
          Time requiredDelayTime = std::max (m_rate.CalculateBytesTxTime (-btoks),
                                             m_peakRate.CalculateBytesTxTime (-ptoks));

          m_id = Simulator::Schedule (requiredDelayTime, &QueueDisc::Run, this);
          NS_LOG_LOGIC("Waking Event Scheduled in " << requiredDelayTime);
        }
    }
  */

  //return 0;
}

bool
BwResvQueueDisc::CheckConfig (void)
{
  NS_LOG_FUNCTION (this);
  if (GetNInternalQueues () > 0)
    {
      NS_LOG_ERROR ("TbfQueueDisc cannot have internal queues");
      return false;
    }

  if (GetNPacketFilters () > 0)
    {
      NS_LOG_ERROR ("TbfQueueDisc cannot have packet filters");
      return false;
    }

  if (GetNQueueDiscClasses () == 0)
    {
      // create a FIFO queue disc
      ObjectFactory factory;
      factory.SetTypeId ("ns3::FifoQueueDisc");
      Ptr<QueueDisc> qd = factory.Create<QueueDisc> ();

      if (!qd->SetMaxSize (GetMaxSize ()))
        {
          NS_LOG_ERROR ("Cannot set the max size of the child queue disc to that of TbfQueueDisc");
          return false;
        }
      qd->Initialize ();

      Ptr<QueueDiscClass> c = CreateObject<QueueDiscClass> ();
      c->SetQueueDisc (qd);
      AddQueueDiscClass (c);
    }

  if (GetNQueueDiscClasses () != 1)
    {
      NS_LOG_ERROR ("TbfQueueDisc needs 1 child queue disc");
      return false;
    }
/*
  if (m_sizeother == 0)
    {
      Ptr<NetDeviceQueueInterface> ndqi = GetNetDeviceQueueInterface ();
      Ptr<NetDevice> dev;
      // if the NetDeviceQueueInterface object is aggregated to a
      // NetDevice, get the MTU of such NetDevice
      if (ndqi && (dev = ndqi->GetObject<NetDevice> ()))
        {
          m_sizeother = dev->GetMtu ();
        }
    }

  if (m_sizeother == 0 && m_rateother > DataRate ("0bps"))
    {
      NS_LOG_ERROR ("A non-null peak rate has been set, but the mtu is null. No packet will be dequeued");
      return false;
    }

  if (m_burst <= m_mtu)
    {
      NS_LOG_WARN ("The size of the first bucket (" << m_burst << ") should be "
                    << "greater than the size of the second bucket (" << m_mtu << ").");
    }

  if (m_peakRate > DataRate ("0bps") && m_peakRate <= m_rate)
    {
      NS_LOG_WARN ("The rate for the second bucket (" << m_peakRate << ") should be "
                    << "greater than the rate for the first bucket (" << m_rate << ").");
    }
*/
  return true;
}

void
BwResvQueueDisc::InitializeParams (void)
{
  NS_LOG_FUNCTION (this);
  // Token Buckets are full at the beginning.
  m_detnet_tokens = m_sizedetnet;
  m_other_tokens = m_sizeother;
  // Initialising other variables to 0.
  m_timeCheckPoint_detnet = Seconds (0);
  m_timeCheckPoint_other = Seconds (0);
  m_timeCheckPoint = Seconds (0);
  m_id = EventId ();
  uint32_t z = 960;
  threshold=1030*8*z;
  nbl_detnet=0;
  nbl_other=0;
  num=0;
}

} // namespace ns3
