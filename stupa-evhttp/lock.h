//
// Thread Lock
//
// Copyright(C) 2010  Mizuki Fujisawa <fujisawa@bayon.cc>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef STUPA_THREAD_H_
#define STUPA_THREAD_H_

namespace stupa {

class ReadWriteLock {
 public:
  /**
   * Constructor.
   */
  ReadWriteLock();

  /**
   * Destructor.
   */
  ~ReadWriteLock();

  /**
   * Aquire a read lock.
   */
  void aquire_read() const;

  /**
   * Aquire a write lock.
   */
  void aquire_write() const;

  /**
   * Release read and write locks.
   */
  void release() const;

 private:
  void *lock_;
};

class RWGuard {
 public:
  /**
   * Constructor.
   * @param rwlock ReadWriteLock object
   * @param write aquire write lock if true
   */
  RWGuard(const ReadWriteLock &rwlock, bool write) : rwlock_(rwlock) {
    if (write) rwlock_.aquire_write();
    else        rwlock_.aquire_read();
  }

  ~RWGuard() {
    rwlock_.release();
  }

 private:
  const ReadWriteLock &rwlock_;
};

} /* namespace stupa */

#endif
