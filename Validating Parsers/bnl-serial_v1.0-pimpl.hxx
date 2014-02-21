// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef CXX___BNL_SERIAL_V1_0_PIMPL_HXX
#define CXX___BNL_SERIAL_V1_0_PIMPL_HXX

#include "bnl-serial_v1.0-pskel.hxx"

namespace bnl_serial_v1_0
{

	class ct_HIERARCHY_pimpl: public virtual ct_HIERARCHY_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  FRONT ();

	  virtual void
	  ISSUE ();

	  virtual void
	  BACK ();

	  virtual void
	  post_ct_HIERARCHY ();
	};

	class ct_FRONT_pimpl: public virtual ct_FRONT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TITLE_SECTION ();

	  virtual void
	  STATEMENT_SECTION ();

	  virtual void
	  PICTURE_SECTION ();

	  virtual void
	  FRONTISPIECE ();

	  virtual void
	  COVER_SECTION ();

	  virtual void
	  BASTARD_TITLE_SECTION ();

	  virtual void
	  TABLE_OF_CONTENTS ();

	  virtual void
	  REFERENCE_LIST ();

	  virtual void
	  LIST_OF_PUBLICATIONS ();

	  virtual void
	  LIST_OF_FIGURES ();

	  virtual void
	  INDEX ();

	  virtual void
	  CORRECTIONS ();

	  virtual void
	  BIBLIOGRAPHY ();

	  virtual void
	  ABBREVIATIONS ();

	  virtual void
	  RIDAS ();

	  virtual void
	  PRODUCTION_NOTE ();

	  virtual void
	  PREFACE ();

	  virtual void
	  NECROLOGY ();

	  virtual void
	  INTRODUCTION ();

	  virtual void
	  DEDICATION ();

	  virtual void
	  CHAPTER ();

	  virtual void
	  ACKNOWLEDGEMENTS ();

	  virtual void
	  ABSTRACT ();

	  virtual void
	  ADVERTISEMENT ();

	  virtual void
	  post_ct_FRONT ();
	};

	class ct_MAIN_pimpl: public virtual ct_MAIN_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TABLE_OF_CONTENTS ();

	  virtual void
	  REFERENCE_LIST ();

	  virtual void
	  LIST_OF_PUBLICATIONS ();

	  virtual void
	  LIST_OF_FIGURES ();

	  virtual void
	  LIST ();

	  virtual void
	  INDEX ();

	  virtual void
	  CORRECTIONS ();

	  virtual void
	  BIBLIOGRAPHY ();

	  virtual void
	  ABBREVIATIONS ();

	  virtual void
	  RIDAS ();

	  virtual void
	  PRODUCTION_NOTE ();

	  virtual void
	  PREFACE ();

	  virtual void
	  POSTFACE ();

	  virtual void
	  POEM ();

	  virtual void
	  NOTES ();

	  virtual void
	  NECROLOGY ();

	  virtual void
	  INTRODUCTION ();

	  virtual void
	  GROUP ();

	  virtual void
	  DEDICATION ();

	  virtual void
	  DEBATE ();

	  virtual void
	  CONTRIBUTION ();

	  virtual void
	  CHAPTER ();

	  virtual void
	  BOOK_REVIEW ();

	  virtual void
	  APPENDIX ();

	  virtual void
	  ACKNOWLEDGEMENTS ();

	  virtual void
	  ABSTRACT ();

	  virtual void
	  ADVERTISEMENT ();

	  virtual void
	  post_ct_MAIN ();
	};

	class ct_BACK_pimpl: public virtual ct_BACK_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  STATEMENT_SECTION ();

	  virtual void
	  PICTURE_SECTION ();

	  virtual void
	  COVER_SECTION ();

	  virtual void
	  TABLE_OF_CONTENTS ();

	  virtual void
	  REFERENCE_LIST ();

	  virtual void
	  LIST_OF_PUBLICATIONS ();

	  virtual void
	  LIST_OF_FIGURES ();

	  virtual void
	  LIST ();

	  virtual void
	  INDEX ();

	  virtual void
	  CORRECTIONS ();

	  virtual void
	  BIBLIOGRAPHY ();

	  virtual void
	  ABBREVIATIONS ();

	  virtual void
	  RIDAS ();

	  virtual void
	  POSTFACE ();

	  virtual void
	  NECROLOGY ();

	  virtual void
	  CHAPTER ();

	  virtual void
	  APPENDIX ();

	  virtual void
	  ACKNOWLEDGEMENTS ();

	  virtual void
	  ADVERTISEMENT ();

	  virtual void
	  post_ct_BACK ();
	};

	class ct_ISSUE_pimpl: public virtual ct_ISSUE_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  MAIN ();

	  virtual void
	  FRONT ();

	  virtual void
	  BACK ();

	  virtual void
	  post_ct_ISSUE ();
	};

	class ct_PAGE_LIKE_pimpl: public virtual ct_PAGE_LIKE_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  CONTENT ();

	  virtual void
	  post_ct_PAGE_LIKE ();
	};

	class ct_TITLE_SECTION_pimpl: public virtual ct_TITLE_SECTION_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TITLE_OF_WORK ();

	  virtual void
	  CREATOR_OF_WORK ();

	  virtual void
	  PUBLISHING_STMT ();

	  virtual void
	  STATEMENT ();

	  virtual void
	  CONTENT ();

	  virtual void
	  post_ct_TITLE_SECTION ();
	};

	class ct_LIST_LIKE_pimpl: public virtual ct_LIST_LIKE_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  CAPTION ();

	  virtual void
	  DATA ();

	  virtual void
	  TABLE_OF_CONTENTS ();

	  virtual void
	  REFERENCE_LIST ();

	  virtual void
	  LIST_OF_PUBLICATIONS ();

	  virtual void
	  LIST_OF_FIGURES ();

	  virtual void
	  LIST ();

	  virtual void
	  INDEX ();

	  virtual void
	  CORRECTIONS ();

	  virtual void
	  BIBLIOGRAPHY ();

	  virtual void
	  ABBREVIATIONS ();

	  virtual void
	  ITEM ();

	  virtual void
	  post_ct_LIST_LIKE ();
	};

	class ct_ITEM_pimpl: public virtual ct_ITEM_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXTBLOCK ();

	  virtual void
	  PAGE_NUMBER ();

	  virtual void
	  ITEM_CAPTION ();

	  virtual void
	  SUBHEADLINE ();

	  virtual void
	  HEADLINE ();

	  virtual void
	  AUTHOR ();

	  virtual void
	  ITEM ();

	  virtual void
	  post_ct_ITEM ();
	};

	class ct_HEADING_pimpl: public virtual ct_HEADING_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TITLE ();

	  virtual void
	  SUBTITLE ();

	  virtual void
	  AUTHOR ();

	  virtual void
	  MOTTO ();

	  virtual void
	  HEADING_TEXT ();

	  virtual void
	  post_ct_HEADING ();
	};

	class ct_CHAPTER_LIKE_pimpl: public virtual ct_CHAPTER_LIKE_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  HEADING ();

	  virtual void
	  ABSTRACT ();

	  virtual void
	  BODY ();

	  virtual void
	  TABLE_OF_CONTENTS ();

	  virtual void
	  REFERENCE_LIST ();

	  virtual void
	  LIST_OF_PUBLICATIONS ();

	  virtual void
	  LIST_OF_FIGURES ();

	  virtual void
	  INDEX ();

	  virtual void
	  CORRECTIONS ();

	  virtual void
	  BIBLIOGRAPHY ();

	  virtual void
	  ABBREVIATIONS ();

	  virtual void
	  RIDAS ();

	  virtual void
	  PRODUCTION_NOTE ();

	  virtual void
	  PREFACE ();

	  virtual void
	  POSTFACE ();

	  virtual void
	  POEM ();

	  virtual void
	  NOTES ();

	  virtual void
	  NECROLOGY ();

	  virtual void
	  INTRODUCTION ();

	  virtual void
	  GROUP ();

	  virtual void
	  DEDICATION ();

	  virtual void
	  DEBATE ();

	  virtual void
	  CONTRIBUTION ();

	  virtual void
	  CHAPTER ();

	  virtual void
	  BOOK_REVIEW ();

	  virtual void
	  APPENDIX ();

	  virtual void
	  ACKNOWLEDGEMENTS ();

	  virtual void
	  MAP_GROUP ();

	  virtual void
	  ILLUSTRATION_GROUP ();

	  virtual void
	  CHART_DIAGRAM_GROUP ();

	  virtual void
	  TABLE_GROUP ();

	  virtual void
	  post_ct_CHAPTER_LIKE ();
	};

	class ct_MAP_GROUP_pimpl: public virtual ct_MAP_GROUP_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  MAP ();

	  virtual void
	  post_ct_MAP_GROUP ();
	};

	class ct_ILLUSTRATION_GROUP_pimpl: public virtual ct_ILLUSTRATION_GROUP_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  ILLUSTRATION ();

	  virtual void
	  post_ct_ILLUSTRATION_GROUP ();
	};

	class ct_CHART_DIAGRAM_GROUP_pimpl: public virtual ct_CHART_DIAGRAM_GROUP_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  CHART_DIAGRAM ();

	  virtual void
	  post_ct_CHART_DIAGRAM_GROUP ();
	};

	class ct_TABLE_GROUP_pimpl: public virtual ct_TABLE_GROUP_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TABLE ();

	  virtual void
	  post_ct_TABLE_GROUP ();
	};

	class ct_ILLUSTRATION_LIKE_STRUCT_pimpl: public virtual ct_ILLUSTRATION_LIKE_STRUCT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  IMAGE ();

	  virtual void
	  CAPTION ();

	  virtual void
	  post_ct_ILLUSTRATION_LIKE_STRUCT ();
	};

	class ct_TABLE_LIKE_STRUCT_pimpl: public virtual ct_TABLE_LIKE_STRUCT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  IMAGE ();

	  virtual void
	  CAPTION ();

	  virtual void
	  post_ct_TABLE_LIKE_STRUCT ();
	};

	class ct_BODY_pimpl: public virtual ct_BODY_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  BODY_CONTENT ();

	  virtual void
	  FOOTNOTE ();

	  virtual void
	  MAP ();

	  virtual void
	  ILLUSTRATION ();

	  virtual void
	  CHART_DIAGRAM ();

	  virtual void
	  BOOKPLATE ();

	  virtual void
	  BARCODE ();

	  virtual void
	  TABLE ();

	  virtual void
	  LIST ();

	  virtual void
	  post_ct_BODY ();
	};

	class ct_BODY_CONTENT_pimpl: public virtual ct_BODY_CONTENT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXTBLOCK ();

	  virtual void
	  PARAGRAPH ();

	  virtual void
	  FORMULA ();

	  virtual void
	  post_ct_BODY_CONTENT ();
	};

	class ct_FORMULA_pimpl: public virtual ct_FORMULA_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  IMAGE ();

	  virtual void
	  post_ct_FORMULA ();
	};

	class ct_PARAGRAPH_pimpl: public virtual ct_PARAGRAPH_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXT ();

	  virtual void
	  MARGINALIA ();

	  virtual void
	  post_ct_PARAGRAPH ();
	};

	class ct_FOOTNOTE_PARAGRAPH_pimpl: public virtual ct_FOOTNOTE_PARAGRAPH_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXT ();

	  virtual void
	  post_ct_FOOTNOTE_PARAGRAPH ();
	};

	class ct_FINAL_pimpl: public virtual ct_FINAL_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  fptr ();

	  virtual void
	  post_ct_FINAL ();
	};

	class ct_FPTR_pimpl: public virtual ct_FPTR_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  area ();

	  virtual void
	  seq ();

	  virtual void
	  par ();

	  virtual void
	  post_ct_FPTR ();
	};

	class ct_SEQ_pimpl: public virtual ct_SEQ_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  area ();

	  virtual void
	  post_ct_SEQ ();
	};

	class Serial_pimpl: public virtual Serial_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  VOLUME ();

	  virtual void
	  post_Serial ();
	};

	class CONTENT_pimpl: public virtual CONTENT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXTBLOCK ();

	  virtual void
	  TABLE ();

	  virtual void
	  PUBLISHING_STMT ();

	  virtual void
	  PARAGRAPH ();

	  virtual void
	  MAP ();

	  virtual void
	  ILLUSTRATION ();

	  virtual void
	  CHART_DIAGRAM ();

	  virtual void
	  BOOKPLATE ();

	  virtual void
	  BARCODE ();

	  virtual void
	  SUBHEADLINE ();

	  virtual void
	  HEADLINE ();

	  virtual void
	  FORMULA ();

	  virtual void
	  FOOTNOTE ();

	  virtual void
	  AUTHOR ();

	  virtual void
	  post_CONTENT ();
	};

	class CONTENT1_pimpl: public virtual CONTENT1_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TABLE ();

	  virtual void
	  MAP ();

	  virtual void
	  ILLUSTRATION ();

	  virtual void
	  CHART_DIAGRAM ();

	  virtual void
	  BOOKPLATE ();

	  virtual void
	  BARCODE ();

	  virtual void
	  FORMULA ();

	  virtual void
	  post_CONTENT1 ();
	};

	class DATA_pimpl: public virtual DATA_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXTBLOCK ();

	  virtual void
	  PAGE_NUMBER ();

	  virtual void
	  ITEM_CAPTION ();

	  virtual void
	  AUTHOR ();

	  virtual void
	  post_DATA ();
	};

	class MOTTO_pimpl: public virtual MOTTO_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXT ();

	  virtual void
	  post_MOTTO ();
	};

	class HEADING_TEXT_pimpl: public virtual HEADING_TEXT_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  TEXT ();

	  virtual void
	  post_HEADING_TEXT ();
	};

	class par_pimpl: public virtual par_pskel
	{
	  public:
	  virtual void
	  pre ();

	  virtual void
	  area ();

	  virtual void
	  seq ();

	  virtual void
	  post_par ();
	};

}
#endif // CXX___BNL_SERIAL_V1_0_PIMPL_HXX
