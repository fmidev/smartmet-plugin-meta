#pragma once

#include <map>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
struct ForecastMetaData
{
  ForecastMetaData(std::string thename,
                   std::string thelabel,
                   std::string thebasePhenomenon,
                   std::string theunit,
                   std::string thestatId,
                   std::string thestatFunc,
                   std::string thetimePeriod,
                   std::string otherstatFunc,
                   std::string othertimePeriod);

  ForecastMetaData(std::string thename,
                   std::string thelabel,
                   std::string thebasePhenomenon,
                   std::string theunit,
                   std::string thestatId,
                   std::string thestatFunc,
                   std::string thetimePeriod);

  ForecastMetaData(std::string thename,
                   std::string thelabel,
                   std::string thebasePhenomenon,
                   std::string theunit);

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
