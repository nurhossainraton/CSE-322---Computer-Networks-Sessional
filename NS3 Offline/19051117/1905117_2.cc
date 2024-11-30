

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

#include <iostream>
#include <iomanip>
#include <map>

using namespace ns3;
using namespace std;


string exp_name = "wifi";
// calculate metrics
double totRx =0,totTx = 0;
Time simulationtime,startTime;
double throughput,deliveryratio;

 static void throughputCalc(Ptr<const Packet> packet, const Address &address)
{
     totRx += packet->GetSize()*8;
     simulationtime =Now();
     //cout<<"my name is "<<endl;

}

static void OnOffTxCalc(Ptr<const Packet> packet)
{
  totTx += packet->GetSize()*8;
 
}

NS_LOG_COMPONENT_DEFINE ("Offline");

int main (int argc, char *argv[])
{
  
  uint16_t port = 5001;
  bool verbose = true;
  // VARIABLE PARAMETERSp
  
  
  
  

  uint32_t nWifi = 40;
  uint32_t num_flow = 20;
  uint32_t nPackets = 200;
  uint32_t speed = 10;
  uint32_t coverageArea = 2;
  string appDataRate = "10Mbps";
  uint32_t packetSize = 500;
    
  uint32_t txRange = 5;
  

  exp_name += "-nwifi-" + to_string(nWifi); // node
  exp_name += "-nflow-" + to_string(num_flow); // flow
  exp_name += "-app-" + appDataRate; // data rate
  exp_name += "-speed-" + to_string(speed); // range

  cout<<"exp name is" << exp_name <<endl;


  CommandLine cmd (__FILE__);
 
  cmd.AddValue ("appDataRate", "Set OnOff App DataRate", appDataRate);

  cmd.AddValue("nWifi", "Number of wifi dynamic devices", nWifi);
  cmd.AddValue("num_flow", "Number of Flows", num_flow);
  cmd.AddValue("nPackets", "Number of packets per second", nPackets);
  cmd.AddValue("speed", "Speed of nodes", speed);
  cmd.AddValue("coverageArea", "Coverage area of static wifi nodes", coverageArea);

  cmd.Parse (argc,argv);

   if(verbose)
    {
        LogComponentEnable("PacketSink",LOG_LEVEL_INFO);
        // LogComponentEnable("OnOffApplication",LOG_LEVEL_INFO);
    }
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (1024));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (appDataRate));


  
  /**
   * @brief 2 point to point nodes, which later will work as AP nodes for wifi
   * 
   */

  NodeContainer p2pnodes;
  p2pnodes.Create (2);

  // Create the point-to-point link helpers
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("50ms"));

  NetDeviceContainer p2pBottleNeckDevices;
  p2pBottleNeckDevices = pointToPoint.Install (p2pnodes);

  /**
   * @brief wifi left + right
   * 
   */

  NodeContainer wifiStaNodesLeft,wifiStaNodesRight;
  wifiStaNodesLeft.Create (nWifi);
  wifiStaNodesRight.Create (nWifi);
  NodeContainer wifiApNodeLeft = p2pnodes.Get (0);
  NodeContainer wifiApNodeRight = p2pnodes.Get (1);

  // PHY helper
  // constructs the wifi devices and the interconnection channel between these wifi nodes.
  YansWifiChannelHelper channelLeft = YansWifiChannelHelper::Default ();
  YansWifiChannelHelper channelRight = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phyLeft,phyRight;
  phyLeft.SetChannel (channelLeft.Create ()); //  all the PHY layer objects created by the YansWifiPhyHelper share the same underlying channel
  phyRight.SetChannel (channelRight.Create ()); 

  // MAC layer
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager"); // tells the helper the type of rate control algorithm to use, here AARF algorithm

  WifiMacHelper macLeft,macRight;
  Ssid ssidLeft = Ssid ("ns-3-ssid-left"); //  creates an 802.11 service set identifier (SSID) object
  Ssid ssidRight = Ssid ("ns-3-ssid-right"); 
  
  // configure Wi-Fi for all of our STA nodes
  macLeft.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssidLeft),
               "ActiveProbing", BooleanValue (false)); 
               
  macRight.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssidRight),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevicesLeft, staDevicesRight;
  staDevicesLeft = wifi.Install (phyLeft, macLeft, wifiStaNodesLeft);
  staDevicesRight = wifi.Install (phyRight, macRight, wifiStaNodesRight);

  //  configure the AP (access point) node
  macLeft.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssidLeft));
  macRight.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssidRight));

  NetDeviceContainer apDevicesLeft, apDevicesRight;
  apDevicesLeft = wifi.Install (phyLeft, macLeft, wifiApNodeLeft); // single AP which shares the same set of PHY-level Attributes (and channel) as the station
  apDevicesRight = wifi.Install (phyRight, macRight, wifiApNodeRight); 


 

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (0.5),
                                 "DeltaY", DoubleValue (1.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  
mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant="+std::to_string(speed)+"]"));
  
  // install on STA nodes
  mobility.Install (wifiStaNodesLeft);
  mobility.Install (wifiStaNodesRight);

  // tell AP node to stay still
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // install on AP node
  mobility.Install (wifiApNodeLeft);
  mobility.Install (wifiApNodeRight);// tell AP node to stay still


  // Install Stack
  InternetStackHelper stack;
  stack.Install (wifiApNodeLeft);
  stack.Install (wifiApNodeRight);
  stack.Install (wifiStaNodesLeft);
  stack.Install (wifiStaNodesRight);

  // Assign IP Addresses
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pBottleNeckDevices);

  Ipv4InterfaceContainer staNodeInterfacesLeft, staNodeInterfacesRight;
  Ipv4InterfaceContainer apNodeInterfaceLeft, apNodeInterfaceRight;

  address.SetBase ("10.1.2.0", "255.255.255.0");
  staNodeInterfacesLeft = address.Assign (staDevicesLeft);
  apNodeInterfaceLeft = address.Assign (apDevicesLeft);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  staNodeInterfacesRight = address.Assign (staDevicesRight);
  apNodeInterfaceRight = address.Assign (apDevicesRight);


  // Install on/off app on all right side nodes
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
 
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);

  ApplicationContainer sinkApps;
  for (uint32_t i = 0; i < nWifi; ++i)
    {
      // create sink app on left side node
      sinkApps.Add (packetSinkHelper.Install (wifiStaNodesRight.Get(i)));
    }
  sinkApps.Start (Seconds (1.0));
  sinkApps.Stop (Seconds (6.5));

 

  ApplicationContainer clientApps;
  int cur_flow_count = 0;
  for (uint32_t i = 0; i < nWifi; ++i)
    {
      // Create an on/off app on left side node which sends packets to the right side
      AddressValue remoteAddress (InetSocketAddress (staNodeInterfacesRight.GetAddress(i), port));
      
      for(uint32_t j = 0; j < nWifi; ++j)
      {
        clientHelper.SetAttribute ("Remote", remoteAddress);
        clientApps.Add (clientHelper.Install (wifiStaNodesLeft.Get(j)));

        cur_flow_count++;
        if(cur_flow_count >= num_flow)
          break;
      }

      if(cur_flow_count >= num_flow)
          break;

  
    }
  clientApps.Start (Seconds (2.0)); // Start 1 second after sink
  

  clientApps.Stop (Seconds (6.0)); // Stop before the sink  
  
   for(uint32_t i =0;i<clientApps.GetN();i++)
  {
    Ptr <Application> app = clientApps.Get(i);
    Ptr<OnOffApplication> appOnOff = DynamicCast <OnOffApplication> (app);

    if(appOnOff)
    {
      appOnOff->TraceConnectWithoutContext("Tx",MakeCallback(&OnOffTxCalc));
    }
  }

  for(uint32_t i = 0;i<sinkApps.GetN();i++)
  {
    Ptr <Application> app = sinkApps.Get(i);
    Ptr<PacketSink> sinkapp = DynamicCast <PacketSink> (app);

    if(sinkapp)
    {
      sinkapp->TraceConnectWithoutContext("Rx",MakeCallback(&throughputCalc));
    }
  }
  

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (6.5)); // force stop,

  string dirToSave = "mkdir -p " + exp_name;
  string dirToDel = "rm -rf " + exp_name;
  if (system (dirToDel.c_str ()) == -1)
  {
      exit (1);
  }
  if (system (dirToSave.c_str ()) == -1)
  {
      exit (1);
  }



  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream (exp_name+"/red-wifi.tr"));
  pointToPoint.EnablePcapAll (exp_name+"/red-wifi");
  

  cout << "Running the simulation :( " <<exp_name<< endl;
  Simulator::Run ();

  
  throughput = totRx/(simulationtime.GetSeconds() - 2);
  deliveryratio = totRx/totTx;

  string filename = "output2.dat";
  ofstream outfile(filename,ios::app);
  if(!outfile)
  {
    cerr<<"Error in opeing file"<<endl;
    return 1;
  }
  

  outfile<<"Throughput is "<<throughput<<" ";
  outfile<<"Delivery Ratio "<<deliveryratio<<endl;

  outfile.close();

  
  cout << "Destroying the simulation" << endl;

  Simulator::Destroy ();
  return 0;
}
