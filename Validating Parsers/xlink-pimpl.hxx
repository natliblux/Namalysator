// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef CXX_________DATA_SCHEMAS_XLINK_PIMPL_HXX
#define CXX_________DATA_SCHEMAS_XLINK_PIMPL_HXX

#include "xlink-pskel.hxx"

namespace xlink
{
  class show_pimpl: public virtual show_pskel,
    public ::xml_schema::string_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    post_show ();
  };

  class actuate_pimpl: public virtual actuate_pskel,
    public ::xml_schema::string_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    post_actuate ();
  };
}

#endif // CXX_________DATA_SCHEMAS_XLINK_PIMPL_HXX
