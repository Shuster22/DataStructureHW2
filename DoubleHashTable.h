//
// Created by Nadav Kaempfer on 17/01/2026.
//

#ifndef DOUBLEHASHTABLE_H
#define DOUBLEHASHTABLE_H
#define  LOAD_FACTOR 0.7
#include <exception>

enum SlotStatus { EMPTY, OCCUPIED, DELETED };

template <typename K, typename V>
class DoubleHashTable {
private:
    struct Entry {
        K key;
        V value;
        SlotStatus status;

        Entry() : status(EMPTY) {}
    };

    Entry* table;
    int capacity;// always prime
    int size;


    DoubleHashTable(const DoubleHashTable&) = delete;
    DoubleHashTable& operator=(const DoubleHashTable&) = delete;

    DoubleHashTable(DoubleHashTable&& other) noexcept
      : table(other.table), capacity(other.capacity), size(other.size) {
        other.table = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    DoubleHashTable& operator=(DoubleHashTable&& other) noexcept {
        if (this == &other) return *this;
        delete[] table;
        table = other.table;
        capacity = other.capacity;
        size = other.size;
        other.table = nullptr;
        other.capacity = 0;
        other.size = 0;
        return *this;
    }


    bool isPrime(int n) const {
        if (n <= 1) return false;
        if (n % 2 == 0 ) return false;
        // for each number between 2 and then every odd check if devides n
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }

    //find the next prime number after the new array size (n)
    int nextPrime(int n) const {
        if (n <= 1) return 2;
        int prime = n;
        bool found = false;
        while (!found) {
            prime++;
            if (isPrime(prime)) found = true;
        }
        return prime;
    }

    // first hash func
    // the casting is to ensure the key is positive and therefor the mod is >=0
    int h1(const K& key) const {
        return (int)(static_cast<unsigned int>(key) % capacity);
    }

    // second hash table used for Collisions handling
    int h2(const K& key) const {
        // the following formula is to ensure full coverage
        int step = (int)(1 + (static_cast<unsigned int>(key) % (capacity - 1)));
        return step;
    }

    // create a new array (~size(capacity * 2)) and move items there
    void rehash() {
        int oldCapacity = capacity;
        Entry* oldTable = table;

        capacity = nextPrime(capacity * 2);
        table = new Entry[capacity];
        size = 0;

        for (int i = 0; i < oldCapacity; i++) {
            if (oldTable[i].status == OCCUPIED) {
                insert(oldTable[i].key, oldTable[i].value);
            }
        }
        delete[] oldTable;
    }

public:
    DoubleHashTable(int initCapacity = 11) : capacity(initCapacity), size(0) {
        table = new Entry[capacity];
    }

    ~DoubleHashTable() {
        delete[] table;
    }


    void insert(const K& key, const V& value) {
        // check if load factor passed, if yes than rehash
        if ((double)size / capacity >= LOAD_FACTOR) {
            rehash();
        }

        int index = h1(key);
        int step = h2(key);
        int firstDeleted = -1;

        // find the next EMPTY spot
        for (int i = 0; i < capacity; i++) {
            // the index to check
            int current = (index + i * step) % capacity;

            if (table[current].status == EMPTY) {
                // if available first deleted insert there
                int targetIndex = (firstDeleted != -1) ? firstDeleted : current;
                table[targetIndex].key = key;
                table[targetIndex].value = value;
                table[targetIndex].status = OCCUPIED;
                size++;
                return;
            }

            if (table[current].status == OCCUPIED && table[current].key == key) {
                // update value if exist
                table[current].value = value;
                return;
            }

            if (table[current].status == DELETED && firstDeleted == -1) {
                // save first Deleted index for later insertion
                firstDeleted = current;
            }
        }
    }

    // return a pointer to the relevant slot if available, nullptr o.w
    V find(const K& key) {
        int index = h1(key);
        int step = h2(key);

        for (int i = 0; i < capacity; i++) {
            int current = (index + i * step) % capacity;

            if (table[current].status == EMPTY) return -1;
            if (table[current].status == OCCUPIED && table[current].key == key) {
                return (table[current].value);
            }
        }
        return -1;
    }

    // according to a certain key mark a slot as deleted
    void remove(const K& key) {
        int index = h1(key);
        int step = h2(key);

        for (int i = 0; i < capacity; i++) {
            int current = (index + i * step) % capacity;

            if (table[current].status == EMPTY) return;
            if (table[current].status == OCCUPIED && table[current].key == key)
                {
                    table[current].status = DELETED;
                    size--;
                    return;
                }
        }
    }
};

#endif //DOUBLEHASHTABLE_H