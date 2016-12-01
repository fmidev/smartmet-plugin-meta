// ======================================================================
/*!
 * \brief SmartMet Meta plugin interface
 */
// ======================================================================

#pragma once

#include "DataQuality.h"

#include <spine/SmartMetPlugin.h>
#include <spine/Reactor.h>
#include <spine/ConfigBase.h>
#include <spine/OptionParsers.h>
#include <engines/observation/ObservableProperty.h>

#include <engines/geonames/Engine.h>
#include <engines/querydata/Engine.h>
#include <engines/observation/Engine.h>

#include <macgyver/TemplateFormatterMT.h>
#include <macgyver/TemplateDirectory.h>

#include <boost/noncopyable.hpp>

#include <ctpp2/CDT.hpp>

#include <utility>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
struct ForecastMetaData;

class Plugin : public SmartMetPlugin, virtual boost::noncopyable
{
 public:
  Plugin(SmartMet::Spine::Reactor* theReactor, const char* theConfig);
  virtual ~Plugin();

  const std::string& getPluginName() const;
  int getRequiredAPIVersion() const;
  bool queryIsFast(const SmartMet::Spine::HTTP::Request& theRequest) const;

 protected:
  void init();
  void shutdown();
  void requestHandler(SmartMet::Spine::Reactor& theReactor,
                      const SmartMet::Spine::HTTP::Request& theRequest,
                      SmartMet::Spine::HTTP::Response& theResponse);

 private:
  std::string query(SmartMet::Spine::Reactor& theReactor,
                    const SmartMet::Spine::HTTP::Request& theRequest,
                    SmartMet::Spine::HTTP::Response& theResponse);

  /**
   *  \brief Existence of emplate directory and files are checked and initilized.
   *  \exception std::runtime_error Directory or file is not found.
   */
  void createTemplateFormatters();

  std::string getDataQualityMetadata(SmartMet::Spine::Reactor& theReactor,
                                     const SmartMet::Spine::HTTP::Request& theRequest,
                                     SmartMet::Spine::HTTP::Response& theResponse);

  std::string getFlashCount(SmartMet::Spine::Reactor& /* theReactor */,
                            const SmartMet::Spine::HTTP::Request& theRequest,
                            SmartMet::Spine::HTTP::Response& theResponse);

  std::string getObsEngineMetadata(SmartMet::Spine::Reactor& theReactor,
                                   const SmartMet::Spine::HTTP::Request& theRequest,
                                   SmartMet::Spine::HTTP::Response& theResponse);

  std::string getForecastMetadata(SmartMet::Spine::Reactor& theReactor,
                                  const SmartMet::Spine::HTTP::Request& theRequest,
                                  SmartMet::Spine::HTTP::Response& theResponse);

  std::string formatObservablePropertiesResponse(CTPP::CDT& hash);

  void parseObservablePropertiesResponse(
      boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >&
          observableProperties,
      CTPP::CDT& hash,
      std::vector<std::string>& parameters);

  void parseForecastConfig();

  void parseDataQualityConfig();

  void updateObservableProperties(
      boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >&
          observableProperties,
      const std::string& language);

  const std::string itsModuleName;

  SmartMet::Spine::ConfigBase itsConfig;

  SmartMet::Spine::Reactor* itsReactor;

  SmartMet::Engine::Geonames::Engine* itsGeoEngine;

  SmartMet::Engine::Querydata::Engine* itsQEngine;

  SmartMet::Engine::Observation::Engine* itsObsEngine;

  std::map<std::string, std::map<std::string, ForecastMetaData> >
      itsForecastMap;  // parameter->language->metadata

  DataQualityRegistry itsDataQualityRegistry;

  boost::scoped_ptr<Fmi::TemplateDirectory> itsTemplateDirectory;
  boost::shared_ptr<Fmi::TemplateFormatterMT> itsExceptionFormatter;
  boost::shared_ptr<Fmi::TemplateFormatterMT> itsDataQualityFormatter;

  // Map contains the keys configured in plugin parameter configuration.
  std::map<std::string, bool> m_supportedUnits;
};  // class Plugin

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
