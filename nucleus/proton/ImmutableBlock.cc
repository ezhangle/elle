#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/log.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.ImmutableBlock");

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    ImmutableBlock::ImmutableBlock():
      Block()
    {
    }

    ImmutableBlock::ImmutableBlock(Network const& network,
                                   Family const family,
                                   neutron::Component const component,
                                   elle::cryptography::PublicKey const& creator_K):
      Block(network, family, component, creator_K)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::io::Path
    ImmutableBlock::_path(Address const& address)
    {
      return (elle::io::Path(
                lune::Lune::Network::Shelter::ImmutableBlock,
                elle::io::Piece("%NETWORK%", address.network().name()),
                elle::io::Piece("%ADDRESS%", address.unique())));
    }

//
// ---------- fileable --------------------------------------------------------
//

    void
    ImmutableBlock::load(Address const& address)
    {
      this->load(ImmutableBlock::_path(address));
    }

    void
    ImmutableBlock::store(Address const& address) const
    {
      this->store(ImmutableBlock::_path(address));
    }

    void
    ImmutableBlock::erase(Address const& address)
    {
      elle::concept::Fileable<>::erase(
        ImmutableBlock::_path(address));
    }

    elle::Boolean
    ImmutableBlock::exists(Address const& address)
    {
      return (elle::concept::Fileable<>::exists(
        ImmutableBlock::_path(address)));
    }

  }
}
