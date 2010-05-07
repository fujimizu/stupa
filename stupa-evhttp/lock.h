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

#include <pthread.h>
#include <queue>

namespace stupa {

/**
 * Thread pool.
 */
class ThreadPool {
 private:
  typedef std::queue<void *> Queue;
  Queue queue_;
  size_t num_threads_;
  size_t max_task_;
  bool shutdown_;
  pthread_t *threads_;
  pthread_mutex_t queue_mutex_;
  pthread_cond_t queue_cond_;

  void create_threads(void *(*func)(void *)) {
    threads_ = new pthread_t[num_threads_];
    for (size_t i = 0; i < num_threads_; i++) {
      pthread_create(&threads_[i], NULL, func, this);
    }
  }

 public:
  ThreadPool(size_t num_threads, size_t max_task)
    : num_threads_(num_threads), max_task_(max_task), shutdown_(false) {
    pthread_mutex_init(&queue_mutex_, NULL);
    pthread_cond_init(&queue_cond_, NULL);
  }

  ~ThreadPool() {
    clear();
    pthread_cond_broadcast(&queue_cond_);
    for (size_t i = 0; i < num_threads_; i++) {
      pthread_join(threads_[i], NULL);
    }
    delete threads_;
  }

  void start(void *(*func)(void *)) {
    create_threads(func);
    while (true) {
    }
  }

  void add(void *task) {
    pthread_mutex_lock(&queue_mutex_);
    queue_.push(task);
    pthread_cond_signal(&queue_cond_);
    pthread_mutex_unlock(&queue_mutex_);
  }

  void clear() {
    pthread_mutex_lock(&queue_mutex_);
    while (!queue_.empty()) queue_.pop();
    pthread_mutex_unlock(&queue_mutex_);
  }
};

/**
 * Read-Write lock.
 */
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

/**
 * Read-Write lock guard.
 */
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
