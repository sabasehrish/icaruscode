#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "larevt/CalibrationDBI/IOVData/SIOVChannelStatusData.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Providers/SIOVChannelStatusProvider.h"

namespace lariov {

  /**
     \class SIOVChannelStatusICARUSService
     art service implementation of ChannelStatusService.  Implements
     a channel status retrieval service for database scheme in which
     all elements in a database folder share a common interval of validity
  */
  class SIOVChannelStatusICARUSService : public ChannelStatusService {
  public:
    SIOVChannelStatusICARUSService(fhicl::ParameterSet const& pset);

  private:
    ChannelStatusDataPtr DataFor(art::Event const& evt) const override;
    SIOVChannelStatusProvider fProvider;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusICARUSService,
                                   lariov::ChannelStatusService,
                                   SHARED)

namespace lariov {

  SIOVChannelStatusICARUSService::SIOVChannelStatusICARUSService(fhicl::ParameterSet const& pset)
    : fProvider(pset.get<fhicl::ParameterSet>("ChannelStatusProvider"))
  {
  }

  ChannelStatusDataPtr SIOVChannelStatusICARUSService::DataFor(const art::Event& evt) const
  {
    // LArSoft follows MicroBooNE convention where time stamp is in ns referenced to the epoch
    std::uint64_t timeStamp = evt.time().value();

    // We need to use a different format if we are looking at data
    // ICARUS convention is upper 32 bits ("timeHigh") are seconds references to the epoch
    // and then the lower 32 bits ("timeLow") are in ns referenced to the seconds above
    if (evt.isRealData()) timeStamp = std::uint64_t(evt.time().timeHigh()) * 1000000000 + std::uint64_t(evt.time().timeLow());

    mf::LogInfo("SIOVChannelStatusICARUSService") << "==> PreProcessEvent using timestamp (ns): " << timeStamp << ", timeHigh: " << evt.time().timeHigh() << ", timeLow: " << evt.time().timeLow();

    return std::make_shared<SIOVChannelStatusData>(&fProvider, timeStamp);
  }

} //end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusICARUSService, lariov::ChannelStatusService)
