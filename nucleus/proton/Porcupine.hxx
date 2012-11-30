#ifndef NUCLEUS_PROTON_PORCUPINE_HXX
# define NUCLEUS_PROTON_PORCUPINE_HXX

# include <nucleus/Exception.hh>
# include <nucleus/proton/Ambit.hh>
# include <nucleus/proton/Seam.hh>
# include <nucleus/proton/Quill.hh>
# include <nucleus/proton/Nest.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Statistics.hh>

# include <elle/assert.hh>
# include <elle/log.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    template <typename T>
    void
    Porcupine::add(std::unique_ptr<T>&& v)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("add(%s)", *v);

      std::unique_ptr<Contents> contents(new Contents(v.release()));
      Handle handle(this->nest().attach(std::move(contents)));
      Ambit<T> value(this->nest(), handle);

      value.load();

      this->add(value().mayor(), value.handle());

      value.unload();
    }

    template <typename T>
    elle::Boolean
    Porcupine::exist(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("exist(%s)", k);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            return false;
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            try
              {
                Ambit<T> value(this->nest(), this->_root);

                value.load();

                if (value().exist(k) == false)
                  return false;

                value.unload();
              }
            catch (...)
              {
                return false;
              }
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            try
              {
                // search for the quill responsible for the given key.
                // XXX

                Handle handle(this->lookup<T>(k));
                Ambit<T> value(this->nest(), handle);

                value.load();

                if (value().exist(k) == false)
                  return false;

                value.unload();
              }
            catch (...)
              {
                return false;
              }
          }
        }

      return true;
    }

    template <typename T>
    Handle
    Porcupine::lookup(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      Handle v;

      ELLE_TRACE_SCOPE("lookup(%s)", k);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            this->_create<T>();

            //
            // do not break and proceed with the lookup.
            //
            ELLE_ASSERT(this->_mode == Porcupine::Mode::single);
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            // XXX verify that the key is valid.

            // return the unique value block.
            v = this->_root;

            break;
          }
        case Mode::hierarchy:
          {
            Handle handle;

            ELLE_TRACE_SCOPE("Mode::hierarchy");

            // search for the quill responsible for the given key.
            handle = this->_search<T>(k);

            Ambit<Quill<T>> quill(this->nest(), handle);

            // load the quill.
            quill.load();

            // retrieve the handle of the looked up value.
            v = quill().lookup_handle(k);

            // unload the quill.
            quill.unload();

            break;
          }
        }

      return (v);
    }

    template <typename T>
    Handle
    Porcupine::seek(Capacity const target,
                    Capacity& base)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      Handle v;

      ELLE_TRACE_SCOPE("seek(%s)", target);

      // initialize the base.
      base = 0;

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            this->_create<T>();

            //
            // do not break and proceed with the lookup.
            //
            ELLE_ASSERT(this->_mode == Porcupine::Mode::single);
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            // verify the validity of the target index.
            if (target >= this->_capacity)
              throw Exception("the given target index '%s' exceeds the "
                              "capacity '%s' of the value",
                              target, this->_capacity);

            // return the value block with a base of zero since it
            // is the first and unique block.
            v = this->_root;

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root.
            root.load();

            // seek down the tree.
            v = root().seek(target, base);

            // unload the root.
            root.unload();

            break;
          }
        }

      return (v);
    }

    template <typename T>
    void
    Porcupine::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("update(%s)", k);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            throw Exception("unable to update an empty porcupine");
            break;
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            Ambit<T> value(this->nest(), this->_root);

            // load the value.
            value.load();

            // XXX verify that the key is valid.

            // update the capacity state.
            this->_capacity = value().capacity();
            this->_state = value().state();

            // unload the value.
            value.unload();

            // try to optimize the tree.
            this->_optimize<T>();

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root nodule.
            root.load();

            // recursively update the porcupine from the given key.
            root().update(k);

            // update the capacity and state.
            this->_capacity = root().capacity();
            this->_state = root().state();

            // unload the root nodule.
            root.unload();

            // try to optimize the tree.
            this->_optimize<T>();

            break;
          }
        }
    }

    template <typename T>
    void
    Porcupine::remove(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("remove(%s)", k);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            throw Exception("unable to remove from an empty porcupine");
            break;
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            // XXX verify that the key is valid.

            // detach the root nodule from the porcupine.
            this->nest().detach(this->_root);

            // set the new tree's root handle.
            this->_root = Handle::Null;

            // set the mode.
            this->_mode = Porcupine::Mode::empty;

            // set the capacity and state.
            this->_capacity = 0;
            this->_state = State::dirty;

            // make sure the tree is now empty.
            ELLE_ASSERT(this->_height == 0);

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root nodule.
            root.load();

            // recursively remove the element from the root.
            root().remove(k);

            // update the capacity and state.
            this->_capacity = root().capacity();
            this->_state = root().state();

            // unload the root nodule.
            root.unload();

            // try to optimize the tree.
            this->_optimize<T>();

            break;
          }
        }
    }

    template <typename T>
    void
    Porcupine::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_DEBUG_SCOPE("check(%s)", flags);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_DEBUG_SCOPE("Mode::empty");

            break;
          }
        case Mode::single:
          {
            Ambit<T> value(this->nest(), this->_root);

            ELLE_DEBUG_SCOPE("Mode::single");

            // load the value nodule.
            value.load();

            // check the address, if required.
            if (flags & flags::address)
              {
                // bind the current block.
                Address address{value.contents().bind()};

                // compare the addresses.
                if (this->_root.address() != address)
                  throw Exception("invalid address: root(%s) versus bind(%s)",
                                  this->_root.address(), address);
              }

            // check the capacity.
            if (flags & flags::capacity)
              {
                if (this->_capacity != value().capacity())
                  throw Exception("invalid capacity: this(%s) versus value(%s)",
                                  this->_capacity, value().capacity());
              }

            // check the footprint.
            if (flags & flags::footprint)
              {
                if (value().footprint() == 0)
                  throw Exception("the footprint is null");

                if (value().footprint() != elle::serialize::footprint(value()))
                  throw Exception("the recorded footprint does not match the "
                                  "instance's: value(%s) versus footprint(%s)",
                                  value().footprint(),
                                  elle::serialize::footprint(value()));

                if (value().footprint() > this->nest().limits().extent())
                  throw Exception("the footprint '%s' exceeds the extent '%s'",
                                  value().footprint(),
                                  this->nest().limits().extent());
              }

            // unload the value nodule.
            value.unload();

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_DEBUG_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root nodule.
            root.load();

            // check the address, if required.
            if (flags & flags::address)
              {
                ELLE_DEBUG_SCOPE("checking addresses");

                // bind the current block.
                Address address{root.contents().bind()};

                // compare the addresses.
                if (this->_root.address() != address)
                  throw Exception("invalid address: root(%s) versus bind(%s)",
                                  this->_root.address(), address);
              }

            // trigger the check method on the root nodule.
            if (flags & flags::recursive)
              root().check(flags);

            // check the capacity, if required.
            if (flags & flags::capacity)
              {
                ELLE_DEBUG_SCOPE("checking capacities");

                if (this->_capacity != root().capacity())
                  throw Exception("invalid capacity: this(%s) versus root(%s)",
                                  this->_capacity, root().capacity());
              }

            // check the footprint.
            if (flags & flags::footprint)
              {
                ELLE_DEBUG_SCOPE("checking footprints");

                if (root().footprint() == 0)
                  throw Exception("the footprint is null");

                if (root().footprint() != elle::serialize::footprint(root()))
                  throw Exception("the recorded footprint does not match the "
                                  "instance's: root(%s) versus footprint(%s)",
                                  root().footprint(),
                                  elle::serialize::footprint(root()));

                if (root().footprint() > this->nest().limits().extent())
                  throw Exception("the footprint '%s' exceeds the extent '%s'",
                                  root().footprint(),
                                  this->nest().limits().extent());
              }

            // check the state.
            if (flags & flags::state)
              {
                ELLE_DEBUG_SCOPE("checking states");

                if (this->_state != root().state())
                  throw Exception("invalid state: this(%s) versus root(%s)",
                                  this->_state, root().state());
              }

            // unload the root nodule.
            root.unload();

            break;
          }
        }
    }

    template <typename T>
    void
    Porcupine::walk(elle::Natural32 const margin)
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the mode.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Mode] " << std::dec << this->_mode << std::endl;

      // dump the height.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Height] " << std::dec << this->_height << std::endl;

      // dump the capacity.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity] " << std::dec << this->_capacity << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      // proceed if there is a root block.
      if (this->_root != Handle::Null)
        {
          // dump the value.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Root]" << std::endl;

          switch (this->_mode)
            {
            case Mode::empty:
              {
                break;
              }
            case Mode::single:
              {
                Ambit<T> value(this->nest(), this->_root);

                // dump the value.
                if (this->_root.Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the handle");

                // load the value nodule.
                value.load();

                // dump the value.
                if (value.contents().Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the value");

                // unload the value nodule.
                value.unload();

                break;
              }
            case Mode::hierarchy:
              {
                Ambit<Nodule<T>> root(this->nest(), this->_root);

                // dump the root.
                if (this->_root.Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the handle");

                // load the root nodule.
                root.load();

                // walk through the nodule.
                root().walk(margin + 4);

                // unload the root nodule.
                root.unload();

                break;
              }
            }
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Root] " << elle::none << std::endl;
        }
    }

    /// XXX
    /// this method goes through the porcupine in order to seal the
    /// blocks having been either created or modified.
    ///
    /// once complete, the porcupine should be consistent so for its
    /// constituent blocks to be stored.
    ///
    template <typename T>
    void
    Porcupine::seal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("seal(%s)", secret);

      switch (this->_state)
        {
        case State::clean:
          {
            ELLE_TRACE_SCOPE("State::clean");

            break;
          }
        case State::dirty:
          {
            ELLE_TRACE_SCOPE("State::dirty");

            switch (this->_mode)
              {
              case Mode::empty:
                {
                  ELLE_TRACE_SCOPE("Mode::empty");

                  throw Exception("unable to seal an empty porcupine");
                  break;
                }
              case Mode::single:
                {
                  Ambit<T> value(this->nest(), this->_root);

                  ELLE_TRACE_SCOPE("Mode::single");

                  // load the value nodule.
                  value.load();

                  // if the porcupine is dirty, so should be the value.
                  ELLE_ASSERT(this->_state == value().state());

                  // Encrypt and bind the value.
                  value.contents().encrypt(secret);
                  Address address{value.contents().bind()};

                  value().state(State::consistent);
                  value.contents().state(State::consistent);

                  // unload the value nodule.
                  value.unload();

                  this->_root.address(address);
                  this->_state = State::consistent;

                  break;
                }
              case Mode::hierarchy:
                {
                  Ambit<Nodule<T>> root(this->nest(), this->_root);

                  ELLE_TRACE_SCOPE("Mode::hierarchy");

                  // load the root nodule.
                  root.load();

                  // if the porcupine is dirty, so should be the root.
                  ELLE_ASSERT(this->_state == root().state());

                  // seal recursively.
                  root().seal(secret);

                  // Encrypt and bind the root block.
                  root.contents().encrypt(secret);
                  Address address{root.contents().bind()};

                  root().state(State::consistent);
                  root.contents().state(State::consistent);

                  // unload the root nodule.
                  root.unload();

                  this->_root.address(address);
                  this->_state = State::consistent;

                  break;
                }
              }

            break;
          }
        case State::consistent:
          {
            ELLE_TRACE_SCOPE("State::consistent");

            throw Exception("unable to seal a consistent porcupine");
          }
        }

      // set the secret key as null.
      this->_root.secret(cryptography::SecretKey::Null);
    }

    template <typename T>
    Statistics
    Porcupine::statistics()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      Statistics stats;

      ELLE_TRACE_SCOPE("statistics()");

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            break;
          }
        case Mode::single:
          {
            Ambit<T> value(this->nest(), this->_root);

            ELLE_TRACE_SCOPE("Mode::single");

            value.load();

            stats.blocks_all(stats.blocks_all() + 1);

            switch (value().state())
              {
              case State::clean:
                {
                  stats.blocks_clean(stats.blocks_clean() + 1);

                  break;
                }
              case State::dirty:
                {
                  stats.blocks_dirty(stats.blocks_dirty() + 1);

                  break;
                }
              case State::consistent:
                {
                  stats.blocks_consistent(stats.blocks_consistent() + 1);

                  break;
                }
              }

            Footprint footprint = elle::serialize::footprint(value());
            Capacity capacity = value().capacity();

            stats.footprint_minimum(
              footprint < stats.footprint_minimum() ?
              footprint : stats.footprint_minimum());
            stats.footprint_average(
              (stats.footprint_average() + footprint) / 2);
            stats.footprint_maximum(
              footprint > stats.footprint_maximum() ?
              footprint : stats.footprint_maximum());

            stats.capacity_minimum(
              capacity < stats.capacity_minimum() ?
              capacity : stats.capacity_minimum());
            stats.capacity_average(
              (stats.capacity_average() + capacity) / 2);
            stats.capacity_maximum(
              capacity > stats.capacity_maximum() ?
              capacity : stats.capacity_maximum());

            value.unload();

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root nodule.
            root.load();

            root().statistics(stats);

            // unload the root nodule.
            root.unload();

            break;
          }
        }

      return (stats);
    }

    template <typename T>
    void
    Porcupine::_add(typename T::K const& k,
                    Handle const& v)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("add(%s)", k);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            // In this case, the tree is empty. therefore, adding a value
            // consists in setting the porcupine as containing a single value
            // i.e no hierarchy.
            this->_create<T>(v);

            break;
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            // In this case, the porcupine already contains a value. A
            // hierarchy therefore needs to be created.
            //
            // A quill (i.e leaf node) is allocated in which the current
            // value is inserted.
            //
            // Finally, the given key/value tuple is also inserted.

            std::unique_ptr<Contents> contents(new Contents(new Quill<T>));
            Handle handle(this->nest().attach(std::move(contents)));

            ELLE_ASSERT(this->_root != Handle::Null);

            // Load the created quill, insert the existing value,
            // update the porcupine's characteristics (state, capacity etc.)
            // and unload the quill.
            Ambit<T> value(this->_root);
            typename T::K key;

            value.load();
            key = value().key();
            value.unload();

            Ambit<Quill<T>> newroot(this->nest(), handle);

            newroot.load();

            // Note that calling add() is valid because _newroot_
            // is a quill i.e the call will not be propagated down
            // to the hierarchy since the quill is the bottom of it!
            newroot().add(key, this->_root);

            this->_capacity = newroot().capacity();
            this->_state = newroot().state();

            newroot.unload();

            this->_root = handle;
            this->_height++;
            this->_mode = Mode::hierarchy;

            // Finally, proceed to the addition of the given key/value
            // tuple considering the porcupine as acting as a tree.
            //
            // Do not break since the code below perform such an operation.
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            // In this case, the porcupine already contains a hierarchy of
            // nodules though perhaps a single one i.e a quill.
            //
            // The code below therefore proceeds in inserting the key/value
            // tuple in the hierarchy by calling the virtual method add().
            //
            // Once added, the porcupine's characteristics are updated.
            // Besides, since the tree's configuration has changed, the
            // porcupine is optimised, operation which could result in nodules
            // being split etc.

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            root.load();

            root().add(k, v);

            this->_capacity = root().capacity();
            this->_state = root().state();

            root.unload();

            this->_optimize<T>();

            break;
          }
        }
    }

    template <typename T>
    void
    Porcupine::_create()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("_create()");

      // make sure the tree is empty.
      ELLE_ASSERT(this->_mode == Porcupine::Mode::empty);
      ELLE_ASSERT(this->_root == Handle::Null);
      ELLE_ASSERT(this->_height == 0);

      // create a unique value and set is as the root though in this
      // case it does not represent the root of a tree.

      std::unique_ptr<Contents> contents(new Contents(new T));
      Handle handle(this->nest().attach(std::move(contents)));
      Ambit<T> value(this->nest(), handle);

      value.load();

      // set the porcupine's capacity and state.
      this->_capacity = value().capacity();
      this->_state = State::dirty;

      value.unload();

      this->_root = handle;

      this->_mode = Porcupine::Mode::single;
    }

    template <typename T>
    void
    Porcupine::_create(Handle& handle)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("_create(%s)", &handle);

      // make sure the tree is empty.
      ELLE_ASSERT(this->_mode == Porcupine::Mode::empty);
      ELLE_ASSERT(this->_root == Handle::Null);
      ELLE_ASSERT(this->_height == 0);

      // a new value is provided, set it in the porcupine.
      Ambit<T> value(this->nest(), handle);

      value.load();

      // set the porcupine's capacity and state
      // by computing the value's.
      this->_capacity = value().capacity();
      this->_state = State::dirty;

      value.unload();

      // finally, update the root handle so as to reference the
      // only value.
      this->_root = handle;

      this->_mode = Porcupine::Mode::single;
    }

    template <typename T>
    void
    Porcupine::_optimize()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("_optimize()");

      switch (this->_mode)
        {
        case Mode::empty:
          {
            ELLE_TRACE_SCOPE("Mode::empty");

            throw Exception("unable to optimize an empty porcupine");
            break;
          }
        case Mode::single:
          {
            ELLE_TRACE_SCOPE("Mode::single");

            Ambit<T> value(this->nest(), this->_root);

            // load the value nodule.
            value.load();

            //
            // XXX
            //
            if ((value().footprint()) > this->nest().limits().extent())
              {
                ELLE_TRACE_SCOPE("value's extent high limit reached: "
                                 "%s > %s",
                                 value().footprint(),
                                 this->nest().limits().extent());

                //
                // in this case, the value needs to be split.
                //
                // in order to re-use the code as much as possible, a quill
                // is created in which the value is inserted. then the
                // quill-specific Optimize() method is called so as to
                // split the values.
                //
                typename T::K mayor;

                // retrieve the mayor of the value.
                mayor = value().mayor();

                // unload the value.
                value.unload();

                // allocate a new root quill.
                std::unique_ptr<Contents> contents(new Contents(new Quill<T>));
                Handle orphan(this->nest().attach(std::move(contents)));
                Ambit<Quill<T>> newroot(this->nest(), orphan);

                // load the newroot.
                newroot.load();

                // add the current value to the new quill root.
                //
                // note that calling add() is valid in this case because
                // _newroot_ is a quill i.e the last nodule of the hierarchy.
                newroot().add(mayor, this->_root);

                // now that the tree is valid, call the Optimize() method because
                // the added value is still too large to remain as such and
                // therefore needs to be split.
                Nodule<T>::optimize(newroot(), mayor);

                // update the capacity and state.
                this->_capacity = newroot().capacity();
                this->_state = newroot().state();

                // unload the newroot.
                newroot.unload();

                // assign the new root handle.
                this->_root = orphan;

                // increment the height.
                this->_height++;

                // set the mode.
                this->_mode = Porcupine::Mode::hierarchy;
              }
            else if (value().footprint() <
                     (this->nest().limits().extent() *
                      this->nest().limits().balancing()))
              {
                ELLE_TRACE_SCOPE("value's extent low limit reached: "
                                 "%s < %s",
                                 value().footprint(),
                                 this->nest().limits().extent() *
                                 this->nest().limits().balancing());

                // XXX
                if (value().empty() == true)
                  {
                    ELLE_TRACE_SCOPE("the value is empty");

                    value.unload();

                    // detache the value block.
                    this->nest().detach(this->_root);

                    // reset the root.
                    this->_root = Handle::Null;

                    // make sure the tree is empty.
                    ELLE_ASSERT(this->_capacity == 0);

                    // update the state.
                    this->_state = State::dirty;

                    // set the mode.
                    this->_mode = Porcupine::Mode::empty;
                  }
                else
                  value.unload();
              }
            else
              value.unload();

            break;
          }
        case Mode::hierarchy:
          {
            ELLE_TRACE_SCOPE("Mode::hierarchy");

            Ambit<Nodule<T>> root(this->nest(), this->_root);

            // load the root nodule.
            root.load();

            ELLE_TRACE("considering the root nodule's footprint: "
                       "%s < %s",
                       root().footprint(),
                       this->nest().limits().extent());

            //
            // at this point, check whether the add() produced the a nodule
            // whose size exceed the extent.
            //
            // should this happen, the root block is split, creating a new
            // block. thus, a new root block is created in which both nodules
            // are inserted.
            //
            // check if the future nodule's footprint---i.e should the inlet
            // be inserted---would exceed the extent.
            //
            if ((root().footprint()) > this->nest().limits().extent())
              {
                ELLE_TRACE_SCOPE("root's extent hight limit reached: "
                                 "%s > %s",
                                 root().footprint(),
                                 this->nest().limits().extent());

                //
                // in this case, the nodule needs to be split.
                //
                Handle orphan;

                // split the current root nodule.
                orphan = root().split();

                //
                // at this point, the tree needs to grow in order to welcome
                // the new right nodule.
                //
                struct
                {
                  typename T::K root;
                  typename T::K newright;
                } mayor;
                struct
                {
                  Capacity root;
                  Capacity newright;
                } capacity;
                struct
                {
                  State root;
                  State  newright;
                } state;

                // retrieve the mayor key of the current root.
                mayor.root = root().mayor();

                // retrieve the capacity and state.
                capacity.root = root().capacity();
                state.root = root().state();

                // unload the root nodule.
                root.unload();

                Ambit<Nodule<T>> newright(this->nest(), orphan);

                // load the new right nodule.
                newright.load();

                // retrieve the mayor key of the newright nodule as this will
                // be required for the future insertion in the current seam.
                mayor.newright = newright().mayor();

                // retrieve the capacity and state.
                capacity.newright = newright().capacity();
                state.newright = newright().state();

                // unload the new right nodule.
                newright.unload();

                //
                // at this point, the new i.e orphan nodule must be inserted
                // into the current seam.
                //
                struct
                {
                  Handle root;
                  Handle newroot;
                  Handle newright;
                } handle;

                // keep the new right handle.
                handle.newright = orphan;

                // copy the current root handle so that no ambit reference
                // _this->_root_ since this variable is going to change.
                handle.root = this->_root;

                // allocate a new root seam.
                std::unique_ptr<Contents> contents(new Contents(new Seam<T>));

                // attach the block to the porcupine.
                handle.newroot = this->nest().attach(std::move(contents));

                Ambit<Seam<T>> newroot(this->nest(), handle.newroot);

                // load the newroot.
                newroot.load();

                // below are manually inserted the nodules because add()
                // would perform the operation recursively.

                // insert the current root nodule in the new root seam.
                typename Seam<T>::I* inlet;

                newroot().insert(mayor.root, handle.root);
                inlet = newroot().locate_inlet(mayor.root);
                inlet->capacity(capacity.root);
                inlet->state(state.root);

                newroot().insert(mayor.newright, handle.newright);
                inlet = newroot().locate_inlet(mayor.newright);
                inlet->capacity(capacity.newright);
                inlet->state(state.newright);

                // compute the capacity of the new root seam. note that
                // the state does not need to be set explicitely because
                // the insert() methods took care of it.
                newroot().capacity(capacity.root + capacity.newright);

                // update the capacity and state.
                this->_capacity = newroot().capacity();
                this->_state = newroot().state();

                // unload the newroot.
                newroot.unload();

                // assign the new root handle.
                this->_root = handle.newroot;

                // increment the height.
                this->_height++;
              }
            else
              {
                ELLE_TRACE_SCOPE("root's footprint may be low enough to "
                                 "reorganize the tree");

                ELLE_ASSERT(this->_height != 0);

                if (this->_height == 1)
                  {
                    ELLE_TRACE_SCOPE("the tree contains a single level");

                    // unload the root nodule.
                    root.unload();

                    Ambit<Quill<T>> quill(this->nest(), this->_root);

                    // load the root nodule.
                    quill.load();

                    // does the quill contain a single value.
                    if (quill().single() == true)
                      {
                        ELLE_TRACE_SCOPE("the root quill contains a "
                                         "single entry");

                        //
                        // if the root nodules contains a single entry, shrink
                        // the tree for this entry to become the new root.
                        //
                        Handle orphan;

                        // retrieve the handle associated with the maiden key.
                        orphan = quill().locate_handle(quill().maiden());

                        // detache the root nodule from the porcupine.
                        this->nest().detach(quill.handle());

                        // unload the root nodule.
                        quill.unload();

                        // set the new tree's root handle.
                        this->_root = orphan;

                        // decrease the tree's height.
                        this->_height--;

                        // at this point, the tree should have a height of zero.
                        ELLE_ASSERT(this->_height == 0);

                        // therefore, set the mode as value now that the
                        // root handle actually references a value.
                        this->_mode = Porcupine::Mode::single;
                      }
                    else
                      quill.unload();
                  }
                else if (this->_height > 1)
                  {
                    ELLE_TRACE_SCOPE("the tree contains several levels "
                                     "of hierarchy.");

                    // unload the root nodule.
                    root.unload();

                    Ambit<Seam<T>> seam(this->nest(), this->_root);

                    // load the root nodule.
                    seam.load();

                    // check whether the root seam contains a single
                    // entry in which case this entry could become
                    // the new root.
                    if (seam().single() == true)
                      {
                        ELLE_TRACE_SCOPE("the root seam contains a "
                                         "single entry.");

                        //
                        // if the root nodules contains a single entry, shrink
                        // the tree for this entry to become the new root.
                        //
                        // XXX here we are sure the root is a seam since height > 1
                        //
                        Handle orphan;

                        // retrieve the handle associated with the maiden key.
                        orphan = seam().locate_handle(seam().maiden());

                        // detache the root nodule from the porcupine.
                        this->nest().detach(seam.handle());

                        // unload the root nodule.
                        seam.unload();

                        // set the new tree's root handle.
                        this->_root = orphan;

                        // decrease the tree's height.
                        this->_height--;

                        ELLE_TRACE_SCOPE("try to optimize further the tree");

                        this->_optimize<T>();
                      }
                    else
                      seam.unload();
                  }
              }

            break;
          }
        }
    }

    /// XXX
    /// this method is used for retrieving the handle of the quill
    /// responsible for the given key.
    ///
    /// this method is widely used being for inserting, locating or
    /// deleting entries.
    ///
    template <typename T>
    Handle
    Porcupine::_search(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      Handle v;

      ELLE_TRACE_SCOPE("_search(%s)", k);

      // make sure we are operating in tree mode.
      ELLE_ASSERT(this->_mode == Porcupine::Mode::hierarchy);

      Ambit<Nodule<T>> root(this->nest(), this->_root);

      // load the root nodule.
      root.load();

      // trigger the search method on the root nodule.
      v = root().search(k);

      // unload the root nodule.
      root.unload();

      // first, set the handle as being the root.
      //
      // this is required because, should the root be a quill since the
      // quill search() implementation does nothing.
      //
      // therefore, the whole search() mechanism relies on the fact that
      // the search() method sets the handle temporarily until the child
      // quill does nothing after which the call returns with the temporary
      // handle which happens to reference the quill.
      //
      if (v == Handle::Null)
        v = this->_root;

      return (v);
    }

  }
}

#endif
