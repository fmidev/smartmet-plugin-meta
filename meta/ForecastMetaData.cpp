
#include "ForecastMetaData.h"

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
ForecastMetaData::ForecastMetaData(const std::string& thename,
                                   const std::string& thelabel,
                                   const std::string& thebasePhenomenon,
                                   const std::string& theunit,
                                   const std::string& thestatId,
                                   const std::string& thestatFunc,
                                   const std::string& thetimePeriod,
                                   const std::string& otherstatFunc,
                                   const std::string& othertimePeriod)
    : name(thename),
      label(thelabel),
      basePhenomenon(thebasePhenomenon),
      unit(theunit),
      statId(thestatId),
      statFunc(thestatFunc),
      timePeriod(thetimePeriod),
      otherStatFunc(otherstatFunc),
      otherTimePeriod(othertimePeriod),
      isStatistical(true),
      hasOtherAggregation(true)
{
}

ForecastMetaData::ForecastMetaData(const std::string& thename,
                                   const std::string& thelabel,
                                   const std::string& thebasePhenomenon,
                                   const std::string& theunit,
                                   const std::string& thestatId,
                                   const std::string& thestatFunc,
                                   const std::string& thetimePeriod)
    : name(thename),
      label(thelabel),
      basePhenomenon(thebasePhenomenon),
      unit(theunit),
      statId(thestatId),
      statFunc(thestatFunc),
      timePeriod(thetimePeriod),
      isStatistical(true),
      hasOtherAggregation(false)
{
}

ForecastMetaData::ForecastMetaData(const std::string& thename,
                                   const std::string& thelabel,
                                   const std::string& thebasePhenomenon,
                                   const std::string& theunit)
    : name(thename),
      label(thelabel),
      basePhenomenon(thebasePhenomenon),
      unit(theunit),
      isStatistical(false),
      hasOtherAggregation(false)
{
}

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet
