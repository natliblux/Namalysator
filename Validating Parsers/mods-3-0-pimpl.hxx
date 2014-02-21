// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef CXX_________DATA_SCHEMAS_MODS_3_0_PIMPL_HXX
#define CXX_________DATA_SCHEMAS_MODS_3_0_PIMPL_HXX

#include "mods-3-0-pskel.hxx"

#include "xml-pimpl.hxx"

#include "xlink-pimpl.hxx"

namespace mods
{
  namespace v3
  {
    class modsType_pimpl: public virtual modsType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      titleInfo ();

      virtual void
      name ();

      virtual void
      typeOfResource ();

      virtual void
      genre ();

      virtual void
      originInfo ();

      virtual void
      language ();

      virtual void
      physicalDescription ();

      virtual void
      abstract ();

      virtual void
      tableOfContents ();

      virtual void
      targetAudience ();

      virtual void
      note ();

      virtual void
      subject ();

      virtual void
      classification ();

      virtual void
      relatedItem ();

      virtual void
      identifier ();

      virtual void
      location ();

      virtual void
      accessCondition ();

      virtual void
      extension ();

      virtual void
      recordInfo ();

      virtual void
      ID (const ::std::string&);

      virtual void
      version ();

      virtual void
      post_modsType ();
    };

    class titleInfoType_pimpl: public virtual titleInfoType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      title ();

      virtual void
      subTitle ();

      virtual void
      partNumber ();

      virtual void
      partName ();

      virtual void
      nonSort ();

      virtual void
      ID (const ::std::string&);

      virtual void
      type ();

      virtual void
      authority ();

      virtual void
      displayLabel (const ::std::string&);

      virtual void
      type1 (const ::std::string&);

      virtual void
      href (const ::std::string&);

      virtual void
      role (const ::std::string&);

      virtual void
      arcrole (const ::std::string&);

      virtual void
      title1 (const ::std::string&);

      virtual void
      show ();

