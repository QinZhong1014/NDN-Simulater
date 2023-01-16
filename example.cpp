
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"

#include "ns3/ndnSIM-module.h"

#include "ns3/point-to-point-module.h"

/////////////////
#include  "ns3/point-to-point-grid.h"
#include  "ns3/ndnSIM-module.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
///////////////

#include "ns3/netanim-module.h"
#include "ns3/constant-velocity-mobility-model.h"

#include "ns3/applications-module.h"

#include "ns3/address.h"
#include "ns3/udp-echo-helper.h"

#include "ns3/flow-monitor-helper.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <pthread.h>
//////////////////////////
#include <algorithm>
///////////////////////////

//#include "mqtt_pub_sub.h"

//NS_LOG_COMPONENT_DEFINE ("simple-wifi-mobility");
NS_LOG_COMPONENT_DEFINE ("example");

using namespace std;

//NS_LOG_COMPONENT_DEFINE ("example")

namespace ns3 {

  NetDeviceContainer PtoPdevices;

  class MqttPubSubVehicleModel 
  {
  public:

    MqttPubSubVehicleModel() : topologyReader("", 9),consumerHelper("ns3::ndn::ConsumerCbr"),producerHelper("ns3::ndn::Producer") {};
    virtual ~MqttPubSubVehicleModel();
 
    CommandLine cmd;     

    AnnotatedTopologyReader topologyReader;
   
    NodeContainer producer;
    NodeContainer wifiApNodes;
    NodeContainer mqttproxy;
    NodeContainer routers;

    NodeContainer PtoPContainer;

    std::string phyMode;
    int range;
    Ssid ssid;

    WifiHelper wifi;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    WifiMacHelper  wifiMacHelperIns;

    NodeContainer consumers;

    static NodeContainer *Ptr_consumers;

    EventId Simulation_EventID;
    static pthread_t VehicleMoving_ThreadID;

    NetDeviceContainer staDevice;
    WifiMacHelper wifiMac;
    uint32_t wifiSta;
    int bottomrow;
    MobilityHelper mobile; 
    double endtime;
   
    ndn::StackHelper ndnHelper;

    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndn::AppHelper consumerHelper;
    ndn::AppHelper producerHelper;

    //NetDeviceContainer PtoPdevices;
/*
    PointToPointHelper PtoPhelperInstance;
    InternetStackHelper InternetStackhelperInstance;
    Ipv4AddressHelper  PtoPIPAddressPool;
    Ipv4InterfaceContainer PtoPDevinterfaces;
    UdpEchoServerHelper PtoPechoServerHelper;
    //UdpEchoClientHelper PtoPechoClientHelper;
    ApplicationContainer PtoPserverApps;
*/
    void Command_Parse(int argc, char *argv[]);

    void AnnotateTopologyRead();

    void TopologyParasetting();

    void Topologybasesetting();

    void TopologyWirelessInstall();

    static void * TopologySimulaterTrail(void * lpparam);

    static void * TopologyWireStaStimulateMovingEventStop(void * lpparam);

    void TopologyWireStaStimulateMoving();

    void TopologyProducerPolicyInstall();

    void TopologyMqttProxyPolicyInstall();

    void TopologyCoreRouterPolicyInstall();

    void TopologyEdgerouterPolicyInstall();

    void TopologyRouterPolicyInstall();

    void TopologyMobileVehiclePolicyInstall();

    void TopologyNdnSimulating();

    void PtoPDevicesAssignAdress();

    void AssignIpv4Adress(Ptr<NetDevice> device,uint u32Addr, Ipv4Mask u32Mask);

    //void TopologyBrokerToMqttProxyPointToPointflowcommunication();

    static void * TopologyBrokerToMqttProxyPointToPointflowcommunication(void * lpparam);

    void TopologyBrokerToMqttProxyPointToPointHandle();

  private:
    //static AnnotatedTopologyReader topologyReader("", 9);

  };

  NodeContainer  * MqttPubSubVehicleModel::Ptr_consumers = NULL;

  pthread_t MqttPubSubVehicleModel::VehicleMoving_ThreadID = 0;


  MqttPubSubVehicleModel::~MqttPubSubVehicleModel()
  {

  }

