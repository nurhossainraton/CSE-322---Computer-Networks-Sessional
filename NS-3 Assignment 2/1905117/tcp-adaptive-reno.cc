#include "tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpAdaptiveReno>()
   
  ;
  return tid;
}

TcpAdaptiveReno::~TcpAdaptiveReno (void)
{
}

TcpAdaptiveReno::TcpAdaptiveReno (void) :
  TcpWestwoodPlus(),
  m_minRtt (Time (0)),
  m_currentRtt (Time (0)),
  m_jPacketLRtt (Time (0)),
  m_conjRtt (Time (0)),
  m_prevConjRtt (Time(0)),
  m_incWnd (0),
  m_baseWnd (0),
  m_probeWnd (0)
{
  NS_LOG_FUNCTION (this);
}

TcpAdaptiveReno::TcpAdaptiveReno (const TcpAdaptiveReno& sk) :
  TcpWestwoodPlus (sk),
  m_minRtt (Time (0)),
  m_currentRtt (Time (0)),
  m_jPacketLRtt (Time (0)),
  m_conjRtt (Time (0)),
  m_prevConjRtt (Time(0)),
  m_incWnd (0),
  m_baseWnd (0),
  m_probeWnd (0)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}



/*
The function is called every time an ACK is received (only one time
also for cumulative ACKs) and contains timing information
*/
void
TcpAdaptiveReno::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                        const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << packetsAcked << rtt);

  if (rtt.IsZero ())
    {
      NS_LOG_WARN ("RTT measured is zero!");
      return;
    }

  m_ackedSegments += packetsAcked;

  // calculate min rtt here
  if(m_minRtt.IsZero()) { m_minRtt = rtt; }
  else if(rtt <= m_minRtt) { m_minRtt = rtt; }

  m_currentRtt = rtt;
  // NS_LOG_LOGIC("Min RTT is : "<<m_minRtt.GetMilliSeconds() << "ms");
  // NS_LOG_LOGIC ("CurRtt: " << m_currentRtt.GetMilliSeconds () << "ms");

  TcpWestwoodPlus::EstimateBW (rtt, tcb);
}


double
TcpAdaptiveReno::EstimateCongestionLevel()
{

  float m_a = 0.85; // exponential smoothing factor
  if(m_prevConjRtt < m_minRtt) m_a = 0; // the initial value should take the full current Jth loss Rtt
  
  double conjRtt = m_a*m_prevConjRtt.GetSeconds() + (1-m_a)*m_jPacketLRtt.GetSeconds(); // TODO : do i need to reset m_maxRtt value to this
  m_conjRtt = Seconds(conjRtt); // for next step calcu
  //NS_LOG_LOGIC("conj rtt : " << m_conjRtt << " ; m_prevConjRtt : " << m_prevConjRtt << " ; j rtt : " << m_jPacketLRtt);

  return std::min(
    (m_currentRtt.GetSeconds() - m_minRtt.GetSeconds()) / (conjRtt - m_minRtt.GetSeconds()),
    1.0
  );
}


void 
TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
{
  

  double congestion = EstimateCongestionLevel();
  int scalingFactor_m = 1000; 
  
  // m_currentBW; -> already calculated in packetsAck
  double m_maxIncWnd = static_cast<uint32_t> (m_currentBW.Get().GetBitRate()) / scalingFactor_m * static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) ; 

  double alpha = 10; 
  double beta = 2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 1)/(std::exp(alpha))));
  double gamma = 1 - (2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 0.5)/(std::exp(alpha)))));

  m_incWnd = (int)((m_maxIncWnd / std::exp(alpha * congestion)) + (beta * congestion) + gamma);


}


void
TcpAdaptiveReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  

  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (segmentsAcked > 0)
    {
      EstimateIncWnd(tcb);
      // base_window = USE NEW RENO IMPLEMENTATION
      double adder = static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get ();
      adder = std::max (1.0, adder);
      m_baseWnd += static_cast<uint32_t> (adder);

      // change probe window
      m_probeWnd = std::max(
        (double) (m_probeWnd + m_incWnd / (int)tcb->m_cWnd.Get()), 
        (double) 0
      );
      
      NS_LOG_LOGIC("Before "<<tcb->m_cWnd<< " ; base "<<m_baseWnd<<" ; probe "<<m_probeWnd);
      tcb->m_cWnd = m_baseWnd + m_probeWnd;
      // NS_LOG_UNCOND("Congestion window changed");

      NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
                   " ssthresh " << tcb->m_ssThresh);
    }

}

uint32_t
TcpAdaptiveReno::GetSsThresh (Ptr<const TcpSocketState> tcb,
                          uint32_t bytesInFlight)
{
 
  m_prevConjRtt = m_conjRtt; // a loss event has occured. so set the previous conjestion RTT
  m_jPacketLRtt = m_currentRtt; // this will now contain the RTT of previous packet or jth loss event
  
  double congestion = EstimateCongestionLevel();
  uint32_t ssthresh = std::max (
    2*tcb->m_segmentSize,
    (uint32_t) (tcb->m_cWnd / (1.0+congestion))
  );

  // reset calculations
  m_baseWnd = ssthresh;
  m_probeWnd = 0;
  
  NS_LOG_LOGIC("new ssthresh : "<<ssthresh<<" ; old conj Rtt : "<<m_prevConjRtt<<" ; new conj Rtt : "<<m_conjRtt<<" ; cong : "<<congestion);
  
  return ssthresh;

}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork ()
{
  return CreateObject<TcpAdaptiveReno> (*this);
}

}