      virtual void
      actuate ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_titleInfoType ();
    };

    class nameType_pimpl: public virtual nameType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      namePart ();

      virtual void
      displayForm ();

      virtual void
      affiliation ();

      virtual void
      role ();

      virtual void
      description ();

      virtual void
      ID (const ::std::string&);

      virtual void
      type ();

      virtual void
      authority ();

      virtual void
      type1 (const ::std::string&);

      virtual void
      href (const ::std::string&);

      virtual void
      role1 (const ::std::string&);

      virtual void
      arcrole (const ::std::string&);

      virtual void
      title (const ::std::string&);

      virtual void
      show ();

      virtual void
      actuate ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_nameType ();
    };

    class namePartType_pimpl: public virtual namePartType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      type ();

      virtual void
      post_namePartType ();
    };

    class roleType_pimpl: public virtual roleType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      roleTerm ();

      virtual void
      post_roleType ();
    };

    class resourceType_pimpl: public virtual resourceType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_resourceType ();
    };

    class typeOfResourceType_pimpl: public virtual typeOfResourceType_pskel,
      public ::mods::v3::resourceType_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      collection ();

      virtual void
      manuscript ();

      virtual void
      post_typeOfResourceType ();
    };

    class genreType_pimpl: public virtual genreType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_genreType ();
    };

    class originInfoType_pimpl: public virtual originInfoType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      place ();

      virtual void
      publisher ();

      virtual void
      dateIssued ();

      virtual void
      dateCreated ();

      virtual void
      dateCaptured ();

      virtual void
      dateValid ();

      virtual void
      dateModified ();

      virtual void
      copyrightDate ();

      virtual void
      dateOther ();

      virtual void
      edition ();

      virtual void
      issuance ();

      virtual void
      frequency ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_originInfoType ();
    };

    class placeType_pimpl: public virtual placeType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      placeTerm ();

      virtual void
      post_placeType ();
    };

    class placeTermType_pimpl: public virtual placeTermType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      type ();

      virtual void
      post_placeTermType ();
    };

    class languageType_pimpl: public virtual languageType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      languageTerm ();

      virtual void
      post_languageType ();
    };

    class physicalDescriptionType_pimpl: public virtual physicalDescriptionType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      form ();

      virtual void
      reformattingQuality ();

      virtual void
      internetMediaType ();

      virtual void
      extent ();

      virtual void
      digitalOrigin ();

      virtual void
      note ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_physicalDescriptionType ();
    };

    class targetAudienceType_pimpl: public virtual targetAudienceType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_targetAudienceType ();
    };

    class subjectType_pimpl: public virtual subjectType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      topic ();

      virtual void
      geographic ();

      virtual void
      temporal ();

      virtual void
      titleInfo ();

      virtual void
      name ();

      virtual void
      geographicCode ();

      virtual void
      hierarchicalGeographic ();

      virtual void
      cartographics ();

      virtual void
      occupation ();

      virtual void
      ID (const ::std::string&);

      virtual void
      type (const ::std::string&);

      virtual void
      href (const ::std::string&);

      virtual void
      role (const ::std::string&);

      virtual void
      arcrole (const ::std::string&);

      virtual void
      title (const ::std::string&);

      virtual void
      show ();

      virtual void
      actuate ();

      virtual void
      authority ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_subjectType ();
    };

    class classificationType_pimpl: public virtual classificationType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      edition ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_classificationType ();
    };

    class relatedItemType_pimpl: public virtual relatedItemType_pskel,
      public ::mods::v3::modsType_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      part ();

      virtual void
      type ();

      virtual void
      displayLabel (const ::std::string&);

      virtual void
      type1 (const ::std::string&);

      virtual void
      href (const ::std::string&);

      virtual void
      role (const ::std::string&);

      virtual void
      arcrole (const ::std::string&);

      virtual void
      title (const ::std::string&);

      virtual void
      show ();

      virtual void
      actuate ();

      virtual void
      post_relatedItemType ();
    };

    class partType_pimpl: public virtual partType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      detail ();

      virtual void
      extent ();

      virtual void
      date ();

      virtual void
      text ();

      virtual void
      post_partType ();
    };

    class detailType_pimpl: public virtual detailType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      number ();

      virtual void
      caption ();

      virtual void
      title ();

      virtual void
      type ();

      virtual void
      level (unsigned long long);

      virtual void
      post_detailType ();
    };

    class extentType_pimpl: public virtual extentType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      start ();

      virtual void
      end ();

      virtual void
      total (unsigned long long);

      virtual void
      list ();

      virtual void
      unit ();

      virtual void
      post_extentType ();
    };

    class identifierType_pimpl: public virtual identifierType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      type ();

      virtual void
      displayLabel ();

      virtual void
      invalid ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_identifierType ();
    };

    class locationType_pimpl: public virtual locationType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      physicalLocation ();

      virtual void
      url ();

      virtual void
      post_locationType ();
    };

    class extensionType_pimpl: public virtual extensionType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      post_extensionType ();
    };

    class recordInfoType_pimpl: public virtual recordInfoType_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      recordContentSource ();

      virtual void
      recordCreationDate ();

      virtual void
      recordChangeDate ();

      virtual void
      recordIdentifier ();

      virtual void
      languageOfCataloging ();

      virtual void
      recordOrigin ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_recordInfoType ();
    };

    class unstructuredText_pimpl: public virtual unstructuredText_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      type (const ::std::string&);

      virtual void
      href (const ::std::string&);

      virtual void
      role (const ::std::string&);

      virtual void
      arcrole (const ::std::string&);

      virtual void
      title (const ::std::string&);

      virtual void
      show ();

      virtual void
      actuate ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      displayLabel (const ::std::string&);

      virtual void
      type1 (const ::std::string&);

      virtual void
      post_unstructuredText ();
    };

    class baseDateType_pimpl: public virtual baseDateType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      encoding ();

      virtual void
      qualifier ();

      virtual void
      point ();

      virtual void
      post_baseDateType ();
    };

    class dateType_pimpl: public virtual dateType_pskel,
      public ::mods::v3::baseDateType_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      keyDate ();

      virtual void
      post_dateType ();
    };

    class sourceType_pimpl: public virtual sourceType_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      lang ();

      virtual void
      lang1 (const ::std::string&);

      virtual void
      script ();

      virtual void
      transliteration ();

      virtual void
      post_sourceType ();
    };

    class codeOrText_pimpl: public virtual codeOrText_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_codeOrText ();
    };

    class placeAuthority_pimpl: public virtual placeAuthority_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_placeAuthority ();
    };

    class modsCollection_pimpl: public virtual modsCollection_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      mods ();

      virtual void
      post_modsCollection ();
    };

    class version_pimpl: public virtual version_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_version ();
    };

    class type_pimpl: public virtual type_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_type ();
    };

    class type1_pimpl: public virtual type1_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_type1 ();
    };

    class type2_pimpl: public virtual type2_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_type2 ();
    };

    class roleTerm_pimpl: public virtual roleTerm_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      type ();

      virtual void
      post_roleTerm ();
    };

    class collection_pimpl: public virtual collection_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_collection ();
    };

    class manuscript_pimpl: public virtual manuscript_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_manuscript ();
    };

    class issuance_pimpl: public virtual issuance_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_issuance ();
    };

    class languageTerm_pimpl: public virtual languageTerm_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      type ();

      virtual void
      post_languageTerm ();
    };

    class form_pimpl: public virtual form_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      post_form ();
    };

    class reformattingQuality_pimpl: public virtual reformattingQuality_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_reformattingQuality ();
    };

    class digitalOrigin_pimpl: public virtual digitalOrigin_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_digitalOrigin ();
    };

    class geographicCode_pimpl: public virtual geographicCode_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      authority ();

      virtual void
      post_geographicCode ();
    };

    class hierarchicalGeographic_pimpl: public virtual hierarchicalGeographic_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      continent ();

      virtual void
      country ();

      virtual void
      province ();

      virtual void
      region ();

      virtual void
      state ();

      virtual void
      territory ();

      virtual void
      county ();

      virtual void
      city ();

      virtual void
      island ();

      virtual void
      area ();

      virtual void
      post_hierarchicalGeographic ();
    };

    class cartographics_pimpl: public virtual cartographics_pskel
    {
      public:
      virtual void
      pre ();

      virtual void
      scale ();

      virtual void
      projection ();

      virtual void
      coordinates ();

      virtual void
      post_cartographics ();
    };

    class type3_pimpl: public virtual type3_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_type3 ();
    };

    class invalid_pimpl: public virtual invalid_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_invalid ();
    };

    class physicalLocation_pimpl: public virtual physicalLocation_pskel,
      public ::mods::v3::sourceType_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      displayLabel ();

      virtual void
      post_physicalLocation ();
    };

    class url_pimpl: public virtual url_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      dateLastAccessed ();

      virtual void
      displayLabel ();

      virtual void
      post_url ();
    };

    class recordIdentifier_pimpl: public virtual recordIdentifier_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      source ();

      virtual void
      post_recordIdentifier ();
    };

    class encoding_pimpl: public virtual encoding_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_encoding ();
    };

    class qualifier_pimpl: public virtual qualifier_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_qualifier ();
    };

    class point_pimpl: public virtual point_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_point ();
    };

    class keyDate_pimpl: public virtual keyDate_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_keyDate ();
    };

    class authority_pimpl: public virtual authority_pskel,
      public ::xml_schema::string_pimpl
    {
      public:
      virtual void
      pre ();

      virtual void
      post_authority ();
    };
  }
}

#endif // CXX_________DATA_SCHEMAS_MODS_3_0_PIMPL_HXX
