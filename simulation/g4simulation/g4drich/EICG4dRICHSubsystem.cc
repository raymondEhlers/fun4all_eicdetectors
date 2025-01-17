//____________________________________________________________________________..
//
// This is the interface to the framework. You only need to define the
// parameters you use for your detector in the SetDefaultParameters() method
// here The place to do this is marked by //implement your own here// The
// parameters have no units, they need to be converted in the
// EICG4dRICHDetector::ConstructMe() method
// but the convention is as mentioned cm and deg
//____________________________________________________________________________..
//
#include "EICG4dRICHDetector.h"
#include "EICG4dRICHSteppingAction.h"
#include "EICG4dRICHSubsystem.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4SteppingAction.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

//_______________________________________________________________________
EICG4dRICHSubsystem::EICG4dRICHSubsystem(const std::string &name)
    : PHG4DetectorSubsystem(name)
    , m_Detector(nullptr)
    , m_SteppingAction(nullptr) 
{
  // call base class method which will set up parameter infrastructure
  // and call our SetDefaultParameters() method
  InitializeParameters();
}
//_______________________________________________________________________
int EICG4dRICHSubsystem::InitRunSubsystem(PHCompositeNode *topNode) 
{
  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  PHNodeIterator dstIter(dstNode);
  if (GetParams()->get_int_param("active")) 
  {
    PHCompositeNode *DetNode = dynamic_cast<PHCompositeNode*>(dstIter.findFirst("PHCompositeNode", Name()));
    if (!DetNode) 
    {
      DetNode = new PHCompositeNode(Name());
      dstNode->addNode(DetNode);
    }
    std::string g4hitnodename = "G4HIT_" + Name();
    PHG4HitContainer *g4_hits = findNode::getClass<PHG4HitContainer>(DetNode, g4hitnodename);
    if (!g4_hits) 
    {
      g4_hits = new PHG4HitContainer(g4hitnodename);
      DetNode->addNode(new PHIODataNode<PHObject>(g4_hits, g4hitnodename, "PHObject"));
    }
  }

  if (m_geoFile.empty())
  {
    std::cout << "ERROR in " << __FILE__ << ": No EICG4dRICH geometry file specified. Abort detector construction." << std::endl;
    std::cout << "Please run SetGeometryFile(std::string filename) first." << std::endl;
    exit(1);
  }
  set_string_param("mapping_file", m_geoFile);
  UpdateParametersWithMacro();

  // create detector
  m_Detector = new EICG4dRICHDetector(this, topNode, GetParams(), Name());
  m_Detector->OverlapCheck(CheckOverlap());
  // create stepping action if detector is active
  if (GetParams()->get_int_param("active")) 
  {
    m_SteppingAction = new EICG4dRICHSteppingAction(m_Detector, GetParams());
  }
  return 0;
}
//_______________________________________________________________________
int EICG4dRICHSubsystem::process_event(PHCompositeNode *topNode) 
{
  // pass top node to stepping action so that it gets
  // relevant nodes needed internally
  if (m_SteppingAction) 
  {
    m_SteppingAction->SetInterfacePointers(topNode);
  }
  return 0;
}
//_______________________________________________________________________
void EICG4dRICHSubsystem::Print(const std::string &what) const 
{
  if (m_Detector) 
  {
    m_Detector->Print(what);
  }
  return;
}

//_______________________________________________________________________
PHG4Detector *EICG4dRICHSubsystem::GetDetector(void) const { return m_Detector; }

//_______________________________________________________________________
void EICG4dRICHSubsystem::SetDefaultParameters() 
{
  //set_default_int_param("verbosity", 0);
  // sizes are in cm
  // angles are in deg
  // units should be converted to G4 units when used
  // implement your own here//
  /*
  set_default_double_param("place_x", 0.);
  set_default_double_param("place_y", 0.);
  set_default_double_param("place_z", 0.);
  set_default_double_param("rot_x", 0.);
  set_default_double_param("rot_y", 0.);
  set_default_double_param("rot_z", 0.);
  set_default_double_param("size_x", 20.);
  set_default_double_param("size_y", 20.);
  set_default_double_param("size_z", 20.);

  set_default_string_param("material", "G4_Cu");
  */
  set_default_string_param("mapping_file", m_geoFile.c_str());
}
