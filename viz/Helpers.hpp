#pragma once
#include <Qt>
#include <QThread>

/**Some helper methods */


#define ASSERT_NOT_NULL(item) \
  do \
  { \
    if(item == nullptr) \
    { \
      throw std::runtime_error(std::string(#item) + " is null"); \
    } \
  }while(false)


namespace envire { namespace viz
{

class Helpers
{
public:
   /**Returns the connection type that should be used to invoke a blocking method on @p obj.
   * I.e. if @p obj lives in the current thread it will return DirectConnection, otherwise
   * BlockingQueuedConnection will be returned*/
  static Qt::ConnectionType determineConnectionType(QObject* obj)
  {
    if(QThread::currentThread() == obj->thread())
    {
      return Qt::DirectConnection;
    }
    return Qt::BlockingQueuedConnection;
  }
};

}}