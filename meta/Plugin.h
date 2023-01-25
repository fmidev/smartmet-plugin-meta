// ======================================================================
/*!
 * \brief SmartMet Meta plugin interface
 */
// ======================================================================

#pragma once

#include "DataQuality.h"

#include <engines/geonames/Engine.h>
#include <engines/querydata/Engine.h>
#include <spine/ConfigBase.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>
#include <timeseries/OptionParsers.h>
#ifndef WITHOUT_OBSERVATION
#include <engines/observation/Engine.h>
#include <engines/observation/ObservableProperty.h>
#endif
#include <ctpp2/CDT.hpp>
#include <macgyver/TemplateFactory.h>
#include <string>
#include <utility>

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
struct ForecastMetaData;

class Plugin : public SmartMetPlugin
{
 public:
  Plugin(Spine::Reactor* theReactor, const char* theConfig);
  ~Plugin() override = default;
  Plugin(const Plugin& other) = delete;
  Plugin(Plugin&& other) = delete;
  Plugin& operator=(const Plugin& other) = delete;
  Plugin& operator=(Plugin&& other) = delete;

  const std::string& getPluginName() const override;
  int getRequiredAPIVersion() const override;
  bool queryIsFast(const Spine::HTTP::Request& theRequest) const override;

 protected:
  void init() override;
  void shutdown() override;
  void requestHandler(Spine::Reactor& theReactor,
                      const Spine::HTTP::Request& theRequest,
                      Spine::HTTP::Response& theResponse) override;

 private:
  std::string query(Spine::Reactor& theReactor,
                    const Spine::HTTP::Request& theRequest,
                    Spine::HTTP::Response& theResponse);

  /**
   *  \brief Existence of emplate directory and files are checked and initilized.
   *  \exception std::runtime_error Directory or file is not found.
   */
  std::string getDataQualityMetadata(Spine::Reactor& theReactor,
                                     const Spine::HTTP::Request& theRequest,
                                     Spine::HTTP::Response& theResponse);

  std::string getForecastMetadata(Spine::Reactor& theReactor,
                                  const Spine::HTTP::Request& theRequest,
                                  Spine::HTTP::Response& theResponse);

  std::string formatObservablePropertiesResponse(CTPP::CDT& hash);

#ifndef WITHOUT_OBSERVATION
  std::string getFlashCount(Spine::Reactor& /* theReactor */,
                            const Spine::HTTP::Request& theRequest,
                            Spine::HTTP::Response& theResponse);

  std::string getObsEngineMetadata(Spine::Reactor& theReactor,
                                   const Spine::HTTP::Request& theRequest,
                                   Spine::HTTP::Response& theResponse);

#endif

  void parseForecastConfig();

  void parseDataQualityConfig();
  void parseDataQualityConfig(const boost::filesystem::path& file);

  const std::string itsModuleName;

  Spine::ConfigBase itsConfig;

  Spine::Reactor* itsReactor{nullptr};

  Engine::Geonames::Engine* itsGeoEngine{nullptr};

  Engine::Querydata::Engine* itsQEngine{nullptr};

#ifndef WITHOUT_OBSERVATION
  Engine::Observation::Engine* itsObsEngine{nullptr};
#endif

 public:
  using ForecastMap = std::map<std::string, std::map<std::string, ForecastMetaData>>;

 private:
  ForecastMap itsForecastMap;  // parameter->language->metadata

  DataQualityRegistry itsDataQualityRegistry;

  std::string itsExceptionTemplateFile;
  std::string itsDataQualityTemplateFile;
  Fmi::TemplateFactory itsTemplateFactory;

  // Map contains the keys configured in plugin parameter configuration.
  std::map<std::string, bool> m_supportedUnits;
};  // class Plugin

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
