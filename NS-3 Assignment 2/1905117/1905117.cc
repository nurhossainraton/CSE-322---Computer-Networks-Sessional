
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/callback.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-module.h"
#include "tutorial-app.cc"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Task2");
class Myapplication : public Application
{
public:
  Myapplication ();
  virtual ~Myapplication ();
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket,  DataRate dataRate, uint32_t simultime,Address address, uint32_t packetSize);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void TxScheduling (void);
  void PacketSend (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  uint32_t        m_simultime;
};



Myapplication::~Myapplication ()
{
  m_socket = 0;
}


TypeId Myapplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("Myapplication")
    .SetParent<Application> ()
    .SetGroupName ("abc")
    .AddConstructor<Myapplication> ()
    ;
  return tid;
}

Myapplication::Myapplication ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0),
    m_simultime (0)
{
}

void
Myapplication::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
    if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  PacketSend ();
}

void
Myapplication::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
Myapplication::Setup (Ptr<Socket> socket,DataRate dataRate, uint32_t simultime,Address address, uint32_t packetSize)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
  m_simultime = simultime;
 
}

void
Myapplication::TxScheduling (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &Myapplication::PacketSend, this);
    }
}

void
Myapplication::PacketSend (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if(Simulator::Now().GetSeconds() < m_simultime) TxScheduling();
}



// static void
// CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
// {
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << endl;
// }

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << endl;
}



