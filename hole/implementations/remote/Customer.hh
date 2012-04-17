//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [sun aug 28 17:49:10 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this class represents a client being connected to the server
      /// i.e the machine.
      ///
      class Customer:
        public elle::Entity
      {
      public:
        //
        // constants
        //
        static const elle::Natural32            Timeout;

        //
        // enumerations
        //
        enum State
          {
            StateUnknown,
            StateConnected,
            StateAuthenticated,
            StateDead
          };

        //
        // constructors & destructors
        //
        Customer();
        ~Customer();

        //
        // methods
        //
        elle::Status            Create(elle::network::TCPSocket*);

        //
        // callbacks
        //
        elle::Status            Disconnected();
        elle::Status            Error(const elle::String&);

        elle::Status            Abort();

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // signals
        //
        struct
        {
          elle::concurrency::Signal<
            elle::Parameters<
              Customer*
              >
            >                   dead;
        }                       signal;

        //
        // attributes
        //
        State                   state;

        elle::network::TCPSocket*        socket;
        elle::concurrency::Timer*            timer;
      };

    }
  }
}

#endif
