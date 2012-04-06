#ifndef ELLE_JSON_BOOL_HH
# define ELLE_JSON_BOOL_HH

# include "Object.hh"
# include "_detail.hh"

namespace elle { namespace format { namespace json {

    typedef detail::BasicObject<bool> Bool;

    extern Bool const true_;
    extern Bool const false_;

}}} // !namespace elle::format::json

#endif /* ! BOOL_HH */


