//
// Created by Steel_Shadow on 2023/11/14.
//

#ifndef COMPILER_CIRCULAR_QUEUE_H
#define COMPILER_CIRCULAR_QUEUE_H

#include <iostream>

template <typename T, int MAX_SIZE>
class CircularQueue {
public:
    T* getData() {
        return data;
    }

    CircularQueue() : size(0), front(0), rear(0) {
        data = new T[MAX_SIZE];
    }

    ~CircularQueue() {
        delete[] data;
    }

    void enqueue(const T& val) {
        if (size == MAX_SIZE) {
            // Queue is full, replace the oldest element
            dequeue();
        }

        data[rear] = val;
        rear = (rear + 1) % MAX_SIZE;
        size++;
    }

    void dequeue() {
        if (size > 0) {
            front = (front + 1) % MAX_SIZE;
            size--;
        }
    }

    T frontValue() const {
        if (size > 0) {
            return data[front];
        } else {
            // Handle empty queue case
            std::cerr << "Queue is empty." << '\n';
            return T(); // or some sentinel value indicating an error
        }
    }

    bool isEmpty() const {
        return size == 0;
    }

private:
    T* data;
    int size;
    int front;
    int rear;
};

#endif //COMPILER_CIRCULAR_QUEUE_H
