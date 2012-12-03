#ifndef PLASMA_TROPHONIUS_CLIENT_HH
# define PLASMA_TROPHONIUS_CLIENT_HH

# include <plasma/plasma.hh>

# include <elle/HttpClient.hh>
# include <elle/serialize/JSONArchive.hh>

# include <boost/system/error_code.hpp>

# include <functional>
# include <queue>

namespace plasma
{
  namespace trophonius
  {

    namespace json = elle::format::json;

    /// Base class for all notifications.
    struct Notification
    {
      int notification_type;
    };

    struct UserStatusNotification:
      public Notification
    {
      std::string user_id;
      int         status;
    };

    struct TransactionNotification:
      public Notification
    {
      Transaction transaction;
    };

    struct TransactionStatusNotification:
      public Notification
    {
      std::string transaction_id;
      int         status;
    };

    struct MessageNotification:
      public Notification
    {
      std::string sender_id;
      std::string message;
    };

    class Client
    {
    public:
      struct Impl;
      std::unique_ptr<Impl> _impl;

      Client(std::string const& server,
             uint16_t port,
             bool check_error = true);

    public:
      bool
      connect(std::string const& _id,
              std::string const& token);

      //GenericNotification
      std::unique_ptr<Notification>
      poll();

      bool
      has_notification(void);

    private:
      std::queue<std::unique_ptr<Notification>> _notifications;

      void
      _read_socket();

      void
      _on_read_socket(boost::system::error_code const& err,
                      size_t bytes_transferred);
    };
  }
}

#undef _PLASMA_TROPHONIUS_GENERATE_HANDLERS

#endif
