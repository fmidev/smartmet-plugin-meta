#include "MultiLanguageString.h"
#include <spine/Exception.h>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <macgyver/String.h>
#include <macgyver/TypeName.h>

/**

@page META_PLUGIN_MULTI_LANGUAGE_STRING Multi Language String

This configuration object contains string value in several langugaes.

The default language is specified when reading multilanguage string from
libconfig type configuration file. A translation to the default language
is required to be present

An example:

@verbatim
label: { eng: "An example"; fin: "Esimerkki"; lav:"Piemērs"; };
@endverbatim

 */

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
MultiLanguageString::MultiLanguageString(const std::string& the_default_language,
                                         libconfig::Setting& setting)
    : default_language(Fmi::ascii_tolower_copy(the_default_language)), data()
{
  try
  {
    bool found_default = false;

    if (setting.getType() != libconfig::Setting::TypeGroup)
    {
      std::ostringstream msg;
      msg << "Libconfig group expected instead of '";
      SmartMet::Spine::ConfigBase::dump_setting(msg, setting);
      msg << "'";
      throw SmartMet::Spine::Exception(BCP, msg.str());
    }

    const int num_items = setting.getLength();
    for (int i = 0; i < num_items; i++)
    {
      libconfig::Setting& item = setting[i];
      if (item.getType() != libconfig::Setting::TypeString)
      {
        std::ostringstream msg;
        msg << "Libconfig string expected instead of '";
        SmartMet::Spine::ConfigBase::dump_setting(msg, item);
        msg << "' while reading item '";
        SmartMet::Spine::ConfigBase::dump_setting(msg, setting);
        msg << "'";
        throw SmartMet::Spine::Exception(BCP, msg.str());
      }

      const std::string value = item;
      const std::string tmp = item.getName();
      const std::string name = boost::algorithm::to_lower_copy(tmp);
      if (name == this->default_language)
        found_default = true;
      data[name] = value;
    }

    if (not found_default)
    {
      std::ostringstream msg;
      msg << "The string for the default language '" << this->default_language
          << "' is not found in '";
      SmartMet::Spine::ConfigBase::dump_setting(msg, setting);
      throw SmartMet::Spine::Exception(BCP, msg.str());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

boost::shared_ptr<MultiLanguageString> MultiLanguageString::create(
    const std::string& default_language, libconfig::Setting& setting)
{
  try
  {
    boost::shared_ptr<MultiLanguageString> result(
        new MultiLanguageString(default_language, setting));
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

MultiLanguageString::~MultiLanguageString()
{
}

std::string MultiLanguageString::get(const std::string& language) const
{
  try
  {
    auto pos = data.find(Fmi::ascii_tolower_copy(language));
    if (pos == data.end())
    {
      return get(default_language);
    }
    else
    {
      return pos->second;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet
