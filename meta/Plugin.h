// ======================================================================
/*!
 * \brief SmartMet Meta plugin interface
 */
// ======================================================================

#pragma once

#include "DataQuality.h"

#include <spine/ConfigBase.h>
#include <spine/OptionParsers.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>

#include <engines/geonames/Engine.h>
#include <engines/querydata/Engine.h>

#ifndef WITHOUT_OBSERVATION
#include <engines/observation/Engine.h>
#include <engines/observation/ObservableProperty.h>
#endif

#include <macgyver/TemplateDirectory.h>
#include <macgyver/TemplateFormatterMT.h>

#include <boost/noncopyable.hpp>

#include <ctpp2/CDT.hpp>

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
  virtual ~Plugin();

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
  void createTemplateFormatters();

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

  void parseObservablePropertiesResponse(
      boost::shared_ptr<std::vector<Engine::Observation::ObservableProperty> >&
          observableProperties,
      CTPP::CDT& hash,
      std::vector<std::string>& parameters);
#endif

  void parseForecastConfig();

  void parseDataQualityConfig();

#ifndef WITHOUT_OBSERVATION
  void updateObservableProperties(
      boost::shared_ptr<std::vector<Engine::Observation::ObservableProperty> >&
          observableProperties,
      const std::string& language);
#endif

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

  std::unique_ptr<Fmi::TemplateDirectory> itsTemplateDirectory;
  boost::shared_ptr<Fmi::TemplateFormatterMT> itsExceptionFormatter;
  boost::shared_ptr<Fmi::TemplateFormatterMT> itsDataQualityFormatter;

  // Map contains the keys configured in plugin parameter configuration.
  std::map<std::string, bool> m_supportedUnits;
};  // class Plugin

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
