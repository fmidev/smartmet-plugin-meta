
#include "ForecastMetaData.h"

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
ForecastMetaData::ForecastMetaData(std::string thename,
                                   std::string thelabel,
                                   std::string thebasePhenomenon,
                                   std::string theunit,
                                   std::string thestatId,
                                   std::string thestatFunc,
                                   std::string thetimePeriod,
                                   std::string otherstatFunc,
                                   std::string othertimePeriod)
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

ForecastMetaData::ForecastMetaData(std::string thename,
                                   std::string thelabel,
                                   std::string thebasePhenomenon,
                                   std::string theunit,
                                   std::string thestatId,
                                   std::string thestatFunc,
                                   std::string thetimePeriod)
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

ForecastMetaData::ForecastMetaData(std::string thename,
                                   std::string thelabel,
                                   std::string thebasePhenomenon,
                                   std::string theunit)
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
