package com.ding.rtc.demo.utils;

import java.util.LinkedList;

public class LimitedSizeLinkedList<E> extends LinkedList<E> {
    private final int MAX_SIZE;

    public LimitedSizeLinkedList(int maxSize) {
        MAX_SIZE = maxSize;
    }

    @Override
    public boolean add(E e) {
        if (size() >= MAX_SIZE) {
            removeFirst();
        }
        return super.add(e);
    }
    @Override
    public void add(int index, E element) {
        if (size() >= MAX_SIZE) {
            removeFirst();
        }
        super.add(index, element);
    }
}
