//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/Elle.hh
//
// created       julien quintard   [mon feb 16 19:12:33 2009]
// updated       julien quintard   [fri mar 26 14:47:01 2010]
//

#ifndef ELLE_ELLE_HH
#define ELLE_ELLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>

#include <elle/misc/Status.hh>

///
/// the main elle namespace
///
namespace elle
{
  using namespace core;
  using namespace misc;

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the main class which contains methods
  /// for controlling the elle library.
  ///
  /// these methods are obviously not thread safe and should therefore
  /// be called only once and probably from the main thread.
  ///
  class Elle
  {
  public:
    //
    // static methods
    //
    static Status	Initialize();
    static Status	Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>
#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/archive/Archive.hh>
#include <elle/factory/Factory.hh>
#include <elle/cryptography/Cryptography.hh>
#include <elle/network/Network.hh>
#include <elle/concurrency/Concurrency.hh>
#include <elle/idiom/Idom.hh>

#include <elle/Manifest.hh>

//
// ---------- namespaces ------------------------------------------------------
//

using namespace elle;
using namespace elle::io;
using namespace elle::core;
using namespace elle::misc;
using namespace elle::archive;
using namespace elle::factory;
using namespace elle::cryptography;
using namespace elle::network;
using namespace elle::concurrency;

#endif