  void MqttPubSubVehicleModel::Command_Parse(int argc, char *argv[])
  {
    string animFile = "ap-mobility-animation.xml";
    cmd.AddValue ("animFile", "File Name for Animation Output", animFile);
    cmd.Parse (argc, argv);
    return;
  }

  
  void MqttPubSubVehicleModel::AnnotateTopologyRead()
  {
    topologyReader.SetFileName("/home/work/work_project/ndnSIM/scenario/scenarios/x-topo.txt");
    topologyReader.Read();

    topologyReader.SaveGraphviz("/home/work/work_project/ndnSIM/scenario/scenarios/GeantGraph.dot");

    producer.Add(Names::Find<Node>("broker"));

    mqttproxy.Add(Names::Find<Node>("r5"));
    
    wifiApNodes.Add(Names::Find<Node>("ap1"));
    wifiApNodes.Add(Names::Find<Node>("ap2"));

    routers.Add(Names::Find<Node>("r1"));
    routers.Add(Names::Find<Node>("r2"));
    routers.Add(Names::Find<Node>("r3"));
    routers.Add(Names::Find<Node>("r4"));

    PtoPContainer.Add(Names::Find<Node>("broker"));
    PtoPContainer.Add(Names::Find<Node>("r5"));

    return;
  }


   void MqttPubSubVehicleModel::TopologyParasetting()
   {
     phyMode = "DsssRate1Mbps";
     range = 110;
     wifiSta = 12;
     bottomrow = 2;
     endtime = 20.0;
     
     return;
   }
   void MqttPubSubVehicleModel::Topologybasesetting()
   {
      
      Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
      Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
      Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

      Config::SetDefault("ns3::PointToPointNetDevice::DataRate", ns3::StringValue("10Mbps"));
      Config::SetDefault("ns3::PointToPointChannel::Delay", ns3::StringValue("10ms"));
      // ns3::Config::SetDefault("ns3::DropTailQueue::MaxPackets", ns3::StringValue("2"));
      Config::SetDefault("ns3::QueueBase::MaxSize",ns3::QueueSizeValue(ns3::QueueSize(ns3::QueueSizeUnit::PACKETS,20)));

      return;

   }

