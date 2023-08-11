/*
 * bw_resv.h
 *
 *  Created on: Jan 10, 2019
 *      Author: vamsi
 */


#ifndef BW_RESV_QUEUE_DISC_H
#define BW_RESV_QUEUE_DISC_H

#include "ns3/queue-disc.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/event-id.h"
//#include "vector.h"
#include <iostream>

namespace ns3 {

/**
 * \ingroup traffic-control
 *
 * \brief A BwResv packet queue disc
 */
class BwResvQueueDisc : public QueueDisc
{
public:

	typedef struct flow_table{
		uint32_t vqueue;
		uint32_t hash;
		std::string type;
		uint64_t bwreq;
		Time last_arrival;
		uint32_t threshold;
	}flow_table_t;

	typedef struct detnetactiveflow{
		flow_table_t *flow;
	}detnetactiveflow_t;

	typedef struct otheractiveflow{
			flow_table_t *flow;
		}otheractiveflow_t;

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief BwResvQueueDisc Constructor
   *
   * Create a BwResv queue disc
   */
  BwResvQueueDisc ();

  /**
   * \brief Destructor
   *
   * Destructor
   */
  virtual ~BwResvQueueDisc ();

  /**
    * \brief Set the size of the first bucket in bytes.
    *
    * \param burst The size of first bucket in bytes.
    */
  void SetSizeDetnet (uint32_t sizedetnet);

  /**
    * \brief Get the size of the first bucket in bytes.
    *
    * \returns The size of the first bucket in bytes.
    */
  uint32_t GetSizeDetnet (void) const;

  /**
    * \brief Set the size of the second bucket in bytes.
    *
    * \param mtu The size of second bucket in bytes.
    */
  void SetSizeOther (uint32_t sizeother);

  /**
    * \brief Get the size of the second bucket in bytes.
    *
    * \returns The size of the second bucket in bytes.
    */
  uint32_t GetSizeOther (void) const;

  /**
    * \brief Set the rate of the tokens entering the first bucket.
    *
    * \param rate The rate of first bucket tokens.
    */
  void SetRateDetnet (DataRate ratedetnet);

  /**
    * \brief Get the rate of the tokens entering the first bucket.
    *
    * \returns The rate of first bucket tokens.
    */
  DataRate GetRateDetnet (void) const;

  /**
    * \brief Set the rate of the tokens entering the second bucket.
    *
    * \param peakRate The rate of second bucket tokens.
    */
  void SetRateOther (DataRate rateother);

  /**
    * \brief Get the rate of the tokens entering the second bucket.
    *
    * \returns The rate of second bucket tokens.
    */
  DataRate GetRateOther (void) const;

  /**
    * \brief Get the current number of tokens inside the first bucket in bytes.
    *
    * \returns The number of first bucket tokens in bytes.
    */
  uint32_t GetTokensDetnet (void) const;

  /**
    * \brief Get the current number of tokens inside the second bucket in bytes.
    *
    * \returns The number of second bucket tokens in bytes.
    */
  uint32_t GetTokensOther (void) const;

  void AddDetnetActiveFlow(detnetactiveflow_t flow);
  detnetactiveflow_t RemoveDetnetActiveFlow(void);
  void AddOtherActiveFlow(otheractiveflow_t flow);
  otheractiveflow_t RemoveOtherActiveFlow(void);
  flow_table_t *ClassifyFlow(Ptr<QueueDiscItem> item,uint32_t hash,uint32_t pktsize);


  // Reasons for dropping packets
  static constexpr const char* UNFORCED_DROP = "Unforced drop";  //!< Early probability drops
  static constexpr const char* FORCED_DROP = "Forced drop";      //!< Forced drops, m_qAvg > m_maxTh
  // Reasons for marking packets
  static constexpr const char* UNFORCED_MARK = "Unforced mark";  //!< Early probability marks
  static constexpr const char* FORCED_MARK = "Forced mark";      //!< Forced marks, m_qAvg > m_maxTh

  uint32_t flow_detnet;
  uint32_t threshold;
  uint32_t detnet_active;
  uint32_t other_active;
  uint32_t num;



protected:
  /**
   * \brief Dispose of the object
   */
  virtual void DoDispose (void);

private:

  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  virtual bool CheckConfig (void);
  virtual void InitializeParams (void);

  /* parameters for the TBF Queue Disc */
  uint32_t m_sizedetnet;      //!< Size of first bucket in bytes
  uint32_t m_sizeother;        //!< Size of second bucket in bytes
  DataRate m_ratedetnet;       //!< Rate at which tokens enter the first bucket
  DataRate m_rateother;   //!< Rate at which tokens enter the second bucket

  /* variables stored by BwResv Queue Disc */
  TracedValue<uint32_t> m_detnet_tokens; //!< Current number of tokens in first bucket
  TracedValue<uint32_t> m_other_tokens; //!< Current number of tokens in second bucket
  Time m_timeCheckPoint_detnet;           //!< Time check-point
  Time m_timeCheckPoint_other;           //!< Time check-point
  Time m_timeCheckPoint;           //!< Time check-point
  uint32_t nbl_detnet;
  uint32_t nbl_other;
  EventId m_id;                    //!< EventId of the scheduled queue waking event when enough tokens are available
  uint32_t batch;
  uint32_t detnet_consumption;
  #define TABLESIZE 1024
#define DETNET 0
#define OTHER 1
  std::vector<flow_table_t>flow_table[TABLESIZE];
  std::vector<detnetactiveflow_t>detnetactiveflows;
  std::vector<otheractiveflow_t>otheractiveflows;
};

} // namespace ns3

#endif /* BW_RESV_QUEUE_DISC_H */
