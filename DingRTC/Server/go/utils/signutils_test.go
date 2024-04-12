package utils

import (
	"encoding/base64"
	"encoding/binary"
	"testing"
)

func Test_GenerateSign(t *testing.T) {
	expected := "dQ0fU32Hqj4gs9VCZoaU5mZtmkas4BHg9qcyZzdZabM="
	secretKey := "secretKeysecretKey"
	ts := int32(1712709815)
	salt := ts
	actualBytes, _ := GenerateSign(secretKey, ts, salt)
	actualStr := base64.StdEncoding.EncodeToString(actualBytes)

	println(actualStr)

	if expected != actualStr {
		t.Error("Error")
	}
}

func Test_Sign(t *testing.T) {
	timestamp := int32(1712709815)
	timestampBytes := make([]byte, IntLength)
	binary.BigEndian.PutUint32(timestampBytes, uint32(timestamp))

	for _, b := range timestampBytes {
		print(b)
	}
}