  void MqttPubSubVehicleModel::TopologyWirelessInstall()
  {
   // wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
    wifiPhy = YansWifiPhyHelper::Default ();

    ////// This is one parameter that matters when using FixedRssLossModel
    ////// set it to zero; otherwise, gain will be added
    // wifiPhy.Set ("RxGain", DoubleValue (0) );

    ////// ns-3 supports RadioTap and Prism tracing extensions for 802.11b
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    // wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

    ////// The below FixedRssLossModel will cause the rss to be fixed regardless
    ////// of the distance between the two stations, and the transmit power
    // wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue(rss));

    ////// the following has an absolute cutoff at distance > range (range == radius)
    wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", 
                                    "MaxRange", DoubleValue(range));
    wifiPhy.SetChannel (wifiChannel.Create ());
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                  "DataMode", StringValue (phyMode),
                                  "ControlMode", StringValue (phyMode));

    ////// Setup the rest of the upper mac
    ////// Setting SSID, optional. Modified net-device to get Bssid, mandatory for AP unicast
    ssid = Ssid ("wifi-default");
    // wifi.SetRemoteStationManager ("ns3::ArfWifiManager");

    ////// Add a non-QoS upper mac of STAs, and disable rate control
    //NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();
    ////// Active associsation of STA to AP via probing.
    wifiMacHelperIns.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid),
                           "ActiveProbing", BooleanValue (true),
                           "ProbeRequestTimeout", TimeValue(Seconds(0.25)));

    ////// Creating 2 mobile nodes
   
    consumers.Create(wifiSta);

    staDevice = wifi.Install (wifiPhy, wifiMacHelperIns, consumers);
   // NetDeviceContainer devices = staDevice;

    ////// Setup AP.
    wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid),
                     "BeaconGeneration", BooleanValue(false));
   
    for (int i = 0; i < bottomrow; i++)
    {
        //NetDeviceContainer apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes[i]);
        NetDeviceContainer apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes.Get(i));
        staDevice.Add (apDevice);
    }    

    mobile.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobile.Install(consumers);
  
    return;
  }
  
  void MqttPubSubVehicleModel::TopologyProducerPolicyInstall()
  {
    ndnHelper.setCsSize(1000);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);

    ndnHelper.Install(producer);

    //NS_FATAL_ERROR("TopologyProducerPolicyInstall  successfully" );

    return;

  }

  void MqttPubSubVehicleModel::TopologyMqttProxyPolicyInstall()
  {
  
    ndnHelper.setCsSize(1000);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);


    ndnHelper.Install(mqttproxy);

   // cout << "TopologyMqttProxyPolicyInstall successfully" ;
     
    return;

  }

  void MqttPubSubVehicleModel::TopologyCoreRouterPolicyInstall()
  {
    
    int i;
  
    ndnHelper.setCsSize(1);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);
 
    ndnHelper.Install(routers); 

    return;
  }
  void MqttPubSubVehicleModel::TopologyEdgerouterPolicyInstall()
  {
    ndnHelper.setCsSize(1000);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);

    ndnHelper.Install(wifiApNodes); 
 
    return;
  }

  void MqttPubSubVehicleModel::TopologyMobileVehiclePolicyInstall()
  {
    //EdgeRouter_ndnHelper.setCsSize(1000);
    //EdgeRouter_ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);

    ndnHelper.Install(consumers);
  }

  void MqttPubSubVehicleModel::TopologyRouterPolicyInstall()
  {
    /*
    ndnHelper.setCsSize(1000);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.SetDefaultRoutes(true);
    //ndnHelper.SetDefaultRoutes(true);
    //ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
    ndnHelper.InstallAll();
    ndnHelper.Install(routers);

    return;
    */

    TopologyProducerPolicyInstall();

    TopologyMqttProxyPolicyInstall();

    TopologyCoreRouterPolicyInstall();

    TopologyEdgerouterPolicyInstall();

    TopologyMobileVehiclePolicyInstall();

    return;
  }
  
  void MqttPubSubVehicleModel::TopologyNdnSimulating()
  {
    // 3. Install NDN stack on all nodes
    NS_LOG_INFO("Installing NDN stack");
    
    //ndnHelper.InstallAll();
    //ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
  
    // Choosing forwarding strategy
    ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");
    //ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

    // Installing global routing interface on all nodes
    ndnGlobalRoutingHelper.InstallAll();

    // 4. Set up applications
    NS_LOG_INFO("Installing Applications");
    // Consumer Helpers
    //ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetPrefix("/root/prefix");
    //consumerHelper.SetPrefix("/test/prefix");
    consumerHelper.SetAttribute("Frequency", DoubleValue(10.0));

    for (uint32_t sta=0; sta<wifiSta ; sta++) {

        consumerHelper.Install(consumers.Get(sta)).Start(Seconds(0.0));
    }

    // Producer Helpers
    //ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    // Register /root prefix with global routing controller and
    // install producer that will satisfy Interests in /root namespace
    ndnGlobalRoutingHelper.AddOrigins("/root", producer);    
    producerHelper.SetPrefix("/root");
    producerHelper.Install(producer);

    // Calculate and install FIBs
    ndn::GlobalRoutingHelper::CalculateRoutes();   

    // Tracing
    wifiPhy.EnablePcap ("simple-wifi-mobility", staDevice);

    Simulator::Stop (Seconds (endtime));
//
    return;
  }

  void MqttPubSubVehicleModel::AssignIpv4Adress(Ptr<NetDevice> device,uint u32Addr, Ipv4Mask u32Mask)
  {
    Ptr<Node> node = device->GetNode();  

    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
  
    int32_t Interface = ipv4->GetInterfaceForDevice(device);
    if (Interface == -1)
    {
      Interface = ipv4->AddInterface(device);
    }
    Ipv4Address stAddr(u32Addr);
    
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress(stAddr,u32Mask);
    ipv4->AddAddress(Interface,ipv4Addr);
    ipv4->SetMetric(Interface,1);
    ipv4->SetUp(Interface);

    return;
  }
  void MqttPubSubVehicleModel::PtoPDevicesAssignAdress()
  {
    Ptr<NetDevice> device = PtoPdevices.Get(0);

    Ipv4Address stAddr = Ipv4Address("10.1.1.1");

    uint32_t u32Addr = stAddr.Get();
    Ipv4Mask stMask = Ipv4Mask("255.255.255.0");

    uint32_t u32Mask = stMask.Get();

    AssignIpv4Adress(device,u32Addr, stMask);
    device = PtoPdevices.Get(1);
    AssignIpv4Adress(device,u32Addr+1, stMask);
    
    return;
  }

