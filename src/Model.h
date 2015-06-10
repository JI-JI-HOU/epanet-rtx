//
//  Model.h
//  epanet-rtx
//
//  Created by the EPANET-RTX Development Team
//  See README.md and license.txt for more information
//  

#ifndef epanet_rtx_Model_h
#define epanet_rtx_Model_h

#include <string>
#include <map>
#include <time.h>

#include <boost/foreach.hpp>

#include "rtxExceptions.h"
#include "Element.h"
#include "Node.h"
#include "Tank.h"
#include "Reservoir.h"
#include "Link.h"
#include "Pipe.h"
#include "Pump.h"
#include "Valve.h"
#include "Dma.h"
#include "PointRecord.h"
#include "Units.h"
#include "rtxMacros.h"


namespace RTX {
  
  /*!
   \class Model
   \brief A hydraulic / water quality model abstraction.
   
   Provides methods for simulation and storing/retrieving states and parameters, and accessing infrastructure elements
   
   \sa Element, Junction, Pipe
   
   */
  
  using std::vector;
  using std::string;
  
  class Model {
  public:
    RTX_SHARED_POINTER(Model);
    
    
    Model();
    virtual ~Model();
    virtual void initEngine() { };
    virtual void closeEngine() { };
    std::string name();
    void setName(std::string name);
    virtual void loadModelFromFile(const string& filename) throw(std::exception);
    string modelFile();
    virtual void overrideControls() throw(RtxException);
    void runSinglePeriod(time_t time);
    void runExtendedPeriod(time_t start, time_t end);
    void setStorage(PointRecord::_sp record);
    void setParameterSource(PointRecord::_sp record);
    
    bool shouldRunWaterQuality();
    void setShouldRunWaterQuality(bool run);
    
    // DMAs -- identified by boundary link sets (doesHaveFlowMeasure)
    void initDMAs();
    void setDmaShouldDetectClosedLinks(bool detect);
    bool dmaShouldDetectClosedLinks();
    void setDmaPipesToIgnore(vector<Pipe::_sp> ignorePipes);
    vector<Pipe::_sp> dmaPipesToIgnore();
    
    // element accessors
    void addJunction(Junction::_sp newJunction);
    void addTank(Tank::_sp newTank);
    void addReservoir(Reservoir::_sp newReservoir);
    void addPipe(Pipe::_sp newPipe);
    void addPump(Pump::_sp newPump);
    void addValve(Valve::_sp newValve);
    void addDma(Dma::_sp dma);
    Link::_sp linkWithName(const string& name);
    Node::_sp nodeWithName(const string& name);
    vector<Element::_sp> elements();
    vector<Dma::_sp> dmas();
    vector<Node::_sp> nodes();
    vector<Link::_sp> links();
    vector<Junction::_sp> junctions();
    vector<Tank::_sp> tanks();
    vector<Reservoir::_sp> reservoirs();
    vector<Pipe::_sp> pipes();
    vector<Pump::_sp> pumps();
    vector<Valve::_sp> valves();
    
    // simulation properties
    virtual void setHydraulicTimeStep(int seconds);
    int hydraulicTimeStep();
    
    virtual void setReportTimeStep(int seconds);
    int reportTimeStep();
    
    virtual void setQualityTimeStep(int seconds);
    int qualityTimeStep();
    
    void setInitialJunctionUniformQuality(double qual);
    void setInitialJunctionQualityFromMeasurements(time_t time);
    virtual void setInitialModelQuality() { };
    vector<Node::_sp> nearestNodes(Node::_sp junc, double maxDistance);

    virtual time_t currentSimulationTime();
    TimeSeries::_sp iterations() {return _iterations;}
    TimeSeries::_sp relativeError() {return _relativeError;}
    
    void setTankResetClock(Clock::_sp resetClock);
    
    void setTanksNeedReset(bool reset);
    bool tanksNeedReset();
        
    virtual std::ostream& toStream(std::ostream &stream);

    void setRecordForDmaDemands(PointRecord::_sp record);
    void setRecordForSimulationStats(PointRecord::_sp record);
    
    // specify records for certain states or inputs
    typedef enum {
      ElementOptionNone               = 0,
      ElementOptionMeasuredAll        = 1 << 0, // setting for pre-fetch record
      ElementOptionMeasuredTanks      = 1 << 1,
      ElementOptionMeasuredFlows      = 1 << 2,
      ElementOptionMeasuredPressures  = 1 << 3,
      ElementOptionMeasuredQuality    = 1 << 4,
      ElementOptionAllTanks           = 1 << 5,
      ElementOptionAllFlows           = 1 << 6,
      ElementOptionAllPressures       = 1 << 7,
      ElementOptionAllHeads           = 1 << 8,
      ElementOptionAllQuality         = 1 << 9
    } elementOption_t;
    
