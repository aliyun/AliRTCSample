package utils

import (
	"bytes"
	"testing"
)

func Test_Compress(t *testing.T) {

	ori_data := "hello world!"

	compressBytes, _ := Compress([]byte(ori_data))

	decompressBytes, _ := Decompress(compressBytes)

	if !bytes.Equal([]byte(ori_data), decompressBytes) {
		t.Error("Error")
	}
}
