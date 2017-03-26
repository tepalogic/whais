/*
 * pm_store.h
 *
 *  Created on: Mar 21, 2017
 *      Author: ipopa
 */

#ifndef PM_STORE_H_
#define PM_STORE_H_

#include "utils/wthread.h"
#include "dbs/dbs_values.h"


namespace whais {
namespace prima {

template<class T>
class Store
{
public:
  T* Alloc()
  {
    whais::LockRAII<decltype(mLock)> _l(mLock);
    return new T();
  }

  void Free(T* const obj)
  {
    whais::LockRAII<decltype(mLock)> _l(mLock);
    delete obj;
  }

  static Store<T>& Instance()
  {
    static Store<T> instance;
    return instance;
  }

protected:
  Store<T>() = default;

  whais::Lock       mLock;
};

using SharedArrayStore = Store<std::shared_ptr<DArray>>;


} /* namespace: prima */
} /* namespace: whais */


#endif /* PM_STORE_H_ */