    void setRecordForElementInputs(PointRecord::_sp record);
    void setRecordForElementOutput(PointRecord::_sp record, elementOption_t options);
    
    vector<TimeSeries::_sp> networkStatesWithOptions(elementOption_t options);
    vector<TimeSeries::_sp> networkInputSeries(elementOption_t options);

    
    // units
    Units flowUnits();
    Units headUnits();
    Units pressureUnits();
    Units qualityUnits();
    Units volumeUnits();
    
    void setFlowUnits(Units units);
    void setHeadUnits(Units units);
    void setPressureUnits(Units units);
    void setQualityUnits(Units units);
    void setVolumeUnits(Units units);
    
  protected:
    
    void setSimulationParameters(time_t time);
    void saveNetworkStates(time_t time);
    
    
    
    // model parameter setting
    // recreating or wrapping basic api functionality here.
    virtual double reservoirLevel(const string& reservoirName) { return 0; };
    virtual double tankLevel(const string& tankName) { return 0; };
    virtual double tankVolume(const std::string& tank) { return 0; };
    virtual double tankFlow(const std::string& tank) { return 0; };
    virtual double junctionHead(const string& junction) { return 0; };
    virtual double junctionPressure(const string& junction) { return 0; };
    virtual double junctionDemand(const string& junctionName) { return 0; };
    virtual double junctionQuality(const string& junctionName) { return 0; };
    virtual double junctionInitialQuality(const string& junctionName) { return 0; };
    // link elements
    virtual double pipeFlow(const string& pipe) { return 0; };
    virtual double pumpEnergy(const string& pump) { return 0; };
    
    virtual void setReservoirHead(const string& reservoir, double level) { };
    virtual void setReservoirQuality(const string& reservoir, double quality) { };
    virtual void setTankLevel(const string& tank, double level) { };
    virtual void setJunctionDemand(const string& junction, double demand) { };
    virtual void setJunctionQuality(const string& junction, double quality) { };
    
    virtual void setPipeStatus(const string& pipe, Pipe::status_t status) { };
    virtual void setPumpStatus(const string& pump, Pipe::status_t status) { };
    virtual void setPumpSetting(const std::string& pump, double setting) { };
    virtual void setValveSetting(const string& valve, double setting) { };
    
    virtual bool solveSimulation(time_t time) { return 1; };
    virtual time_t nextHydraulicStep(time_t time) { return 0; };
    virtual void stepSimulation(time_t time) { };
    virtual int iterations(time_t time) { return 0; };
    virtual double relativeError(time_t time) { return 0; };
    
    virtual void setCurrentSimulationTime(time_t time);
    
    double nodeDirectDistance(Node::_sp n1, Node::_sp n2);
    double toRadians(double degrees);
    
    
    
  private:
    string _name;
    string _modelFile;
    bool _shouldRunWaterQuality;
    bool _tanksNeedReset;
    void _checkTanksForReset(time_t time);
    // master list access
    void add(Junction::_sp newJunction);
    void add(Pipe::_sp newPipe);

    // element lists
    // master node/link lists
//    std::vector<Node::_sp> _nodes;
//    std::vector<Link::_sp> _links;
    std::map<string, Node::_sp> _nodes;
    std::map<string, Link::_sp> _links;
    // convenience lists for iterations
    vector<Element::_sp> _elements;
    vector<Junction::_sp> _junctions;
    vector<Tank::_sp> _tanks;
    vector<Reservoir::_sp> _reservoirs;
    vector<Pipe::_sp> _pipes;
    vector<Pump::_sp> _pumps;
    vector<Valve::_sp> _valves;
    vector<Dma::_sp> _dmas;
    vector<Pipe::_sp> _dmaPipesToIgnore;
    bool _dmaShouldDetectClosedLinks;
    
    Clock::_sp _regularMasterClock, _simReportClock;
    TimeSeries::_sp _relativeError;
    TimeSeries::_sp _iterations;
    TimeSeries::_sp _convergence;
    Clock::_sp _tankResetClock;
    int _qualityTimeStep;
    bool _doesOverrideDemands;
    
    time_t _currentSimulationTime;
    
    Units _flowUnits, _headUnits, _pressureUnits, _qualityUnits, _volumeUnits;

    
  };
  
  std::ostream& operator<< (std::ostream &out, Model &model);

}


#endif
