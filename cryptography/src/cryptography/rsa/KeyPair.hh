#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HH

# include <cryptography/fwd.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/rsa/KeyPair.hh>
# include <cryptography/rsa/defaults.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

# include <elle/types.hh>
# include <elle/serialization/Serializer.hh>

# include <iosfwd>

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
#  include <elle/concept/Uniquable.hh>
#  include <elle/serialize/construct.hh>
# endif

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a cryptographic key pair _i.e_ a pair of public and
      /// private keys.
      ///
      /// Note that the public key is always written as a capital 'K'
      /// while a private key is noted with a lower-case 'k'.
      class KeyPair:
        public elle::Printable
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , public elle::serialize::DynamicFormat<KeyPair>
        , public elle::concept::MakeUniquable<KeyPair>
# endif
      {
      public:
        /*-------------.
        | Construction |
        `-------------*/
      public:
        KeyPair(PublicKey const& K,
                PrivateKey const& k);
        KeyPair(PublicKey&& K,
                PrivateKey&& k);
        KeyPair(KeyPair const& other);
        KeyPair(KeyPair&& other);
        virtual
        ~KeyPair() = default;

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Return the public key.
        PublicKey const&
        K() const;
        /// Return the private key.
        PrivateKey const&
        k() const;
        /// Return the key pair's size in bytes.
        elle::Natural32
        size() const;
        /// Return the key pair's length in bits.
        elle::Natural32
        length() const;

# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /*---------.
        | Rotation |
        `---------*/
      public:
        explicit
        /// Deduce a keypair based out of the given seed.
        KeyPair(Seed const& seed,
                Padding const encryption_padding = defaults::encryption_padding,
                Padding const signature_padding = defaults::signature_padding,
                Oneway const digest_algorithm = defaults::digest_algorithm,
                Cipher const envelope_cipher = defaults::envelope_cipher,
                Mode const envelope_mode = defaults::envelope_mode);
# endif

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(KeyPair const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(KeyPair);

        /*----------.
        | Printable |
        `----------*/
      public:
        void
        print(std::ostream& stream) const override;

        /*--------------.
        | Serialization |
        `--------------*/
      public:
        KeyPair(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        /// The public key.
        ELLE_ATTRIBUTE(std::unique_ptr<PublicKey>, K);
        /// The private key.
        ELLE_ATTRIBUTE(std::unique_ptr<PrivateKey>, k);

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        /*-------.
        | Legacy |
        `-------*/
      public:
        // construction
        KeyPair() {}
        ELLE_SERIALIZE_CONSTRUCT(KeyPair)
        {}
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(KeyPair);
        using elle::serialize::SerializableMixin<
          infinit::cryptography::rsa::KeyPair,
          elle::serialize::Base64Archive>::serialize;
# endif
      };
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated RSA key pair.
        ///
        /// Note that the length is in bits.
        KeyPair
        generate(elle::Natural32 const length,
                 Padding const encryption_padding =
                   defaults::encryption_padding,
                 Padding const signature_padding =
                   defaults::signature_padding,
                 Oneway const digest_algorithm =
                   defaults::digest_algorithm,
                 Cipher const envelope_cipher =
                   defaults::envelope_cipher,
                 Mode const envelope_mode =
                   defaults::envelope_mode);
      }
    }
  }
}

# include <cryptography/rsa/KeyPair.hxx>

#endif
