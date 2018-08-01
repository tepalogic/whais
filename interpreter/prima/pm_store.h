/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Romania
Phone:   +40721939650
e-mail:  popaiulian@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

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
    whais::LockGuard<decltype(mLock)> _l(mLock);
    return new T();
  }

  void Free(T* const obj)
  {
    whais::LockGuard<decltype(mLock)> _l(mLock);
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
using SharedTextStore = Store<std::shared_ptr<DText>>;


} /* namespace: prima */
} /* namespace: whais */


#endif /* PM_STORE_H_ */
