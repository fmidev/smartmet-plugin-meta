#ifndef SMARTMET_META_PLUGIN_DATA_QUALITY_H
#define SMARTMET_META_PLUGIN_DATA_QUALITY_H

#include <map>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "MultiLanguageString.h"

namespace SmartMet
{
namespace Plugin
{
namespace Meta
{
class DataQualityRegistry : protected virtual boost::noncopyable
{
 public:
  struct MapEntry
  {
    MultiLanguageStringP label;
    MultiLanguageStringP description;
  };

  typedef std::map<std::string, MapEntry> EntryMapType;
  typedef std::map<std::string, std::string> AliasMapType;

 public:
  DataQualityRegistry();
  ~DataQualityRegistry();

  /**
   *  \brief Set data quality information for a data quality code.
   *  \param[in] code Data quality code that the information contains.
   *  \param[in] default_language Default language of the map entry.
   *  \param[in] label Code label in multiple languages.
   *  \param[in] description Code description in multiple languages.
   */
  void addMapEntry(const std::string& code,
                   const std::string& default_language,
                   libconfig::Setting& label,
                   libconfig::Setting& description);

  /**
   *  \brief Set an alias of a data quality code.
   *  \param[in] code Data quality code that the information contains.
   *  \param[in] codeAlias Data quality code that is alias of the code
   *             that the information contains.
   */
  bool addMapEntryAlias(const std::string& code, const std::string& codeAlias);

  /**
   *  \brief Get data quality information of a data quality code.
   *  \param[in] code Data quality code.
   *  \param[out] entry Data quality information entry.
   *  \return false if code not found.
   */
  bool getMapEntry(const std::string& code, struct MapEntry& entry);

  /**
   *  \brief Get data quality code list.
   *  \param[out] keyList Data quality code list.
   */
  void getMapKeyList(std::vector<std::string>& keyList);

  /**
   *  \brief Conversion from alias to key.
   *  If quality code conversion is enabled the conversion is done with
   *  the modified way explained in comments of
   *  supportQualityCodeConversion method.
   *
   *  \param [out] key Result of the conversion.
   *  \param [in] keyAlias Alias Data quality code that is alias of the code
   *              that the information contains.
   *  \return false if conversion fail.
   */
  bool getKey(std::string& key, const std::string& keyAlias);

  /**
   *  \brief Enable or disable key conversion.
   *
   *  Key conversion can be enabled or disabled by using this method.
   *  Key conversion is done from an old observation quality code
   *  to a new code. The new code is one character of length and
   *  old key is 1, 2, 3, or 4 character(s) of length. This class
   *  support only conversion from 2, 3 and 4 character(s).
   *  If the key conversion is disabled aliases are converted as
   *  they are.
   *
   *  Conversion is done by the following way (written in finnish)
   *
   *  DATA_QUALITY-laatukoodit ovat uusia yksinumeroisia koodeja välillä [0,9]
   *
   *  Lippujen konversio laatukoodeiksi.
   *  F on yksi-nelinumeroisen lipun ensimmäinen eli merkittävin numero.
   *  Sarakkeissa f, fx, fxx ja fxxx kuvataan minkä arvon
   *  DATA_QUALITY-laatukoodi saa kun F esiintyy (F=f) eri kohdissa
   *  nelinumeroista lippua. Lipun paikka kertoo mistä QC-tarkastuksesta
   *  on kyse: f on QC0-lippu (asemalla tehtävä), fx on QC1-lippu, fxx
   *  on QC2-lippu (ei käytössä) ja fxxx on HQC-lippu).
   *
   * F      f   fx  fxx fxxx   Nykyinen lippu    Uusi laadun kuvaus
   * -   ---- ---- ---- ----   ----------------  -------------------
   * 0      0                  No checked        Tuntematon laatu
   * 1      0    1    1    2   Checked and OK    Hyvä laatu
   * 2      3    3    3    4   Small difference  Hyväksytty laatu
   * 3      6    6    6    7   Big difference    Ei-hyväksytty laatu
   * 4      3    3    3    4   Calculated        Hyväksytty laatu
   * 5      3    3    3    4   Estimated         Hyväksytty laatu
   * 6      0    0    0    0   Not used          Tuntematon laatu
   * 7      0    0    0    0   Not used          Tuntematon laatu
   * 8      5    5    5    5   Missing           Puuttuva arvo
   * 9      8    8    8    9   Deleted           Virheellinen
   *
   * So, if alias key (code) is for example 3xxx it match with
   * the new quality code 7 or if the alias key is 3x it match
   * with the new quality code 6. x characters in alias keys
   * are handled as any number between 0 and 9 inclusively.
   * For example 3xxx is handled same way as 3023 or 3020.
   *
   * By default the class do not support the quality
   * key conversion.
   * \param[in] val true (enable) or false (disable).
   */
  void supportQualityCodeConversion(bool val);

 private:
  /**
   *  \brief Test if a quality code is set into registry.
   *  \param[in] code Data quality code in test.
   *  \return false if code not found.
   */
  bool isMapEntry(const std::string& code) const;

  /**
   *  \brief Test if quality code is an alias of a code set into registry.
   *  \param[in] codeAlias Data quality code in test.
   *  \return false if code not found.
   */
  bool isMapEntryAlias(const std::string& codeAlias) const;

 private:
  // Data Quality code collection.
  EntryMapType m_codeMap;
  AliasMapType m_codeAliasMap;
  bool m_keyConversionEnabled;
};

}  // namespace Meta
}  // namespace Plugin
}  // namespace SmartMet

#endif  // SMARTMET_META_PLUGIN_DATA_QUALITY_H
