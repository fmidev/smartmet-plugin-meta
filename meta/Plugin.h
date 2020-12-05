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
#include <spine/OptionParsers.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>
#ifndef WITHOUT_OBSERVATION
#include <engines/observation/Engine.h>
#include <engines/observation/ObservableProperty.h>
#endif
#include <boost/noncopyable.hpp>
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

class Plugin : public SmartMetPlugin, virtual boost::noncopyable
{
 public:
  Plugin(Spine::Reactor* theReactor, const char* theConfig);
  virtual ~Plugin() = default;

  const std::string& getPluginName() const;
  int getRequiredAPIVersion() const;
  bool queryIsFast(const Spine::HTTP::Request& theRequest) const;

 protected:
  void init();
  void shutdown();
  void requestHandler(Spine::Reactor& theReactor,
                      const Spine::HTTP::Request& theRequest,
                      Spine::HTTP::Response& theResponse);

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

  const std::string itsModuleName;

  Spine::ConfigBase itsConfig;

  Spine::Reactor* itsReactor{nullptr};

  Engine::Geonames::Engine* itsGeoEngine{nullptr};

  Engine::Querydata::Engine* itsQEngine{nullptr};

#ifndef WITHOUT_OBSERVATION
  Engine::Observation::Engine* itsObsEngine{nullptr};
#endif

  std::map<std::string, std::map<std::string, ForecastMetaData> >
      itsForecastMap;  // parameter->language->metadata

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
