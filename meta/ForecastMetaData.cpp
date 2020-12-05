
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
    : name(std::move(thename)),
      label(std::move(thelabel)),
      basePhenomenon(std::move(thebasePhenomenon)),
      unit(std::move(theunit)),
      statId(std::move(thestatId)),
      statFunc(std::move(thestatFunc)),
      timePeriod(std::move(thetimePeriod)),
      otherStatFunc(std::move(otherstatFunc)),
      otherTimePeriod(std::move(othertimePeriod)),
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
    : name(std::move(thename)),
      label(std::move(thelabel)),
      basePhenomenon(std::move(thebasePhenomenon)),
      unit(std::move(theunit)),
      statId(std::move(thestatId)),
      statFunc(std::move(thestatFunc)),
      timePeriod(std::move(thetimePeriod)),
      isStatistical(true),
      hasOtherAggregation(false)
{
}

ForecastMetaData::ForecastMetaData(std::string thename,
                                   std::string thelabel,
                                   std::string thebasePhenomenon,
                                   std::string theunit)
    : name(std::move(thename)),
      label(std::move(thelabel)),
      basePhenomenon(std::move(thebasePhenomenon)),
      unit(std::move(theunit)),
      isStatistical(false),
      hasOtherAggregation(false)
{
}

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet
