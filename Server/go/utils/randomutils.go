package utils

import (
	"crypto/rand"
	"encoding/binary"
)

func RandomInt32() int32 {
	var num int32
	err := binary.Read(rand.Reader, binary.BigEndian, &num)
	if err != nil {
		panic("generate random int32 failed")
	}

	return num
}
