#pragma once

#include <string>
#include <map>

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
struct ForecastMetaData
{
  ForecastMetaData(const std::string& thename,
                   const std::string& thelabel,
                   const std::string& thebasePhenomenon,
                   const std::string& theunit,
                   const std::string& thestatId,
                   const std::string& thestatFunc,
                   const std::string& thetimePeriod,
                   const std::string& otherstatFunc,
                   const std::string& othertimePeriod);

  ForecastMetaData(const std::string& thename,
                   const std::string& thelabel,
                   const std::string& thebasePhenomenon,
                   const std::string& theunit,
                   const std::string& thestatId,
                   const std::string& thestatFunc,
                   const std::string& thetimePeriod);

  ForecastMetaData(const std::string& thename,
                   const std::string& thelabel,
                   const std::string& thebasePhenomenon,
                   const std::string& theunit);

  std::string name;

  std::string label;

  std::string basePhenomenon;

  std::string unit;

  std::map<std::string, std::string> unitMap;

  std::string statId;

  std::string statFunc;

  std::string timePeriod;

  std::string otherStatFunc;

  std::string otherTimePeriod;

  bool isStatistical;

  bool hasOtherAggregation;
};

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet
