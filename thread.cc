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

#include <pthread.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "thread.h"

namespace stupa {

ReadWriteLock::ReadWriteLock() : lock_(NULL) {
  pthread_rwlock_t *rwlock = new pthread_rwlock_t;
  if (pthread_rwlock_init(rwlock, NULL) != 0) {
    delete rwlock;
    fprintf(stderr, "cannot initialize read write lock\n");
    exit(EXIT_FAILURE);
  }
  lock_ = (void *)rwlock;
}

ReadWriteLock::~ReadWriteLock() {
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)lock_;
  pthread_rwlock_destroy(rwlock);
  delete rwlock;
}

void ReadWriteLock::aquire_read() const {
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)lock_;
  if (pthread_rwlock_rdlock(rwlock) != 0) {
    fprintf(stderr, "cannot aquire a read lock\n");
    exit(EXIT_FAILURE);
  }
}

void ReadWriteLock::aquire_write() const {
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)lock_;
  if (pthread_rwlock_wrlock(rwlock) != 0) {
    fprintf(stderr, "cannot aquire a write lock\n");
    exit(EXIT_FAILURE);
  }
}

void ReadWriteLock::release() const {
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)lock_;
  if (pthread_rwlock_unlock(rwlock) != 0) {
    fprintf(stderr, "cannot realese read write locks\n");
    exit(EXIT_FAILURE);
  }
}

} /* namespace stupa */