int main(int argc, char *argv[]){
    
    string tcp1 = "ns3::TcpNewReno"; // TcpNewReno
    string tcp2 = "ns3::TcpAdaptiveReno"; //TcpAdaptiveReno TcpWestwood, TcpHighSpeed
   
    string senderDataRate = "1Gbps", senderDelay = "1ms",output_folder = "task1";
    int simulationTime = 60, bttlnkRate = 50, bttlnkDelay = 100,clntime = 2 ;
    int packet_loss_power = 6, exp = 1, nLeaf = 2, nFlows = 2;
    
    uint32_t payloadSize = 1000;

    // input from CMD
    CommandLine cmd (__FILE__);
    cmd.AddValue ("bttlnkRate","Max Packets allowed in the device queue", bttlnkRate);
    cmd.AddValue ("plossRate", "Packet loss rate", packet_loss_power);
    cmd.AddValue ("exp","1 for bttlnck, 2 for packet loss rate", exp);
    cmd.AddValue ("tcp2","variant of tcp", tcp2);
    cmd.Parse (argc,argv);

    nFlows = nLeaf;
    string file = output_folder + "/data_" +  to_string(exp) +tcp2+ ".txt";
    double packet_loss_rate = (1.0 /  pow(10, packet_loss_power));
    string btlneckRate = to_string(bttlnkRate) + "Mbps";
    string bottleNeckDelay =  to_string(bttlnkDelay) + "ms";
   

    NS_LOG_UNCOND("USING TCP 1 = "<<tcp1<<" ; TCP 2 = "<<tcp2<<" ; nLeaf = "<<nLeaf<<
                  " ; bottleneck rate = "<<btlneckRate<<
                  " ; packet loss rate = "<<packet_loss_rate<<
                  " ; sender data rate = "<<senderDataRate);

    
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    PointToPointHelper bottleNeckLink;
    bottleNeckLink.SetDeviceAttribute  ("DataRate", StringValue (btlneckRate));
    bottleNeckLink.SetChannelAttribute ("Delay", StringValue (bottleNeckDelay));


    PointToPointHelper pointToPointLeaf;
    pointToPointLeaf.SetDeviceAttribute  ("DataRate", StringValue (senderDataRate));
    pointToPointLeaf.SetChannelAttribute ("Delay", StringValue (senderDelay));
    pointToPointLeaf.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue   (to_string (bttlnkDelay * bttlnkRate) + "p"));

    PointToPointDumbbellHelper dumbbel (nLeaf, pointToPointLeaf,
                                  nLeaf, pointToPointLeaf,
                                  bottleNeckLink);

    // error rate
    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (packet_loss_rate));
    dumbbel.m_routerDevices.Get(1)->SetAttribute ("ReceiveErrorModel", PointerValue (em)); 


    InternetStackHelper stack1,stack2;
    // Installing STACK for tcp1
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcp1));
    
    for (uint32_t i = 0; i < dumbbel.LeftCount (); i+=2)
      {
        stack1.Install (dumbbel.GetLeft (i)); 
      }
    for (uint32_t i = 0; i < dumbbel.RightCount (); i+=2)
      {
        stack1.Install (dumbbel.GetRight (i)); 
      }
    stack1.Install (dumbbel.GetLeft ());
    stack1.Install (dumbbel.GetRight ());

    // tcp variant 2
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcp2));
    for (uint32_t i = 1; i < dumbbel.LeftCount (); i+=2)
      {
        stack2.Install (dumbbel.GetLeft (i)); 
      }
    for (uint32_t i = 1; i < dumbbel.RightCount (); i+=2)
      {
        stack2.Install (dumbbel.GetRight (i)); 
      }
    dumbbel.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"), // left nodes
                          Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),  // right nodes
                          Ipv4AddressHelper ("10.3.1.0", "255.255.255.0")); // routers 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables (); // populate routing table
    
    FlowMonitorHelper flowmon;
    flowmon.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(clntime)));
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

    uint16_t port = 8080;
    double totRx =0,totTx = 0;
    Time simulationtime,startTime;
    double throughput,deliveryratio;
    string filename = "output2.dat";
    ofstream outfile(filename,ios::app);
    for(int i=0;i<nFlows; i++){
      
      ostringstream oss;
      Address sinkAddress (InetSocketAddress (dumbbel.GetRightIpv4Address (i), port));
      PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port));
      ApplicationContainer sinkApps = packetSinkHelper.Install (dumbbel.GetRight (i));
      sinkApps.Start (Seconds (0));
      sinkApps.Stop (Seconds (simulationTime+clntime));

      Ptr<Socket> tcpSocket = Socket::CreateSocket (dumbbel.GetLeft (i), TcpSocketFactory::GetTypeId ());
      Ptr<Myapplication> app = CreateObject<Myapplication> ();
      app->Setup (tcpSocket, DataRate (senderDataRate), simulationTime,sinkAddress, payloadSize);
      dumbbel.GetLeft (i)->AddApplication (app);
      app->SetStartTime (Seconds (1));
      app->SetStopTime (Seconds (simulationTime));

      
      oss << output_folder << "/flow" << i+1 <<tcp2<<  ".cwnd";
      AsciiTraceHelper asciiTraceHelper;
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (oss.str());
      tcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
    }
    
  
    Simulator::Stop (Seconds (simulationTime+clntime));
    Simulator::Run ();


    // flow monitor statistics
    int j = 0;
    float AvgThroughput = 0;
    float CurThroughputArr[] = {0, 0};


    ofstream MyFile(file, ios_base::app);

    uint32_t SentPackets = 0;
    uint32_t ReceivedPackets = 0;
    uint32_t LostPackets = 0;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    for (auto iter = stats.begin (); iter != stats.end (); ++iter) {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first); 
            // classifier returns FiveTuple in correspondance to a flowID

  
      //CurThroughput = iter->second.rxBytes * 8.0/((simulationTime+clntime)*1000);
      
      if(j%2 == 0) { CurThroughputArr[0] += iter->second.rxBytes; }
      if(j%2 == 1) { CurThroughputArr[1] += iter->second.rxBytes; }


      SentPackets = SentPackets +(iter->second.txPackets);
      ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
      LostPackets = LostPackets + (iter->second.lostPackets);
      j = j + 1;

    }

  
    CurThroughputArr[0] /= ((simulationTime + clntime)*1000);
    CurThroughputArr[1] /= ((simulationTime + clntime)*1000);
    AvgThroughput = CurThroughputArr[0] + CurThroughputArr[1];

    // cols : bttlneck rate | random packet loss | throughput1 | throughput2 
    MyFile << btlneckRate.substr(0, btlneckRate.length()-4) << " " << -1*packet_loss_power<< " " << CurThroughputArr[0] << " " << CurThroughputArr[1] << " " <<  endl;
  
    


    Simulator::Destroy ();

    return 0;
}