/*
  
  void MqttPubSubVehicleModel::TopologyBrokerToMqttProxyPointToPointflowcommunication()
  {
    PointToPointHelper PtoPhelperInstance;
    InternetStackHelper InternetStackhelperInstance;
    Ipv4AddressHelper  PtoPIPAddressPool;
    Ipv4InterfaceContainer PtoPDevinterfaces;
    Address ip;
    uint16_t port=9;
    UdpEchoServerHelper PtoPechoServerHelper(port);
    
    UdpEchoClientHelper PtoPechoClientHelper(ip,port);
    ApplicationContainer PtoPserverApps;

    //MqttPubSubVehicleModel* pThreadThis = (MqttPubSubVehicleModel*) lpparam;


    // NodeContainer PtoPContainer;

    NS_LOG_INFO("Point to Point link is going to be established \n");

    PtoPhelperInstance.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    PtoPhelperInstance.SetChannelAttribute ("Delay", StringValue ("2ms"));

    //NetDeviceContainer PtoPdevices;
    PtoPdevices = PtoPhelperInstance.Install (PtoPContainer);

    InternetStackhelperInstance.Install(PtoPContainer);
  
    //PtoPIPAddressPool.SetBase ("10.1.1.0", "255.255.255.0");

    //PtoPDevinterfaces = PtoPIPAddressPool.Assign(PtoPdevices);

    PtoPDevicesAssignAdress();

    //PtoPechoServerHelper.SetAttribute("Port",UintegerValue(9));    

    //PtoPhelperInstance.EnablePcap("Myptoptest",PtoPContainer);

    PtoPserverApps = PtoPechoServerHelper.Install(PtoPContainer.Get(0));

    AnimationInterface anim("first.xml");

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;

    flowMonitor = flowHelper.Install(PtoPContainer);

    flowMonitor->SerializeToXmlFile("Mytest.xml",true,true);

    PtoPserverApps.Start (Seconds (1.0));
    PtoPserverApps.Stop (Seconds (10.0));

    NS_LOG_INFO("Server Application is starting; \n");

    Address Temp_addr;

    //UdpEchoClientHelper PtoPechoClientHelper(AddressValue(PtoPDevinterfaces.GetAddress(0)),UintegerValue(9));
    //UdpEchoClientHelper PtoPechoClientHelper(PtoPDevinterfaces.GetAddress(1),UintegerValue(9));

    Ipv4Address stAddr = Ipv4Address("10.1.1.1");   

    PtoPechoClientHelper.SetAttribute("RemoteAddress",AddressValue(Address(stAddr)));
    PtoPechoClientHelper.SetAttribute("RemotePort",UintegerValue(9));
    PtoPechoClientHelper.SetAttribute("MaxPackets", UintegerValue (2));
    PtoPechoClientHelper.SetAttribute("Interval", TimeValue (Seconds (1.0)));
    PtoPechoClientHelper.SetAttribute("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = PtoPechoClientHelper.Install (PtoPContainer.Get(1));

    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    NS_LOG_INFO("Client Application is starting; \n");
    
    return;
  }
*/
  void * MqttPubSubVehicleModel::TopologyBrokerToMqttProxyPointToPointflowcommunication(void * lpparam) 
  {
    PointToPointHelper PtoPhelperInstance;
    InternetStackHelper InternetStackhelperInstance;
    Ipv4AddressHelper  PtoPIPAddressPool;
    Ipv4InterfaceContainer PtoPDevinterfaces;
    Address ip;
    uint16_t port=9;
    UdpEchoServerHelper PtoPechoServerHelper(port);
    
    UdpEchoClientHelper PtoPechoClientHelper(ip,port);
    ApplicationContainer PtoPserverApps;

    MqttPubSubVehicleModel* pThreadThis = (MqttPubSubVehicleModel*) lpparam;


    // NodeContainer PtoPContainer;

    NS_LOG_INFO("Point to Point link is going to be established \n");

    PtoPhelperInstance.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    PtoPhelperInstance.SetChannelAttribute ("Delay", StringValue ("2ms"));

    //NetDeviceContainer PtoPdevices;
    PtoPdevices = PtoPhelperInstance.Install (pThreadThis->PtoPContainer);

    InternetStackhelperInstance.Install(pThreadThis->PtoPContainer);
  
    //PtoPIPAddressPool.SetBase ("10.1.1.0", "255.255.255.0");
    //PtoPDevinterfaces = PtoPIPAddressPool.Assign(PtoPdevices);

    pThreadThis->PtoPDevicesAssignAdress();

    PtoPserverApps = PtoPechoServerHelper.Install(pThreadThis->PtoPContainer.Get(0));

    AnimationInterface anim("first.xml");

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;

    flowMonitor = flowHelper.Install(pThreadThis->PtoPContainer);

    flowMonitor->SerializeToXmlFile("Mytest.xml",true,true);

    PtoPserverApps.Start (Seconds (1.0));
    PtoPserverApps.Stop (Seconds (10.0));

    NS_LOG_INFO("Server Application is starting; \n");

    Address Temp_addr;

    Ipv4Address stAddr = Ipv4Address("10.1.1.1");   

    PtoPechoClientHelper.SetAttribute("RemoteAddress",AddressValue(Address(stAddr)));
    PtoPechoClientHelper.SetAttribute("RemotePort",UintegerValue(9));
    PtoPechoClientHelper.SetAttribute("MaxPackets", UintegerValue (2));
    PtoPechoClientHelper.SetAttribute("Interval", TimeValue (Seconds (1.0)));
    PtoPechoClientHelper.SetAttribute("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = PtoPechoClientHelper.Install (pThreadThis->PtoPContainer.Get(1));

    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    NS_LOG_INFO("Client Application is starting; \n");
    sleep(5);
    
    return NULL;
  }

  void MqttPubSubVehicleModel::TopologyBrokerToMqttProxyPointToPointHandle()
  {
    int ret = 0;
    pthread_t ThreadId=0;
       
    NS_LOG_INFO("Hello this is going to Broker and Proxy Point to Point communication thread!");

   // TopologyBrokerToMqttProxyPointToPointflowcommunication();

    ret = pthread_create(&ThreadId,NULL,TopologyBrokerToMqttProxyPointToPointflowcommunication,(void *)this);
    if (ret == 0)
    {
        NS_LOG_INFO("Creating Broker and Proxy Point to Point communication thread successfully!");
        pthread_join(ThreadId,NULL);
        //pthread_detach(ThreadId);
    }
    else{
        NS_LOG_INFO("Creating Broker and Proxy Point to Point communication failed!");
        return;
    }
    
    return;
  }



  void * MqttPubSubVehicleModel::TopologySimulaterTrail(void * lpparam)
  {
    int nxt = 0;
    double sim_endtime = 20.0;
    int sim_bottomrow = 2;
    double speed;
    double speed_reverse; 
    int sim_spacing=200;

    speed = (double)(sim_bottomrow*sim_spacing)/sim_endtime; //setting speed to span full sim time   
    speed_reverse = (double)((-1)*sim_bottomrow*sim_spacing)/sim_endtime; 

    MqttPubSubVehicleModel* pThreadThis = (MqttPubSubVehicleModel*) lpparam;

    NodeContainer consumers;

    NS_LOG_INFO("It is going to start the vehicles moving on action!");

    consumers = pThreadThis->consumers;
     

    for (uint32_t i=0; i<2 ; i++) {
     // NS_LOG_INFO("The left to right begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm1 = consumers.Get(i)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (-50-nxt, 10, 0);
        Vector vel (speed, 0, 0);
        cvmm1->SetPosition(pos);
        cvmm1->SetVelocity(vel);
        nxt += 200;
    }
    
    nxt = 0;
    for (uint32_t i_down=2; i_down<4; i_down++) {
     // NS_LOG_INFO("The left up to down begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm2 = consumers.Get(i_down)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (0-nxt, -200, 0);
        Vector vel (0, speed, 0);
        cvmm2->SetPosition(pos);
        cvmm2->SetVelocity(vel);
        nxt += 50;
    }
    nxt = 0;
    for (uint32_t i_up=4; i_up<6 ; i_up++) {
     // NS_LOG_INFO("The left down to up begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm3 = consumers.Get(i_up)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (0-nxt, 500, 0);
        Vector vel (0, speed_reverse, 0);
        cvmm3->SetPosition(pos);
        cvmm3->SetVelocity(vel);
        nxt += 50;
    }

    nxt = 0;
    for (uint32_t j=6; j<8 ; j++) {
     // NS_LOG_INFO("The right to left begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm4 = consumers.Get(j)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (600-nxt, -10, 0);
        Vector vel (speed_reverse, 0, 0);
        cvmm4->SetPosition(pos);
        cvmm4->SetVelocity(vel);
        nxt += 200;
    }
    nxt = 0;
    for (uint32_t j_down=8; j_down<10 ; j_down++) {
     // NS_LOG_INFO("The right up to down begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm5 = consumers.Get(j_down)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (150-nxt, -300, 0);
        Vector vel (0, speed, 0);
        cvmm5->SetPosition(pos);
        cvmm5->SetVelocity(vel);
        nxt += 50;
    }
    nxt = 0;
    for (uint32_t j_up=10; j_up<12 ; j_up++) {
     // NS_LOG_INFO("The right down to up begin to move! \n");
        Ptr<ConstantVelocityMobilityModel> cvmm6 = consumers.Get(j_up)->GetObject<ConstantVelocityMobilityModel> ();
        Vector pos (150-nxt, 700, 0);
        Vector vel (0, speed_reverse, 0);
        cvmm6->SetPosition(pos);
        cvmm6->SetVelocity(vel);
        nxt += 50;
    }

   // sleep(1);
    
    pthread_exit(NULL);
    return NULL;
  }
  
  

  void * MqttPubSubVehicleModel::TopologyWireStaStimulateMovingEventStop(void * lpparam)
  {
    
    int ret = 0;
    EventId Temple_EventID;

    std::string message = "Cancel of Vehicle moving thread failed!";
    std::stringstream error_no;

    char returnvalue[5];


    pthread_t ThreadId=0;

    NS_LOG_INFO("This is going to cancel the vehicle moving Event!");
    MqttPubSubVehicleModel* pThreadThis = (MqttPubSubVehicleModel*) lpparam;
   
    //Simulator::Remove(pThreadThis->Simulation_EventID);

    //Simulator::ScheduleDestroy(pThreadThis->Simulation_EventID);

    ret = pthread_cancel(VehicleMoving_ThreadID);
    if (ret == 0)
    {
      NS_LOG_INFO("Cancel of Vehicle moving thread successfully!");

    }
    else{
      //int2str(ret,returnvalue);
      error_no << ret;
      //strcpy(returnvalue,itoa(ret));
      error_no >> message;
      //strcat(std::message,returnvalue);
      NS_LOG_INFO("Cancel of Vehicle moving thread failed! Ther error no is:");
      NS_LOG_INFO(message);

    }

    pthread_exit(NULL);
    return NULL;
  }

  void MqttPubSubVehicleModel::TopologyWireStaStimulateMoving()
  {
    int ret = 0;
    Ptr_consumers = &consumers;
    pthread_t ThreadId=0;

    //EventId Temple_EventID;

    //EventId *Ptr_EventID = NULL;

       
    NS_LOG_INFO("Hello this is going to start vehicles moving thread!");

    ret = pthread_create(&ThreadId,NULL,TopologySimulaterTrail,(void *)this);
    if (ret == 0)
    {
        VehicleMoving_ThreadID = ThreadId;
        NS_LOG_INFO("Creating Vehicle Moving thread successfully!");
        pthread_join(ThreadId,NULL);
        //pthread_detach(ThreadId);
    }
    else{
        NS_LOG_INFO("Creating Vehicle Moving thread failed!");
        return;
    }

    return;
  }

  int main (int argc, char *argv[])
  {
    std::string phyMode ("DsssRate1Mbps");
    uint32_t wifiSta = 6;
    uint32_t wifiSta_reverse = 6; // added for reverse vehicles
  /*
    int bottomrow = 2;            // number of AP nodes
    int spacing = 200;            // between bottom-row nodes
    int range = 110;
    double endtime = 20.0;
    double speed = (double)(bottomrow*spacing)/endtime; //setting speed to span full sim time 
    double speed_reverse = (double)((-1)*bottomrow*spacing)/endtime; //adder for reverse vehicles: setting speed to span full sim time 
   */
    MqttPubSubVehicleModel MqttModeExample;

    LogComponentEnable("example",LOG_INFO);

    LogComponentEnable("UdpEchoClientApplication",LOG_INFO);
    //LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication",LOG_INFO);


    NS_LOG_INFO("This is begin of simulation!");
    MqttModeExample.Command_Parse(argc, argv);

    MqttModeExample.AnnotateTopologyRead();
    
    MqttModeExample.TopologyParasetting();

    MqttModeExample.Topologybasesetting();

    MqttModeExample.TopologyWirelessInstall();
 

    MqttModeExample.TopologyRouterPolicyInstall();
  
    MqttModeExample.TopologyNdnSimulating();  

    //MqttModeExample.TopologyBrokerToMqttProxyPointToPointHandle();
    
    MqttModeExample.TopologyWireStaStimulateMoving(); 
      

    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
  }
}

int main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}