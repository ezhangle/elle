#ifndef  ELLE_UTILITY_BUFFER_HH
# define ELLE_UTILITY_BUFFER_HH

# include <elle/types.hh>

# include <elle/io/Dumpable.hh>

# include <elle/serialize/fwd.hh>


namespace elle
{
  namespace utility
  {

    /// Manage a memory zone. Note that this class owns the pointed
    /// memory at every moment.
    ///
    /// @see WeakBuffer for a buffer that doesn't own the data
    class Buffer
      : public elle::io::Dumpable
    {
      friend elle::serialize::ArchiveSerializer<Buffer>;
    private:
      elle::Byte*       _contents;
      size_t            _size;
      size_t            _buffer_size;

    public:
      Buffer();
      Buffer(size_t                 size);

      // Buffer class is moveable
      Buffer(Buffer&&               other);
      Buffer& operator =(Buffer&&   other);

      // Buffer class is Inheritable
      virtual ~Buffer();

    private:
      // Buffer is not copyable
      Buffer(Buffer const&);
      Buffer& operator =(Buffer const&);

    public:
      void                Append(void const* data,
                                 size_t size);

      void                Size(size_t size);
      size_t              Size() const { return this->_size; }
      elle::Byte const*   Contents() const { return this->_contents; }

      elle::serialize::OutputBufferArchive Writer();
      elle::serialize::InputBufferArchive  Reader() const;

      virtual Status    Dump(const Natural32 shift = 0) const;

    private:
      static size_t _NextSize(size_t);
    };


    class WeakBuffer
    {
    private:
      elle::Byte const*   _contents;
      size_t              _size;

    public:
      WeakBuffer(void const* data, size_t size)
        : _contents(static_cast<elle::Byte const*>(data))
        , _size(size)
      {}

      WeakBuffer(WeakBuffer&& other)
        : _contents(other._contents)
        , _size(other._size)
      {}

      size_t              Size() const        { return this->_size; }
      elle::Byte const*   Contents() const    { return this->_contents; }

      elle::serialize::InputBufferArchive  Reader() const;
    };

  }
}

#endif /* ! BUFFER_HH */


