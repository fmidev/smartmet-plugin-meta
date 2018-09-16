#include "DataQuality.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <macgyver/TypeName.h>
#include <spine/Exception.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
DataQualityRegistry::DataQualityRegistry() : m_keyConversionEnabled(false) {}

void DataQualityRegistry::addMapEntry(const std::string& code,
                                      const std::string& default_language,
                                      libconfig::Setting& label,
                                      libconfig::Setting& description)
{
  try
  {
    MapEntry entry;
    entry.label = MultiLanguageString::create(default_language, label);
    entry.description = MultiLanguageString::create(default_language, description);
    m_codeMap.insert(std::make_pair(code, entry));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

bool DataQualityRegistry::addMapEntryAlias(const std::string& code, const std::string& codeAlias)
{
  try
  {
    if (!isMapEntry(code))
      return false;

    if (isMapEntryAlias(codeAlias))
    {
      throw SmartMet::Spine::Exception(BCP, "Dublicate alias '" + codeAlias + "' found!");
    }

    m_codeAliasMap[codeAlias] = code;

    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

bool DataQualityRegistry::getMapEntry(const std::string& code, struct MapEntry& entry)
{
  try
  {
    // Conversion from alias to key or just use the input.
    std::string realCode;
    if (!getKey(realCode, code))
      realCode = code;

    if (isMapEntryAlias(realCode))
    {
      realCode = m_codeAliasMap[realCode];
    }

    if (isMapEntry(realCode))
    {
      entry = m_codeMap[realCode];
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

void DataQualityRegistry::getMapKeyList(std::vector<std::string>& keyList)
{
  try
  {
    EntryMapType::iterator it;
    for (it = m_codeMap.begin(); it != m_codeMap.end(); ++it)
      keyList.push_back(it->first);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

bool DataQualityRegistry::getKey(std::string& code, const std::string& codeAlias)
{
  try
  {
    std::string tmpCode = codeAlias;
    std::size_t codeLength = tmpCode.length();
    if (m_keyConversionEnabled and codeLength > 1 and codeLength < 5)
    {
      std::string::iterator it;
      bool isNumber = true;

      // Old data quality codes are length of from 1 to 4 characters
      // We do support at the moment only lengths of 2, 3 and 4 because
      // all input quality codes must be quality checked. It means
      // that every code has at least two characters.
      for (it = tmpCode.begin(); it != tmpCode.end(); ++it)
        if (isdigit(*it) == 0)
          isNumber = false;

      // Convert only if it is a number.
      if (isNumber)
        tmpCode.replace(tmpCode.begin() + 1, tmpCode.end(), codeLength - 1, 'x');
    }

    if (isMapEntryAlias(tmpCode))
    {
      code = m_codeAliasMap[tmpCode];
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

bool DataQualityRegistry::isMapEntry(const std::string& code) const
{
  try
  {
    return !(m_codeMap.count(code) <= 0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

bool DataQualityRegistry::isMapEntryAlias(const std::string& codeAlias) const
{
  try
  {
    return !(m_codeAliasMap.count(codeAlias) <= 0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

void DataQualityRegistry::supportQualityCodeConversion(bool val)
{
  m_keyConversionEnabled = val;
}

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================

/**

@page META_PLUGIN_DATA_QUALITY_CONFIG Data quality code configuration

@section META_PLUGIN_DATA_QUALITY_CONFIG_PARAMETERS Data quality code configuration parameters

<table>
<tr><td>\b Parameter</td><td>\b Type</td><td>\b Use</td><td>\b Description</td></tr>
<tr><td>disabled</td><td>bool</td><td>optional (default false)</td><td>Specifies if a data quality
configuration file is disabled.</td></tr>
<tr><td>code</td><td>string</td><td>mandatory</td><td>Specifies a quality code.</td></tr>
<tr><td>alias</td><td>array of strings</td><td>optional</td><td>Specifies aliases of the quality
code.</td></tr>
<tr><td>label</td><td>label</td><td>mandatory</td><td>The label of the quality code in multiple
language. The default language defined in the main configuration must be included.</td></tr>
<tr><td>description</td><td>description</td><td>mandatory</td><td>Detailed description of the
quality code in multiple language. The default language defined in the main configuration must be
included.</td></tr>
</table>

<hr>

Quality code configuration example:

@verbatim
disabled = true;
code  = "9";
alias = ["9xxx"];
label:
{
    eng = "Erroneous value";
    fin = "Virheellinen arvo";
};
description:
{
    eng = "Manually quality controlled value. Value is completely erroneus. Value is completely
useless in every application and can be removed from database.";
    fin = "Manuaalisesti laatutarkastettu arvo. Arvo on varmasti täysin virheellinen. Arvo on täysin
käyttökelvoton kaikille sovelluksille ja se voidaan jatkossa kokonaan poistaa tietokannasta.";
};
@endverbatim

See also @subpage META_PLUGIN_MULTI_LANGUAGE_STRING
*/
