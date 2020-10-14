// ======================================================================
/*!
 * \brief SmartMet Meta plugin implementation
 */
// ======================================================================

#include "Plugin.h"
#include "ForecastMetaData.h"
#ifndef WITHOUT_OBSERVATION
#include <engines/observation/Utils.h>
#endif
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <ctpp2/CDT.hpp>
#include <macgyver/StringConversion.h>
#include <macgyver/TimeZoneFactory.h>
#include <spine/Convenience.h>
#include <macgyver/Exception.h>
#include <spine/SmartMet.h>
#include <spine/Table.h>
#include <spine/Value.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace
{
#if 0
  std::string formatDouble(double in)
  {
	std::ostringstream os;
	os << std::setprecision(6) << in;
	return os.str();
  }

  std::string formatInt(int in)
  {
	std::ostringstream os;
	os << in;
	return os.str();
  }
#endif
}  // namespace

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
namespace bp = boost::posix_time;
// ----------------------------------------------------------------------
/*!
 * \brief Plugin constructor
 */
// ----------------------------------------------------------------------

Plugin::Plugin(SmartMet::Spine::Reactor* theReactor, const char* theConfig)
    : itsModuleName("Meta"), itsConfig(theConfig), itsReactor(theReactor)
{
  try
  {
    if (theReactor->getRequiredAPIVersion() != SMARTMET_API_VERSION)
      throw Fmi::Exception(BCP, "Meta plugin and Server API version mismatch");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Initializator
 */
// ----------------------------------------------------------------------
void Plugin::init()
{
  try
  {
    // Obtain the QEngine pointer
    itsQEngine = reinterpret_cast<SmartMet::Engine::Querydata::Engine*>(
        itsReactor->getSingleton("Querydata", nullptr));

    // ObsEngine needs also GeoEngine, get it
    itsGeoEngine = reinterpret_cast<SmartMet::Engine::Geonames::Engine*>(
        itsReactor->getSingleton("Geonames", nullptr));
    if (itsGeoEngine == nullptr)
      throw Fmi::Exception(BCP, "Geonames engine unavailable");

#ifndef WITHOUT_OBSERVATION
    // Obtain the ObsEngine pointer
    itsObsEngine = reinterpret_cast<SmartMet::Engine::Observation::Engine*>(
        itsReactor->getSingleton("Observation", nullptr));
#endif

#ifndef WITHOUT_OBSERVATION
    if (itsQEngine == nullptr && itsObsEngine == nullptr)
    {
      // Nothing of interest available, print notification
      throw Fmi::Exception(BCP,
                                       "Both Querydata-engine and Observation-engine are absent, "
                                       "Meta-plugin has nothing to show");
    }
#else
    if (itsQEngine == nullptr)
    {
      // Nothing of interest available, print notification
      throw Fmi::Exception(BCP,
                                       "Querydata engine absent, Meta-plugin has nothing to show");
    }
#endif

    itsDataQualityRegistry.supportQualityCodeConversion(true);
    parseForecastConfig();
    parseDataQualityConfig();

    std::string template_dir = itsConfig.get_mandatory_path("templateDir");
    std::string itsExceptionTemplateFile =
        template_dir + "/" + itsConfig.get_mandatory_config_param<std::string>("exceptionTemplate");
    std::string itsDataQualityTemplateFile =
        template_dir + "/" +
        itsConfig.get_mandatory_config_param<std::string>("dataQualityTemplate");

    if (!itsReactor->addContentHandler(
            this, "/meta", boost::bind(&Plugin::callRequestHandler, this, _1, _2, _3)))
      throw Fmi::Exception(BCP, "Failed to register Meta plugin content handler");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Shutdown the plugin
 */
// ----------------------------------------------------------------------

void Plugin::shutdown()
{
  std::cout << "  -- Shutdown requested (Meta)\n";
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the plugin name
 */
// ----------------------------------------------------------------------

const std::string& Plugin::getPluginName() const
{
  return itsModuleName;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the plugin name
 */
// ----------------------------------------------------------------------
void Plugin::parseForecastConfig()
{
  try
  {
    auto parameters = itsConfig.get_mandatory_config_array<std::string>("forecast_parameters");

    BOOST_FOREACH (const std::string& param, parameters)
    {
      auto name = itsConfig.get_mandatory_config_param<std::string>(param + ".name");

      Fmi::ascii_tolower(name);

      auto iteratorPair =
          itsForecastMap.insert(std::make_pair(name, std::map<std::string, ForecastMetaData>()));
      if (!iteratorPair.second)
      {
        throw Fmi::Exception(
            BCP, "Multiple definition of parameter " + param + " in Meta config");
      }

      const libconfig::Setting& units =
          itsConfig.get_mandatory_config_param<libconfig::Setting&>(param + ".units");
      auto unit = itsConfig.get_mandatory_config_param<std::string>(param + ".units.default");

      auto isStatistical = itsConfig.get_mandatory_config_param<bool>(param + ".is_statistical");

      const libconfig::Setting& labels =
          itsConfig.get_mandatory_config_param<libconfig::Setting&>(param + ".label");

      const libconfig::Setting& phenomenons =
          itsConfig.get_mandatory_config_param<libconfig::Setting&>(param + ".base_phenomenon");

      int labelLength = labels.getLength();

      int phenomenonLength = phenomenons.getLength();

      if (labelLength != phenomenonLength)
      {
        throw Fmi::Exception(
            BCP, "Language number mismatch in parameter " + param + " in Meta config");
      }

      // Language maps for labels and phenomenons
      std::map<std::string, std::string> labelmap;

      std::map<std::string, std::string> phenomenonmap;

      for (int i = 0; i < labelLength; ++i)
      {
        auto& thisLabel = labels[i];
        auto& thisPhenomenon = phenomenons[i];

        auto labelLan = thisLabel.getName();
        auto phenomenonLan = thisPhenomenon.getName();

        const std::string labelString = thisLabel;
        const std::string phenString = thisPhenomenon;

        labelmap.insert(std::make_pair(labelLan, labelString));
        phenomenonmap.insert(std::make_pair(phenomenonLan, phenString));
      }

      std::map<std::string, std::string> unitmap;

      for (int i = 0; i < units.getLength(); ++i)
      {
        std::string unitValue = units[i];
        std::string unitKey = units[i].getName();
        Fmi::ascii_tolower(unitKey);
        unitmap.insert(std::make_pair(unitKey, unitValue));
        m_supportedUnits[unitKey] = true;
      }

      for (const auto& tmp : labelmap)
      {
        const auto& lan = tmp.first;
        const auto& thisLabel = tmp.second;

        // Find the matching phenomenon
        auto phit = phenomenonmap.find(lan);
        if (phit == phenomenonmap.end())
        {
          throw Fmi::Exception(
              BCP,
              "Invalid translation for base phenomenon in parameter " + param + " in Meta config");
        }

        auto thisPhenomenon = phit->second;

        if (isStatistical)
        {
          // Get statistics information
          auto statFunc =
              itsConfig.get_mandatory_config_param<std::string>(param + ".statistical_function");
          auto aggrTime =
              itsConfig.get_mandatory_config_param<std::string>(param + ".aggregation_time");

          auto statId = statFunc + "-" + Fmi::ascii_tolower_copy(aggrTime);

          // Check if otherAggregation is defined
          bool hasOtherAgg = itsConfig.get_config().exists(param + ".otherAggregation");
          if (hasOtherAgg)
          {
            auto otherstatFunc = itsConfig.get_mandatory_config_param<std::string>(
                param + ".otherAggregation.statistical_function");
            auto otheraggrTime = itsConfig.get_mandatory_config_param<std::string>(
                param + ".otherAggregation.aggregation_time");
            ForecastMetaData fMetaData = ForecastMetaData(name,
                                                          thisLabel,
                                                          thisPhenomenon,
                                                          unit,
                                                          statId,
                                                          statFunc,
                                                          aggrTime,
                                                          otherstatFunc,
                                                          otheraggrTime);
            fMetaData.unitMap.insert(unitmap.begin(), unitmap.end());
            iteratorPair.first->second.insert(std::make_pair(lan, fMetaData));
          }
          else
          {
            ForecastMetaData fMetaData =
                ForecastMetaData(name, thisLabel, thisPhenomenon, unit, statId, statFunc, aggrTime);
            fMetaData.unitMap.insert(unitmap.begin(), unitmap.end());
            iteratorPair.first->second.insert(std::make_pair(lan, fMetaData));
          }
        }
        else
        {
          // Without statistics information
          ForecastMetaData fMetaData = ForecastMetaData(name, thisLabel, thisPhenomenon, unit);
          fMetaData.unitMap.insert(unitmap.begin(), unitmap.end());
          iteratorPair.first->second.insert(std::make_pair(lan, fMetaData));
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

void Plugin::parseDataQualityConfig()
{
  try
  {
    // Test folder path and file existencies
    boost::filesystem::path features_dir(itsConfig.get_mandatory_path("dataQualityDefinitionDir"));

    if (not boost::filesystem::exists(features_dir))
    {
      throw Fmi::Exception(BCP, "Directory '" + features_dir.string() + "' not found!");
    }

    if (not boost::filesystem::is_directory(features_dir))
    {
      throw Fmi::Exception(BCP, "'" + features_dir.string() + "' is not a directory!");
    }

    std::string defaultLanguage =
        itsConfig.get_mandatory_config_param<std::string>("defaultLanguage");

    // Iterate over all data quality code configurations.
    for (auto it = boost::filesystem::directory_iterator(features_dir);
         it != boost::filesystem::directory_iterator();
         ++it)
    {
      const boost::filesystem::path entry = *it;
      const auto fn = entry.filename().string();
      if (boost::filesystem::is_regular_file(entry) and
          not boost::algorithm::starts_with(fn, ".") and
          not boost::algorithm::starts_with(fn, "#") and boost::algorithm::ends_with(fn, ".conf"))
      {
        boost::shared_ptr<SmartMet::Spine::ConfigBase> desc(
            new SmartMet::Spine::ConfigBase(entry.string(), "Data quality description"));

        try
        {
          // Get root of the data quality code configuration and form an entry into
          // DataQualityRegistry.
          auto& root = desc->get_root();
          const bool disabled = desc->get_optional_config_param<bool>(root, "disabled", false);
          const std::string code = desc->get_mandatory_config_param<std::string>(root, "code");
          auto& labelDesc = desc->get_mandatory_config_param<libconfig::Setting&>("label");
          auto& descriptionDesc =
              desc->get_mandatory_config_param<libconfig::Setting&>("description");

          if (code.length() == 0)
          {
            std::ostringstream msg;
            msg << SmartMet::Spine::log_time_str()
                << ": [Meta] error reading Data quality description"
                << " file '" << entry.string() << "'. Parameter 'code' is zero length.";
            throw Fmi::Exception(BCP, msg.str());
          }

          if (disabled)
          {
            std::ostringstream msg;
            msg << SmartMet::Spine::log_time_str() << ": [Meta] [Disabled] Data quality code='"
                << code << "' config='" << entry.string() << "'\n";
            std::cout << msg.str() << std::flush;
          }
          else
          {
            desc->assert_is_group(labelDesc);
            desc->assert_is_group(descriptionDesc);

            itsDataQualityRegistry.addMapEntry(code, defaultLanguage, labelDesc, descriptionDesc);

            std::ostringstream msg;
            msg << SmartMet::Spine::log_time_str() << ": [Meta] [Registered] Data quality code='"
                << code << "' config='" << entry.string() << "' ";

            std::vector<std::string> tmp;
            if (desc->get_config_array("alias", tmp))
            {
              msg << "aliases ";
              for (auto ait = tmp.begin(); ait != tmp.end(); ++ait)
              {
                const auto& codeAlias = *ait;
                itsDataQualityRegistry.addMapEntryAlias(code, codeAlias);
                msg << "'" << codeAlias << "' ";
              }
            }
            msg << "\n";
            std::cout << msg.str() << std::flush;
          }
        }
        catch (const std::exception&)
        {
          std::cerr << SmartMet::Spine::log_time_str()
                    << ": error reading data quality code configuration"
                    << " file '" << entry.string() << "'" << std::endl;

          Fmi::Exception exception(
              BCP, "Error while reading data quality code configuration!", nullptr);
          exception.addParameter("File", entry.string());
          throw exception;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the required version
 */
// ----------------------------------------------------------------------

int Plugin::getRequiredAPIVersion() const
{
  return SMARTMET_API_VERSION;
}
std::string Plugin::query(SmartMet::Spine::Reactor& theReactor,
                          const SmartMet::Spine::HTTP::Request& theRequest,
                          SmartMet::Spine::HTTP::Response& theResponse)
{
  try
  {
    auto parameterMap = theRequest.getParameterMap();

    if (parameterMap.count("observableProperty") == 1)
    {
      auto observableProperty =
          SmartMet::Spine::required_string(theRequest.getParameter("observableProperty"),
                                           "Parameter 'observableProperty' is undefined");

      if (observableProperty == "forecast")
      {
        return getForecastMetadata(theReactor, theRequest, theResponse);
      }
#ifndef WITHOUT_OBSERVATION
      else if (observableProperty == "observation")
      {
        return getObsEngineMetadata(theReactor, theRequest, theResponse);
      }
      else if (observableProperty == "flashcount")
      {
        theResponse.setHeader("Content-Type", "application/json; charset=UTF-8");
        return getFlashCount(theReactor, theRequest, theResponse);
      }
#endif
      else
      {
        throw Fmi::Exception(
            BCP, "Bad 'observableProperty' parameter: " + observableProperty);
      }
    }

    if (parameterMap.count("qualityCode") == 1)
    {
      return getDataQualityMetadata(theReactor, theRequest, theResponse);
    }

    CTPP::CDT hash;
    hash["language"] = "eng";
    hash["exceptionCode"] = "OperationParsingFailed";
    hash["exceptionText"] = "Bad request";

    std::ostringstream output, log;
    itsTemplateFactory.get(itsExceptionTemplateFile)->process(hash, output, log);

    theResponse.setStatus(SmartMet::Spine::HTTP::Status::bad_request);
    theResponse.setHeader("X-Meta-Error", "OperationParsingFailed");
    theResponse.setHeader("Content-Type", "text/xml; charset=UTF-8");

    return output.str();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

#ifndef WITHOUT_OBSERVATION
void Plugin::updateObservableProperties(
    boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >&
        observableProperties,
    const std::string& language)
{
  try
  {
    std::string quality_s;
    std::string phenomenon_s;
    if (language == "fi")
    {
      quality_s = " laatu";
      phenomenon_s = "Laatu";
    }
    if (language == "en")
    {
      quality_s = " quality";
      phenomenon_s = "Quality";
    }
    for (auto prop = observableProperties->begin(); prop != observableProperties->end(); ++prop)
    {
      if (!prop->gmlId.empty())
        prop->gmlId.append("-qc");
      if (!prop->observablePropertyLabel.empty())
        prop->observablePropertyLabel.append(quality_s);
      prop->basePhenomenon = phenomenon_s;
      prop->uom = "Index";
      // block statistical fields to shown out.
      prop->statisticalFunction.resize(0);
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}
#endif

#ifndef WITHOUT_OBSERVATION

void Plugin::parseObservablePropertiesResponse(
    boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >&
        observableProperties,
    CTPP::CDT& hash,
    std::vector<std::string>& /* parameters */)
{
  try
  {
    int propertiesCount = 0;

    for (auto prop = observableProperties->begin(); prop != observableProperties->end(); ++prop)
    {
      hash["observableProperties"][prop->gmlId]["observablePropertyId"] = prop->gmlId;
      hash["observableProperties"][prop->gmlId]["observablePropertyLabel"] =
          prop->observablePropertyLabel;
      hash["observableProperties"][prop->gmlId]["basePhenomenon"] = prop->basePhenomenon;

      boost::erase_all(prop->uom, " ");  // e.g. "ug S/m3"
      if (not prop->uom.empty())
        hash["observableProperties"][prop->gmlId]["uom"] = prop->uom;
      // No need to define if statisticalFunction is empty
      if (not prop->statisticalFunction.empty())
      {
        hash["observableProperties"][prop->gmlId]["statisticalMeasureId"] =
            prop->statisticalMeasureId;
        hash["observableProperties"][prop->gmlId]["statisticalFunction"] =
            prop->statisticalFunction;
        hash["observableProperties"][prop->gmlId]["aggregationTimePeriod"] =
            prop->aggregationTimePeriod;
      }
      propertiesCount++;
    }
    hash["propertiesCount"] = propertiesCount;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}
#endif

std::string Plugin::formatObservablePropertiesResponse(CTPP::CDT& hash)
{
  try
  {
    std::ostringstream output, log;

    std::string tmpl =
        itsConfig.get_mandatory_config_param<std::string>("observable_properties_template");

    itsTemplateFactory.get(tmpl)->process(hash, output, log);

    return output.str();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

std::string Plugin::getDataQualityMetadata(SmartMet::Spine::Reactor& /* theReactor */,
                                           const SmartMet::Spine::HTTP::Request& theRequest,
                                           SmartMet::Spine::HTTP::Response& theResponse)
{
  try
  {
    std::string language =
        SmartMet::Spine::optional_string(theRequest.getParameter("language"), "eng");
    boost::optional<std::string> code = theRequest.getParameter("qualityCode");

    CTPP::CDT hash;
    hash["language"] = language;
    bool exceptionResponse = false;
    std::string responseTemplate;
    std::ostringstream output, log;
    DataQualityRegistry::MapEntry mapEntry;

    // Return all if no a code given.
    if (not code or (*code == ""))
    {
      std::vector<std::string> keyList;
      itsDataQualityRegistry.getMapKeyList(keyList);
      std::vector<std::string>::iterator codeIt;
      for (codeIt = keyList.begin(); codeIt != keyList.end(); ++codeIt)
      {
        itsDataQualityRegistry.getMapEntry(*codeIt, mapEntry);
        hash["codeList"][*codeIt]["code"] = *codeIt;
        hash["codeList"][*codeIt]["label"] = mapEntry.label->get(language);
        hash["codeList"][*codeIt]["description"] = mapEntry.description->get(language);
      }
    }
    else if (itsDataQualityRegistry.getMapEntry(*code, mapEntry))
    {
      std::string realCode = *code;
      itsDataQualityRegistry.getKey(realCode, code.get());

      hash["codeList"][realCode]["code"] = realCode;
      hash["codeList"][realCode]["label"] = mapEntry.label->get(language);
      hash["codeList"][realCode]["description"] = mapEntry.description->get(language);
    }
    else
    {
      exceptionResponse = true;
    }

    if (exceptionResponse)
    {
      hash["language"] = "eng";
      hash["exceptionCode"] = "OperationParsingFailed";
      hash["exceptionText"] = "Data quality code not match with the supported codes.";

      itsTemplateFactory.get(itsExceptionTemplateFile)->process(hash, output, log);

      theResponse.setStatus(SmartMet::Spine::HTTP::Status::bad_request);
      theResponse.setHeader("X-Meta-Error", "OperationParsingFailed");
    }
    else
    {
      hash["language"] = language;
      itsTemplateFactory.get(itsDataQualityTemplateFile)->process(hash, output, log);

      theResponse.setStatus(SmartMet::Spine::HTTP::Status::ok);
    }

    theResponse.setHeader("Content-Type", "text/xml; charset=UTF-8");
    return output.str();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

#ifndef WITHOUT_OBSERVATION
std::string Plugin::getFlashCount(SmartMet::Spine::Reactor& /* theReactor */,
                                  const SmartMet::Spine::HTTP::Request& theRequest,
                                  SmartMet::Spine::HTTP::Response& /* theResponse */)
{
  try
  {
    SmartMet::Spine::TimeSeriesGeneratorOptions toptions(
        SmartMet::Spine::OptionParsers::parseTimes(theRequest));

    auto loptions(
        new SmartMet::Engine::Geonames::LocationOptions(itsGeoEngine->parseLocations(theRequest)));

    // Handle timezones
    auto tz = theRequest.getParameter("tz");
    if (!tz)
      throw Fmi::Exception(BCP, "Timezone 'tz' must be specified");

    boost::local_time::time_zone_ptr tz_ptr =
        Fmi::TimeZoneFactory::instance().time_zone_from_string(*tz);

    auto local_start_time = Fmi::TimeParser::make_time(
        toptions.startTime.date(), toptions.startTime.time_of_day(), tz_ptr);

    auto local_end_time =
        Fmi::TimeParser::make_time(toptions.endTime.date(), toptions.endTime.time_of_day(), tz_ptr);

    boost::posix_time::ptime utc_start_time = local_start_time.utc_time();
    boost::posix_time::ptime utc_end_time = local_end_time.utc_time();

    SmartMet::Engine::Observation::FlashCounts flashcounts =
        itsObsEngine->getFlashCount(utc_start_time, utc_end_time, loptions->locations());

    std::string response =
        "{\n    \"flashcount\": " + Fmi::to_string(flashcounts.flashcount) + ",\n" +
        "    \"strokecount\": " + Fmi::to_string(flashcounts.strokecount) + ",\n" +
        "    \"iccount\": " + Fmi::to_string(flashcounts.iccount) + "\n}\n";
    return response;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}
#endif

#ifndef WITHOUT_OBSERVATION
std::string Plugin::getObsEngineMetadata(SmartMet::Spine::Reactor& /* theReactor */,
                                         const SmartMet::Spine::HTTP::Request& theRequest,
                                         SmartMet::Spine::HTTP::Response& theResponse)
{
  try
  {
    std::string observablePropertyParameters =
        SmartMet::Spine::optional_string(theRequest.getParameter("param"), "");
    std::string language =
        SmartMet::Spine::optional_string(theRequest.getParameter("language"), "eng");

    // Language types are fi and en in database
    if (language == "fin")
    {
      language = "fi";
    }
    else if (language == "eng")
    {
      language = "en";
    }
    else
    {
      throw Fmi::Exception(BCP, "[Meta] Bad language: " + language);
    }

    // true is the qc_parameter
    std::vector<std::string> parameters;
    std::vector<std::string> qc_parameters;
    std::string param_name;
    if (!observablePropertyParameters.empty())
    {
      boost::algorithm::split(
          parameters, observablePropertyParameters, boost::algorithm::is_any_of(","));
      auto it = parameters.begin();
      auto it_end = parameters.end();
      while (it != it_end)
      {
        Fmi::ascii_tolower(*it);
        // Separation of quality code parameters
        if (it->compare(0, 3, "qc_") == 0)
        {
          std::string qc_param(*it);
          qc_param.erase(0, 3);
          qc_parameters.push_back(qc_param);
          // Erase qc parameter from the vector.
          parameters.erase(it);
          it = parameters.begin();
          it_end = parameters.end();
        }
        else
          ++it;
      }
    }

    // If parameter map is empty, all metadata is returned by the engine
    boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >
        observableProperties;
    if (parameters.empty() and qc_parameters.empty())
    {
      observableProperties = itsObsEngine->observablePropertyQuery(parameters, language);
    }
    else
    {
      if (not parameters.empty())
      {
        observableProperties = itsObsEngine->observablePropertyQuery(parameters, language);
      }

      if (not qc_parameters.empty())
      {
        boost::shared_ptr<std::vector<SmartMet::Engine::Observation::ObservableProperty> >
            qc_observableProperties;
        qc_observableProperties = itsObsEngine->observablePropertyQuery(qc_parameters, language);
        updateObservableProperties(qc_observableProperties, language);

        if (observableProperties == nullptr)
          observableProperties = qc_observableProperties;
        else
          observableProperties->insert(observableProperties->begin() + observableProperties->size(),
                                       qc_observableProperties->begin(),
                                       qc_observableProperties->end());
      }
    }
    CTPP::CDT hash;

    parseObservablePropertiesResponse(observableProperties, hash, parameters);

    theResponse.setHeader("Content-Type", "text/xml; charset=UTF-8");

    return formatObservablePropertiesResponse(hash);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}
#endif

std::string Plugin::getForecastMetadata(SmartMet::Spine::Reactor& /* theReactor */,
                                        const SmartMet::Spine::HTTP::Request& theRequest,
                                        SmartMet::Spine::HTTP::Response& theResponse)
{
  try
  {
    std::string observablePropertyParameters =
        SmartMet::Spine::optional_string(theRequest.getParameter("param"), "");

    std::string language =
        SmartMet::Spine::optional_string(theRequest.getParameter("language"), "eng");
    Fmi::ascii_tolower(language);

    std::string units_key = SmartMet::Spine::optional_string(theRequest.getParameter("units"), "");
    Fmi::ascii_tolower(units_key);

    CTPP::CDT hash;

    // If the value of units parameter is not configured throw an error.
    if (!units_key.empty() and m_supportedUnits.find(units_key) == m_supportedUnits.end())
    {
      std::ostringstream output, log;
      hash["language"] = "eng";
      hash["exceptionCode"] = "InvalidParameterValue";
      hash["exceptionText"] = "Unknown units parameter value.";

      itsTemplateFactory.get(itsExceptionTemplateFile)->process(hash, output, log);

      theResponse.setStatus(SmartMet::Spine::HTTP::Status::bad_request);
      theResponse.setHeader("X-Meta-Error", "InvalidParameterValue");
      return output.str();
    }

    std::vector<std::string> parameters;
    if (!observablePropertyParameters.empty())
    {
      boost::algorithm::split(
          parameters, observablePropertyParameters, boost::algorithm::is_any_of(","));
      for (auto it = parameters.begin(); it != parameters.end(); ++it)
      {
        Fmi::ascii_tolower(*it);
      }
    }

    uint32_t propertiesCount = 0;

    if (parameters.empty())
    {
      // Empty parameters-vector means we want all
      for (auto iter = itsForecastMap.begin(); iter != itsForecastMap.end(); ++iter)
      {
        auto langit = iter->second.find(language);
        if (langit != iter->second.end())
        {
          // We found the requested translation

          // Select correct unit of a parameter
          if (units_key.empty())
          {
            hash["observableProperties"][propertiesCount]["uom"] = langit->second.unit;
          }
          else
          {
            // Select only the parameters that have units_key. The key exist (see above)
            auto requested_unit = langit->second.unitMap.find(units_key);
            if (requested_unit == langit->second.unitMap.end())
              continue;
            hash["observableProperties"][propertiesCount]["uom"] = requested_unit->second;
          }

          hash["observableProperties"][propertiesCount]["observablePropertyId"] =
              langit->second.name;
          hash["observableProperties"][propertiesCount]["observablePropertyLabel"] =
              langit->second.label;
          hash["observableProperties"][propertiesCount]["basePhenomenon"] =
              langit->second.basePhenomenon;

          if (langit->second.isStatistical)
          {
            hash["observableProperties"][propertiesCount]["statisticalMeasureId"] =
                langit->second.statId;
            hash["observableProperties"][propertiesCount]["statisticalFunction"] =
                langit->second.statFunc;
            hash["observableProperties"][propertiesCount]["aggregationTimePeriod"] =
                langit->second.timePeriod;
            if (langit->second.hasOtherAggregation)
            {
              hash["observableProperties"][propertiesCount]["otherAggregation"]
                  ["statisticalFunction"] = langit->second.otherStatFunc;
              hash["observableProperties"][propertiesCount]["otherAggregation"]
                  ["aggregationTimePeriod"] = langit->second.otherTimePeriod;
            }
          }
          propertiesCount++;
        }
      }
    }
    else
    {
      // Return only the given parameters
      BOOST_FOREACH (auto& param, parameters)
      {
        auto it = itsForecastMap.find(param);
        if (it != itsForecastMap.end())
        {
          // We found a parameter with this name
          auto langit = it->second.find(language);
          if (langit != it->second.end())
          {
            // We found the requested translation
            hash["observableProperties"][propertiesCount]["observablePropertyId"] =
                langit->second.name;
            hash["observableProperties"][propertiesCount]["observablePropertyLabel"] =
                langit->second.label;
            hash["observableProperties"][propertiesCount]["basePhenomenon"] =
                langit->second.basePhenomenon;

            hash["observableProperties"][propertiesCount]["uom"] = langit->second.unit;

            // Select other unit if requested and if defined.
            if (!units_key.empty())
            {
              auto requested_unit = langit->second.unitMap.find(units_key);
              if (requested_unit != langit->second.unitMap.end())
                hash["observableProperties"][propertiesCount]["uom"] = requested_unit->second;
            }

            if (langit->second.isStatistical)
            {
              hash["observableProperties"][propertiesCount]["statisticalMeasureId"] =
                  langit->second.statId;
              hash["observableProperties"][propertiesCount]["statisticalFunction"] =
                  langit->second.statFunc;
              hash["observableProperties"][propertiesCount]["aggregationTimePeriod"] =
                  langit->second.timePeriod;
              if (langit->second.hasOtherAggregation)
              {
                hash["observableProperties"][propertiesCount]["otherAggregation"]
                    ["statisticalFunction"] = langit->second.otherStatFunc;
                hash["observableProperties"][propertiesCount]["otherAggregation"]
                    ["aggregationTimePeriod"] = langit->second.otherTimePeriod;
              }
            }

            propertiesCount++;
          }
        }
      }
    }

    hash["propertiesCount"] = propertiesCount;

    theResponse.setHeader("Content-Type", "text/xml; charset=UTF-8");

    return formatObservablePropertiesResponse(hash);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Performs the query and sets HTTP wrappings
 */
// ----------------------------------------------------------------------

void Plugin::requestHandler(SmartMet::Spine::Reactor& theReactor,
                            const SmartMet::Spine::HTTP::Request& theRequest,
                            SmartMet::Spine::HTTP::Response& theResponse)
{
  try
  {
    bool isdebug = false;

    try
    {
      const int expires_seconds = 60;
      bp::ptime t_now = bp::second_clock::universal_time();

      // Excecuting the query

      std::string response = query(theReactor, theRequest, theResponse);

      // Status might already been set while executing the query.
      if (theResponse.getStatus() == SmartMet::Spine::HTTP::Status::not_a_status)
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::ok);

      theResponse.setContent(response);

      // Adding response headers

      bp::ptime t_expires = t_now + bp::seconds(expires_seconds);
      boost::shared_ptr<Fmi::TimeFormatter> tformat(Fmi::TimeFormatter::create("http"));
      std::string cachecontrol = "public, max-age=" + std::to_string(expires_seconds);
      std::string expiration = tformat->format(t_expires);
      std::string modification = tformat->format(t_now);

      theResponse.setHeader("Cache-Control", cachecontrol);
      theResponse.setHeader("Expires", expiration);
      theResponse.setHeader("Last-Modified", modification);
      theResponse.setHeader("Access-Control-Allow-Origin", "*");

      if (response.size() == 0)
      {
        std::cerr << "Warning: Empty input for request " << theRequest.getQueryString() << " from "
                  << theRequest.getClientIP() << std::endl;
      }

#ifdef MYDEBUG
      std::cout << "Output:" << std::endl << response << std::endl;
#endif
    }
    catch (...)
    {
      // Catching all exceptions

      Fmi::Exception exception(BCP, "Request processing exception!", nullptr);
      exception.addParameter("URI", theRequest.getURI());
      exception.addParameter("ClientIP", theRequest.getClientIP());
      exception.printError();

      if (isdebug)
      {
        // Delivering the exception information as HTTP content
        std::string fullMessage = exception.getHtmlStackTrace();
        theResponse.setContent(fullMessage);
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::ok);
      }
      else
      {
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::bad_request);
      }

      // Adding the first exception information into the response header

      std::string firstMessage = exception.what();
      boost::algorithm::replace_all(firstMessage, "\n", " ");
      firstMessage = firstMessage.substr(0, 300);
      theResponse.setHeader("X-Meta-Error", firstMessage);
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Metadata queries are always fast
 */
// ----------------------------------------------------------------------

bool Plugin::queryIsFast(const SmartMet::Spine::HTTP::Request& /* theRequest */) const
{
  return true;
}

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

/*
 * Server knows us through the 'SmartMetPlugin' virtual interface, which
 * the 'Plugin' class implements.
 */

extern "C" SmartMetPlugin* create(SmartMet::Spine::Reactor* them, const char* config)
{
  return new SmartMet::Plugin::Meta::Plugin(them, config);
}

extern "C" void destroy(SmartMetPlugin* us)
{
  // This will call 'Plugin::~Plugin()' since the destructor is virtual
  delete us;
}

// ======================================================================

/**

@page META_PLUGIN_CONFIG Meta Plugin configuration

@section META_PLUGIN_CONFIG_PARAMETERS Main configuration parameters

<table>
<tr><td>\b Parameter</td><td>\b Type</td><td>\b Use</td><td>\b Description</td></tr>
<tr><td>defaultLanguage</td><td>string</td><td>mandatory</td><td>Specifies default
language</td></tr>
<tr><td>templateDir</td><td>string</td><td>mandatory</td><td>Specifies absolute path of a directory
where to look for CTPP2 format template files.</td></tr>
<tr><td>dataQualityTemplate</td><td>string</td><td>mandatory</td><td>Specifies file name of CTPP2
template file.</td></tr>
<tr><td>exceptionTemplate</td><td>string</td><td>mandatory</td><td>Specifies file name of CTPP2
template file.</td></tr>
<tr><td>observable_properties_template</td><td>string</td><td>mandatory</td><td>Specifies absolute
file name path of CTPP2 template file.</td></tr>
<tr><td>forecastTemplate</td><td>string</td><td>mandatory</td><td>Specifies absolute file name path
of CTPP2 template file.</td></tr>
<tr><td>forecast_parameters</td><td>string</td><td>mandatory</td><td>Specifies forcast parameter
names configured into the configuration file.</td></tr>
<tr><td>dataQualityDefinitionDir</td><td>string</td><td>mandatory</td><td>Specifies absolute of a
directory where to look data quality configuration files.</td></tr>
</table>

Example parameter configuration:
@verbatim
TestWind:
{
        name = "TestWind";
        label = { eng = "Test Wind"; fin = "Tuulitesti"; };
        units = { default = "mm/s"; grib = "m/s"; someother = "km/h" };
        base_phenomenon = {eng = "Wind"; fin = "Tuuli"; };
        is_statistical = true;
        statistical_function = "max";
        aggregation_time = "PT1H";
        otherAggregation = { statistical_function = "avg"; aggregation_time = "PT3S"; };
};
@endverbatim

*/
